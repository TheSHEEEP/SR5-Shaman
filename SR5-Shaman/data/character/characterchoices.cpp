#include "characterchoices.h"

#include <QDebug>
#include <QString>
#include <QColor>

#include "charactervalues.h"
#include "effectregistry.h"
#include "data/appstatus.h"
#include "data/dictionary.h"

CharacterChoices* CharacterChoices::_instance = 0;

//---------------------------------------------------------------------------------
CharacterChoices::CharacterChoices()
    : _metatypeID("")
    , _magicUserType("")
    , _purchasedPowerPoints(0)
{
    _selectedPriorities.resize(5, PRIORITY_INVALID);
}

//---------------------------------------------------------------------------------
void
CharacterChoices::setPriority(int p_priorityIndex, Priority p_prio)
{
    // Make sure non-magic stays at lowest priority
    bool magicUser = getIsMagicUser();

    // Make sure we only have this priority once
    // Also get the old index of this priority
    int oldPrioIndex = -1;
    for (int i = 0; i < 5; ++i)
    {
        if (_selectedPriorities[i] == p_prio)
        {
            oldPrioIndex = i;
            if (i != p_priorityIndex)
            {
                _selectedPriorities[i] = PRIORITY_INVALID;
            }
        }
    }

    // Set the priotiy
    _selectedPriorities[p_priorityIndex] = p_prio;

    // Make sure non-magic stays at lowest priority
    if (!magicUser && p_priorityIndex != 4 && p_prio != PRIORITY_MAGIC)
    {
        setIsMagicUser(false);
    }

    // If this is a "downgrade" we possibly need to clean up spent points, spells, etc.
    if (oldPrioIndex < p_priorityIndex)
    {
        if (p_prio == PRIORITY_ATTRIBUTES)
        {
            cleanUpSpentAttributes(false);
        }
        else if (p_prio == PRIORITY_METATYPE)
        {
            cleanUpSpentAttributes(true);
        }
        else if (p_prio == PRIORITY_MAGIC)
        {
            cleanUpSpentAttributes(true);
            resetFreeSkills();
            resetFreeSpells();
        }
    }
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getSpentKarma() const
{
    int result = 0;

    // From attribute increases
    QMap<QString, int>::const_iterator it = _attributeIncreasesKarma.begin();
    while (it != _attributeIncreasesKarma.end())
    {
        int valueWithoutKarma = METATYPE_RULES->getDefinition(getMetatypeID()).attributesMin[it.key()];

        // Mind possible magic bonus starting value
        if (getIsMagicUser() &&
            it.key() == "magic")
        {
            valueWithoutKarma += MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID())
                    .priorities[getPriorityIndex(PRIORITY_MAGIC)]->startingMagic;
        }

        if (_attributeIncreasesFreebies.find(it.key()) != _attributeIncreasesFreebies.end())
        {
            valueWithoutKarma += _attributeIncreasesFreebies[it.key()];
        }
        result += ATTRIBUTE_RULES->calculateAttributeIncreaseCost(valueWithoutKarma,
                                                                  CHARACTER_VALUES->getAttribute(it.key(), false, false));
        ++it;
    }

    // From purchased power points
    result += _purchasedPowerPoints * MAGIC_RULES->getPowerPointPurchaseCost();

    // From qualities
    result += getQualitiesKarma(true);

    // From skill increases
    it = _skillIncreasesKarma.begin();
    while (it != _skillIncreasesKarma.end())
    {
        // Get the pure value
        int pureValue = CHARACTER_VALUES->getSkill(it.key(), false);

        // Add the cost from the pure value to the pure value + karma increases
        result += SKILL_RULES->calculateSkillIncreaseCost(it.key(), pureValue - it.value(), pureValue);

        ++it;
    }

    return result;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableKarma() const
{
    return CHARACTER_VALUES->getKarmaPool() - getSpentKarma() + getQualitiesKarma(false);
}

//---------------------------------------------------------------------------------
void
CharacterChoices::increaseAttribute(const QString& p_attribute, int p_increase, int p_fromFreebies)
{
    // Sanity check 1 - metatype
    if (getMetatypeID() == "")
    {
        qWarning() << QString("Cannot increase attributes when there is no metatype chosen.")
                            .arg(p_attribute);
        return;
    }

    // Sanity check 2 - passed values
    if (p_fromFreebies > p_increase)
    {
        qWarning() << QString("Passed fromFreebies (%1) is bigger than passed increase (%2). Reducing freebies to %2")
                      .arg(p_fromFreebies, p_increase);
        p_fromFreebies = p_increase;
    }

    // Get the current unmodified attribute value
    int valueCurrent = CHARACTER_VALUES->getAttribute(p_attribute, false, false);
    int attemptedIncrease = p_increase;
    int wouldBeValue = valueCurrent + attemptedIncrease;

    // Check for decrease first, it's far easier to do
    int naturalMin = METATYPE_RULES->getDefinition(getMetatypeID()).attributesMin[p_attribute];
    QMap<QString, int>::iterator increaseIt;
    if (attemptedIncrease < 0)
    {
        attemptedIncrease *= -1;

        // Check if below the natural minimum
        if (wouldBeValue < naturalMin)
        {
            APPSTATUS->setStatusBarMessage(tr("Trying to reduce %1 below natural minimum. Set to minimum.")
                                                .arg(p_attribute),
                                           5.0f, APPSTATUS->getHelperColors().statusBarMessage);
            _attributeIncreasesFreebies[p_attribute] = 0;
            _attributeIncreasesKarma[p_attribute] = 0;
            return;
        }

        // Reduce as far as possible
        // Subtract from Karma first
        increaseIt = _attributeIncreasesKarma.find(p_attribute);
        if (increaseIt != _attributeIncreasesKarma.end())
        {
            (*increaseIt) -= attemptedIncrease;
            // Get leftover
            if ((*increaseIt) < 0)
            {
                attemptedIncrease = -(*increaseIt);
                (*increaseIt) = 0;
            }
            else
            {
                attemptedIncrease = 0;
            }
        }

        // Subtract from freebies
        _attributeIncreasesFreebies[p_attribute] -= attemptedIncrease;
        return;
    } // END decrease

    // Check if an increase would be over the natural maximum
    int naturalMax = CHARACTER_VALUES->getAttributeMax(p_attribute);
    if (wouldBeValue > naturalMax)
    {
        APPSTATUS->setStatusBarMessage(tr("Trying to increase %1 above natural maximum. Set to maximum.").arg(p_attribute),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        attemptedIncrease = naturalMax - valueCurrent;
        if (p_fromFreebies > attemptedIncrease)
        {
            p_fromFreebies = attemptedIncrease;
        }
        wouldBeValue = valueCurrent + attemptedIncrease;
    }

    // No increase possible? Do nothing
    if (attemptedIncrease == 0)
    {
        return;
    }

    // Get available free attribute points from correct pool
    int availableFreebies = 0;
    if (p_attribute == "edge" || p_attribute == "magic")
    {
        availableFreebies = getAvailableSpecialAttributePoints();
    }
    else
    {
        availableFreebies = getAvailableAttributePoints();
    }
    int actualFromFreebies = p_fromFreebies > availableFreebies ? availableFreebies : p_fromFreebies;
    if (actualFromFreebies > attemptedIncrease)
    {
        actualFromFreebies = attemptedIncrease;
    }

    // Number of increases from karma
    int fromKarma = attemptedIncrease - actualFromFreebies;

    // If the available amount of freebies is lower than the input, try to get the rest from karma points
    if (actualFromFreebies > 0 &&
        actualFromFreebies < p_fromFreebies)
    {
        fromKarma += p_fromFreebies - actualFromFreebies;
    }

    // Calculate karma cost
    int karmaCost = 0;
    if (fromKarma > 0)
    {
        karmaCost = ATTRIBUTE_RULES->calculateAttributeIncreaseCost(valueCurrent + actualFromFreebies, wouldBeValue);
    }

    // Do we have enough karma?
    int availableKarma = CHARACTER_VALUES->getKarmaPool() - getSpentKarma();
    if (karmaCost > availableKarma)
    {
        // Get the maximum we can increase to and recalculate the values
        fromKarma = ATTRIBUTE_RULES->calculateMaximumAttributeIncrease(valueCurrent + actualFromFreebies,
                                                                       naturalMax,
                                                                       availableKarma);
        attemptedIncrease = actualFromFreebies + fromKarma;
        wouldBeValue = valueCurrent + attemptedIncrease;
        karmaCost = ATTRIBUTE_RULES->calculateAttributeIncreaseCost(valueCurrent + actualFromFreebies, wouldBeValue);
    }

    // If the increase is now 0, it means we do not have enough attribute/karma points
    if (attemptedIncrease == 0)
    {
        APPSTATUS->setStatusBarMessage(tr("Not enough attribute or karma points to increase %1 further.").arg(p_attribute),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        return;
    }

    // Finally do the increase!
    increaseIt = _attributeIncreasesFreebies.find(p_attribute);
    if (increaseIt == _attributeIncreasesFreebies.end())
    {
        increaseIt = _attributeIncreasesFreebies.insert(p_attribute, 0);
    }
    (*increaseIt) += actualFromFreebies;

    increaseIt = _attributeIncreasesKarma.find(p_attribute);
    if (increaseIt == _attributeIncreasesKarma.end())
    {
        increaseIt = _attributeIncreasesKarma.insert(p_attribute, 0);
    }
    (*increaseIt) += fromKarma;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAttributeIncreases(const QString& p_attribute, bool p_fromFreebies, bool p_fromKarma) const
{
    int result = 0;
    if (p_fromFreebies && _attributeIncreasesFreebies.find(p_attribute) != _attributeIncreasesFreebies.end())
    {
        result += _attributeIncreasesFreebies[p_attribute];
    }
    if (p_fromKarma && _attributeIncreasesKarma.find(p_attribute) != _attributeIncreasesKarma.end())
    {
        result += _attributeIncreasesKarma[p_attribute];
    }
    return result;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableAttributePoints() const
{
    // Sanity check - priority
    if (getPriorityIndex(PRIORITY_ATTRIBUTES) == -1)
    {
        qWarning() << QString("Cannot get the number of available attribute points when attributes have no priority selected.");
        return 0;
    }

    // Get the available amount
    int result = ATTRIBUTE_RULES->getNumFreebies(getPriorityIndex(PRIORITY_ATTRIBUTES));

    // Subtract the spent amount
    QMap<QString, int>::const_iterator it = _attributeIncreasesFreebies.begin();
    while (it != _attributeIncreasesFreebies.end())
    {
        if (it.key() != "edge" && it.key() != "magic")
        {
            result -= *it;
        }
        ++it;
    }

    return result;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableSpecialAttributePoints() const
{
    // Sanity check 1 - priority
    if (getPriorityIndex(PRIORITY_ATTRIBUTES) == -1)
    {
        qWarning() << QString("Cannot get the number of available special attribute points when attributes have no priority selected.");
        return 0;
    }
    // Sanity check 2 - metatype
    if (getMetatypeID() == "")
    {
        qWarning() << QString("Cannot get the number of available special attribute points when no metatype is selected.");
        return 0;
    }
    // Sanity check 3 - metatype prio
    if (getPriorityIndex(PRIORITY_METATYPE) == -1)
    {
        qWarning() << QString("Cannot get the number of available special attribute points when metatype has no priority selected.");
        return 0;
    }

    // Get the available amount
    int result = METATYPE_RULES->getDefinition(getMetatypeID()).specialAttribPointsPerPrio[getPriorityIndex(PRIORITY_METATYPE)];

    // Subtract the spent amount
    QMap<QString, int>::const_iterator it = _attributeIncreasesFreebies.begin();
    while (it != _attributeIncreasesFreebies.end())
    {
        if (it.key() == "edge" || it.key() == "magic")
        {
            result -= *it;
        }
        ++it;
    }

    return result;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::cleanUpSpentAttributes(bool p_cleanUpSpecialPoints)
{
    // TODO: In theory, we could iterate over all attributes, keeping as many spent
    //          points as possible, but that would require knowing which attributes the player favors.
    if (!p_cleanUpSpecialPoints)
    {
        resetAttributeIncreases("body");
        resetAttributeIncreases("agility");
        resetAttributeIncreases("reaction");
        resetAttributeIncreases("strength");
        resetAttributeIncreases("willpower");
        resetAttributeIncreases("logic");
        resetAttributeIncreases("intuition");
        resetAttributeIncreases("charisma");
    }
    else
    {
        resetAttributeIncreases("magic");
        resetAttributeIncreases("edge");
    }
}

//---------------------------------------------------------------------------------
void
CharacterChoices::resetAttributeIncreases(const QString p_attribute, bool p_fromFreebies, bool p_fromKarma)
{
    // Sanity check - attribute name
    if (!ATTRIBUTE_RULES->isValidAttribute(p_attribute))
    {
        qWarning() << QString("Attribute \"%1\" is not a valid base attribute.")
                            .arg(p_attribute);
        return;
    }

    // Reset from freebies
    if (p_fromFreebies)
    {
        _attributeIncreasesFreebies.remove(p_attribute);
    }

    // Reset from karma
    if (p_fromKarma)
    {
        _attributeIncreasesKarma.remove(p_attribute);
    }
}

//---------------------------------------------------------------------------------
void
CharacterChoices::increaseSkill(const QString& p_skill, int p_numIncreases)
{
    int currentValue = CHARACTER_VALUES->getSkill(p_skill);
    int minimumValue = _skillIncreasesFreebies.contains(p_skill) ? _skillIncreasesFreebies[p_skill] : 0;
    int maxValue = CHARACTER_VALUES->getSkillMax(p_skill);
    int actualIncreases = p_numIncreases;

    // Make sure to not increase higher than maximum, or lower than minimum
    if ((currentValue + actualIncreases) > maxValue)
    {
        APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("SKILL_INC_ABOVE_MAX").arg(p_skill),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        actualIncreases = maxValue - currentValue;
    }
    else if ((currentValue + actualIncreases) < minimumValue)
    {
        APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("SKILL_INC_BELOW_MIN").arg(p_skill),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        actualIncreases = minimumValue - currentValue;
    }

    // Do nothing on no change
    if (actualIncreases == 0)
    {
        return;
    }

    // Increase
    if (actualIncreases > 0)
    {
        // Use available skill points first
        int availableSkillPoints = getAvailableSkillPoints(SKILL_RULES->getDefinition(p_skill).isGroup);
        int currentValuePure = CHARACTER_VALUES->getSkill(p_skill, false);
        int increasesFromPoints = actualIncreases;
        int increasesFromKarma = 0;
        int wouldBeValue = currentValuePure + actualIncreases;

        // Check if we need karma, and if so, how much
        if (increasesFromPoints > availableSkillPoints)
        {
            increasesFromPoints = availableSkillPoints;
            increasesFromKarma = actualIncreases - increasesFromPoints;

            // Get required karma
            int availableKarma = getAvailableKarma();
            int karmaCost =
                    SKILL_RULES->calculateSkillIncreaseCost(p_skill,
                                                            currentValuePure + increasesFromPoints,
                                                            wouldBeValue);

            // If we don't have enough karma, calculate the maximum increase
            if (karmaCost > availableKarma)
            {
                increasesFromKarma = SKILL_RULES->calculateMaximumSkillIncrease(p_skill,
                                                                                currentValuePure + increasesFromPoints,
                                                                                maxValue,
                                                                                availableKarma);
                actualIncreases = increasesFromPoints + increasesFromKarma;

                APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("SKILL_INC_NOT_ENOUGH_KARMA")
                                               .arg(p_skill).arg(wouldBeValue)
                                               .arg(currentValuePure + actualIncreases),
                                               5.0f,
                                               APPSTATUS->getHelperColors().statusBarMessage);
                wouldBeValue = currentValuePure + actualIncreases;
            }
        }

        // Apply increase
        if (!_skillIncreasesKarma.contains(p_skill))
        {
            _skillIncreasesKarma[p_skill] = 0;
        }
        _skillIncreasesKarma[p_skill] += increasesFromKarma;
        if (!_skillIncreasesSkillPoints.contains(p_skill))
        {
            _skillIncreasesSkillPoints[p_skill] = 0;
        }
        _skillIncreasesSkillPoints[p_skill] += increasesFromPoints;
    }
    // Decrease
    else
    {
        int decreases = -actualIncreases;

        // Decrease from karma first
        if (_skillIncreasesKarma.contains(p_skill))
        {
            int oldIncreases = _skillIncreasesKarma[p_skill];
            if (oldIncreases > decreases)
            {
                _skillIncreasesKarma[p_skill] = oldIncreases - decreases;
                return;
            }
            else
            {
                decreases -= oldIncreases;
                _skillIncreasesKarma.erase(_skillIncreasesKarma.find(p_skill));
            }
        }

        // If there are decreases left, decrease from skill points
        if (decreases > 0)
        {
            if (_skillIncreasesSkillPoints.contains(p_skill))
            {
                int increases = _skillIncreasesSkillPoints[p_skill];
                increases -= decreases;

                if (increases <= 0)
                {
                    _skillIncreasesSkillPoints.erase(_skillIncreasesSkillPoints.find(p_skill));
                }
                else
                {
                    _skillIncreasesSkillPoints[p_skill] = increases;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getSkillIncreases(const QString& p_skill,
                                    bool p_fromFreebies,
                                    bool p_fromSkillPoints,
                                    bool p_fromKarma) const
{
    int result = 0;
    if (p_fromFreebies && _skillIncreasesFreebies.find(p_skill) != _skillIncreasesFreebies.end())
    {
        result += _skillIncreasesFreebies[p_skill];
    }
    if (p_fromSkillPoints && _skillIncreasesSkillPoints.find(p_skill) != _skillIncreasesSkillPoints.end())
    {
        result += _skillIncreasesSkillPoints[p_skill];
    }
    if (p_fromKarma && _skillIncreasesKarma.find(p_skill) != _skillIncreasesKarma.end())
    {
        result += _skillIncreasesKarma[p_skill];
    }
    return result;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableFreeSkills(bool p_skillGroups) const
{
    int maxFreeSkills = 0;

    // Get free skills from magic user
    if (getIsMagicUser())
    {
        std::pair<int, int> freeSkills;
        if (!p_skillGroups)
        {
            freeSkills = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID()).priorities[getPriorityIndex(PRIORITY_MAGIC)]
                            ->freeSkills;
        }
        else
        {
            freeSkills = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID()).priorities[getPriorityIndex(PRIORITY_MAGIC)]
                    ->freeSkillGroup;
        }

        if (freeSkills.first > 0)
        {
            maxFreeSkills += freeSkills.first;
        }
    }

    // Stop here when we have no free skills
    if (maxFreeSkills == 0)
    {
        return 0;
    }

    // Get the number of spent free skills
    int spentSkills = 0;
    QMap<QString, int>::const_iterator it;
    for (it = _skillIncreasesFreebies.begin(); it != _skillIncreasesFreebies.end(); ++it)
    {
        // Every increase, no matter the actual increase value counts as one spent free skill
        // TODO: This might change in the future and require a more deep look at the nature of the increase
        ++spentSkills;
    }

    return maxFreeSkills - spentSkills;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableSkillPoints(bool p_groupPoints) const
{
    // Sanity check - priority
    if (getPriorityIndex(PRIORITY_SKILLS) == -1)
    {
        qWarning() << QString("Cannot get the number of available skill points when attributes have no priority selected.");
        return 0;
    }

    // Get the available amount
    int result = SKILL_RULES->getNumSkillPoints(getPriorityIndex(PRIORITY_SKILLS), p_groupPoints);

    // Subtract the spent amount
    QMap<QString, int>::const_iterator it = _skillIncreasesSkillPoints.begin();
    while (it != _skillIncreasesSkillPoints.end())
    {
        if (SKILL_RULES->getDefinition(it.key()).isGroup == p_groupPoints)
        {
            result -= *it;
        }
        ++it;
    }

    return result;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::addFreeSkill(const QString& p_id, Priority p_source)
{
    // Check we do not already have this skill as a free skill increase
    if (_skillIncreasesFreebies.contains(p_id))
    {
        qWarning() << QString("Could not add free skill %1. Skill is already added as a free skill.")
                            .arg(p_id);
        return;
    }

    // Most likely source is magic
    if (p_source == PRIORITY_MAGIC)
    {
        // Sanity check - magic user
        if (!getIsMagicUser())
        {
            qWarning() << QString("Could not add free skill %1. Magic priority given, but character is no magic user.")
                                .arg(p_id);
            return;
        }

        // Get the skill definition
        const SkillDefinition& skillDef = SKILL_RULES->getDefinition(p_id);

        // Get the skill value
        int skillValue = 0;
        if (!skillDef.isGroup)
        {
            skillValue = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID()).priorities[getPriorityIndex(PRIORITY_MAGIC)]
                            ->freeSkills.second;
        }
        else
        {
            skillValue = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID()).priorities[getPriorityIndex(PRIORITY_MAGIC)]
                                        ->freeSkillGroup.second;
        }

        // No free skills available!
        if (skillValue <= 0)
        {
            qWarning() << QString("Could not add free skill %1. Magic type %2 does not have free skills at priority %3.")
                                .arg(p_id)
                                .arg(getMagicUserTypeID())
                                .arg(getPriorityIndex(PRIORITY_MAGIC));
            return;
        }

        // Apply free skill
        _skillIncreasesFreebies[p_id] = skillValue;
    }
}

//---------------------------------------------------------------------------------
void
CharacterChoices::removeFreeSkill(const QString& p_id)
{
    // Check if the skill is actually there
    if (!_skillIncreasesFreebies.contains(p_id))
    {
        return;
    }

    _skillIncreasesFreebies.remove(p_id);
}

//---------------------------------------------------------------------------------
QStringList
CharacterChoices::getChosenFreeSkills()
{
    QStringList result;
    for (int i = 0; i < _skillIncreasesFreebies.keys().size(); ++i)
    {
        result.push_back(_skillIncreasesFreebies.keys()[i]);
    }
    return result;
}

//---------------------------------------------------------------------------------
bool
CharacterChoices::addFreeSpell(const QString& p_id)
{
    // Sanity check - magic user
    if (!getIsMagicUser())
    {
        qWarning() << QString("Could not add free spell %1. Character is no magic user.")
                            .arg(p_id);
        return false;
    }

    // Get the spell definition
    const MagicAbilityDefinition& spellDef = MAGIC_RULES->getDefinition(p_id);

    // Prevent too many additions
    float currentValue = 0.0f;
    if (_spellsFromFreebies.contains(p_id))
    {
        currentValue = _spellsFromFreebies[p_id];

        // Spells and complex forms can only be added once
        if (spellDef.abilityType == MAGICABILITYTYPE_SPELL ||
            spellDef.abilityType == MAGICABILITYTYPE_COMPLEX_FORM)
        {
            APPSTATUS->setStatusBarMessage(tr("Could not add free spell/complex form %1. Spell/complex form is already added as a free spell.")
                                                .arg(p_id),
                                           5.0f,
                                           APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
        // Adept powers can only be added once, a certain number of times or unlimited
        // all depending on the type.
        else if (spellDef.abilityType == MAGICABILITYTYPE_ADEPT_POWER)
        {
            if (spellDef.adeptPower->costType == COSTTYPE_NORMAL ||
                    (spellDef.adeptPower->costType == COSTTYPE_ARRAY &&
                     _spellsFromFreebies[p_id] >= spellDef.adeptPower->costArray.back()))
            {
                APPSTATUS->setStatusBarMessage(tr("Could not add adept power %1. Power exceeds valid points.")
                                                    .arg(p_id),
                                               5.0f,
                                               APPSTATUS->getHelperColors().statusBarMessage);
                return false;
            }
        }
    }

    // Get the spell value
    float influencingSpellValue = 0.0f;
    float spellValue = 0.0f;
    switch (spellDef.abilityType)
    {
        // Most spells just cost exactly 1 point
        case MAGICABILITYTYPE_SPELL:
        case MAGICABILITYTYPE_COMPLEX_FORM:
        default:
            spellValue = 1.0f;
        break;

        // Adept powers have varying cost types
        case MAGICABILITYTYPE_ADEPT_POWER:
            switch ( spellDef.adeptPower->costType)
            {
                case COSTTYPE_NORMAL:
                case COSTTYPE_PER_LEVEL:
                default:
                    spellValue = spellDef.adeptPower->costArray.back();
                break;

                case COSTTYPE_ARRAY:
                {
                    if (_spellsFromFreebies.contains(p_id))
                    {
                        influencingSpellValue = currentValue;
                    }

                    // Get the next index cost
                    for (unsigned int i = 0; i < spellDef.adeptPower->costArray.size(); ++i)
                    {
                        if (spellDef.adeptPower->costArray[i] > currentValue)
                        {
                            spellValue = spellDef.adeptPower->costArray[i];
                            break;
                        }
                    }

                    // If we reach this point and the value is still 0,
                    // it means we already are at the max value
                    if (spellValue <= 0.0f)
                    {
                        APPSTATUS->setStatusBarMessage(tr("Could not add adept power %1. Power exceeds valid points.")
                                                            .arg(p_id),
                                                       5.0f,
                                                       APPSTATUS->getHelperColors().statusBarMessage);
                        return false;
                    }
                }
                break;
            }
        break;
    }

    // Get available free spell points
    float availableFreePoints = 0.0f;
    if (spellDef.abilityType == MAGICABILITYTYPE_SPELL || spellDef.abilityType == MAGICABILITYTYPE_COMPLEX_FORM)
    {
        availableFreePoints = getAvailableFreeSpells();
    }
    // If this an adept power, it is not really a free spell, but purchased from power points
    if (spellDef.abilityType == MAGICABILITYTYPE_ADEPT_POWER)
    {
        availableFreePoints = getAvailablePowerPoints();
    }

    // No free skills available!
    if (availableFreePoints - (spellValue - influencingSpellValue) < 0.0f)
    {
        APPSTATUS->setStatusBarMessage(tr("Could not add free spell %1. Not enough spell points remaining.")
                                            .arg(p_id),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        return false;
    }

    // Magic abilities can possibly have effects, so apply these
    // First, check if all can be applied, as all have to be valid
    bool allValid = true;
    QString lastError = "";
    for (unsigned int i = 0; i < spellDef.effects.size(); ++i)
    {
        // If the spell is user defined, give the custom choice as target, otherwise, the ID
        QString target = spellDef.isUserDefined ? spellDef.customString : spellDef.id;
        if (!spellDef.effects[i]->canBeApplied(target))
        {
            allValid = false;
            lastError = spellDef.effects[i]->getError();
            APPSTATUS->setStatusBarMessage(tr("Could not add free spell %1. Effect could not be applied:\n%2")
                                                .arg(p_id)
                                                .arg(lastError),
                                           5.0f,
                                           APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
    }
    // Apply and add the effects
    if (allValid)
    {
        for (unsigned int i = 0; i < spellDef.effects.size(); ++i)
        {
            EFFECT_REGISTRY->addActiveEffect(spellDef.effects[i]);
        }
    }

    // Apply free skill
    switch (spellDef.abilityType)
    {
        case MAGICABILITYTYPE_SPELL:
        case MAGICABILITYTYPE_COMPLEX_FORM:
        default:
            _spellsFromFreebies[p_id] = spellValue;
        break;

        case MAGICABILITYTYPE_ADEPT_POWER:
            switch (spellDef.adeptPower->costType)
            {
                case COSTTYPE_NORMAL:
                case COSTTYPE_ARRAY:
                default:
                    _spellsFromFreebies[p_id] = spellValue;
                break;

                case COSTTYPE_PER_LEVEL:
                    if (_spellsFromFreebies.contains(p_id))
                    {
                        _spellsFromFreebies[p_id] += spellValue;
                    }
                    else
                    {
                        _spellsFromFreebies[p_id] = spellValue;
                    }
                break;
            }
        break;
    }

    return true;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::removeFreeSpell(const QString& p_id)
{
    // Check if the skill is actually there
    if (!_spellsFromFreebies.contains(p_id))
    {
        return;
    }

    _spellsFromFreebies.remove(p_id);

    // If the removed spell had active effects, remove those, too
    const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(p_id);
    if (def.effects.size() > 0)
    {
        for (unsigned int i = 0; i < def.effects.size(); ++i)
        {
            EFFECT_REGISTRY->removeActiveEffect(def.effects[i]);
        }
    }
}

//---------------------------------------------------------------------------------
void
CharacterChoices::resetFreeSpells()
{
    for (int j = 0; j < _spellsFromFreebies.keys().size(); ++j)
    {
        // If the removed spell had active effects, remove those, too
        const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(_spellsFromFreebies.keys()[j]);
        if (def.effects.size() > 0)
        {
            for (unsigned int i = 0; i < def.effects.size(); ++i)
            {
                EFFECT_REGISTRY->removeActiveEffect(def.effects[i]);
            }
        }
    }

    _spellsFromFreebies.clear();
}

//---------------------------------------------------------------------------------
float
CharacterChoices::getAvailableFreeSpells() const
{
    float maxFreeSpells = 0;

    // Get free spells from magic user
    if (getIsMagicUser())
    {
        const MagicTypeDefinition& def = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID());

        // For magicians and technomancers, we get free spells
        // This also works for mystical adepts, as those do not get any free power points
        if (std::find(def.types.begin(), def.types.end(), "magic") != def.types.end() ||
            std::find(def.types.begin(), def.types.end(), "resonance") != def.types.end())
        {
            maxFreeSpells = def.priorities[getPriorityIndex(PRIORITY_MAGIC)]->freeSpells;
        }
    }

    // Stop here when we have no free spells
    if (maxFreeSpells <= 0.0f)
    {
        return 0.0f;
    }

    // Get the number of spent free spells
    float spentSpells = 0;
    QMap<QString, float>::const_iterator it;
    for (it = _spellsFromFreebies.begin(); it != _spellsFromFreebies.end(); ++it)
    {
        // Do not count adept powers, as those are subtracted from power points instead
        const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(it.key());
        if (def.abilityType != MAGICABILITYTYPE_ADEPT_POWER)
        {
            spentSpells += *it;
        }
    }

    return maxFreeSpells - spentSpells;
}

//---------------------------------------------------------------------------------
float
CharacterChoices::getSpellFreebies(const QString& p_id)
{
    if (!_spellsFromFreebies.contains(p_id))
    {
        return 0.0f;
    }

    return _spellsFromFreebies[p_id];
}

//---------------------------------------------------------------------------------
QStringList
CharacterChoices::getChosenFreeSpells()
{
    QStringList result;
    for (int i = 0; i < _spellsFromFreebies.keys().size(); ++i)
    {
        result.push_back(_spellsFromFreebies.keys()[i]);
    }
    return result;
}

//---------------------------------------------------------------------------------
float
CharacterChoices::getPowerPoints() const
{
    float value = 0.0f;

    // Get free spells from magic user
    if (getIsMagicUser())
    {
        const MagicTypeDefinition& def = MAGIC_RULES->getMagicTypeDefinition(getMagicUserTypeID());

        // Only "pure" adepts get free power points equal to their magic rating
        // Mystical adepts must purchase them
        if (std::find(def.types.begin(), def.types.end(), "adept") != def.types.end() &&
            std::find(def.types.begin(), def.types.end(), "magic") == def.types.end())
        {
            value = CHARACTER_VALUES->getAttribute("magic");
        }
    }

    // Purchased power points
    value += _purchasedPowerPoints;

    return value;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::setPurchasePowerPoints(int p_targetValue)
{
    // If this is an increase, make sure we have enough karma
    if (p_targetValue > _purchasedPowerPoints)
    {
        int cost = (p_targetValue - _purchasedPowerPoints) * MAGIC_RULES->getPowerPointPurchaseCost();
        if (cost > getAvailableKarma())
        {
            APPSTATUS->setStatusBarMessage(tr("Not enough karma to purchase that many power points. Setting to max."),
                                           5.0f,
                                           APPSTATUS->getHelperColors().statusBarMessage);

            // Calculate maximum
            p_targetValue--;
            while (p_targetValue > _purchasedPowerPoints)
            {
                int cost = (p_targetValue - _purchasedPowerPoints) * MAGIC_RULES->getPowerPointPurchaseCost();
                if (cost <= getAvailableKarma())
                {
                    break;
                }
                p_targetValue--;
            }
        }
    }

    _purchasedPowerPoints = p_targetValue;
    if (_purchasedPowerPoints < 0)
    {
        APPSTATUS->setStatusBarMessage(tr("Can't purchase negative power points value: %1. Set to 0.")
                                            .arg(p_targetValue),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        _purchasedPowerPoints = 0;
    }
    else if (_purchasedPowerPoints > CHARACTER_VALUES->getAttribute("magic"))
    {
        APPSTATUS->setStatusBarMessage(tr("Can't purchase more power points than magic attribute. Set to max."),
                                       5.0f,
                                       APPSTATUS->getHelperColors().statusBarMessage);
        _purchasedPowerPoints = CHARACTER_VALUES->getAttribute("magic");
    }

    // If we now have fewer points available than spent, reset all spent points
    if (getPowerPoints() < getSpentPowerPoints())
    {
        resetAdeptPowers();
    }
}


//---------------------------------------------------------------------------------
void
CharacterChoices::resetAdeptPowers()
{
    QMap<QString, float>::iterator it;
    for (it = _spellsFromFreebies.begin(); it != _spellsFromFreebies.end();)
    {
        const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(it.key());
        if (def.abilityType == MAGICABILITYTYPE_ADEPT_POWER)
        {
            _spellsFromFreebies.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

//---------------------------------------------------------------------------------
float
CharacterChoices::getSpentPowerPoints() const
{
    float spentPowerPoints = 0.0f;

    // We only look at freebies here, as power points count as freebies
    // They may still cost karma as power points can be bought with karma
    QMap<QString, float>::const_iterator it;
    for (it = _spellsFromFreebies.begin(); it != _spellsFromFreebies.end(); ++it)
    {
        const MagicAbilityDefinition& def = MAGIC_RULES->getDefinition(it.key());
        if (def.abilityType == MAGICABILITYTYPE_ADEPT_POWER)
        {
            spentPowerPoints += it.value();
        }
    }
    return spentPowerPoints;
}

//---------------------------------------------------------------------------------
float
CharacterChoices::getAvailablePowerPoints() const
{
    float maxPowerPoints = getPowerPoints();

    // Stop here when we have no power points
    if (maxPowerPoints <= 0.0f)
    {
        return 0.0f;
    }

    return maxPowerPoints - getSpentPowerPoints();
}

//---------------------------------------------------------------------------------
bool
CharacterChoices::addQuality(const QString& p_id)
{
    const QualityDefinition& def = QUALITY_RULES->getDefinition(p_id);

    // Check if the quality is already present
    int currentLevel = 0;
    int costAlreadyPaid = 0;
    if (_qualities.contains(p_id))
    {
        currentLevel = _qualities[p_id];

        // Quality is not levelled and already present - stop!
        if (def.costType == COSTTYPE_NORMAL)
        {
            APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_ADDED_ALREADY")
                                                .arg(p_id),
                                           5.0f, APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
        else if (def.costType == COSTTYPE_ARRAY)
        {
            costAlreadyPaid = def.costArray[currentLevel - 1];
        }
        else
        {
            costAlreadyPaid = def.costArray[0] * currentLevel;
        }
    }
    int wouldBeLevel = currentLevel + 1;

    // Calculate the actual karma cost
    int karmaCost = 0;
    if (def.costType == COSTTYPE_ARRAY)
    {
        karmaCost = def.costArray[wouldBeLevel - 1];
    }
    // Calculation is the same for per_level and normal as normal is always level 1
    else
    {
        karmaCost = def.costArray[0] * wouldBeLevel;
    }
    karmaCost -= costAlreadyPaid;

    // On postive quality: Do we have enough karma?
    if (def.isPositive && CHARACTER_CHOICES->getAvailableKarma() < karmaCost)
    {
        APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_KARMA")
                                            .arg(p_id)
                                            .arg(wouldBeLevel),
                                       5.0f, APPSTATUS->getHelperColors().statusBarMessage);
        return false;
    }

    // Do we have too many qualities already?
    // Core-rule: Only 25 may be gained from / spent on qualities
    if (!CHARACTER_CHOICES->getHouseRules().qualityLimitSumCounts)
    {
        if (def.isPositive &&
            (getQualitiesKarma(true) + karmaCost) > 25)
        {
            APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_KARMA_LIMIT_POS")
                                                .arg(p_id),
                                           5.0f, APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
        else if (!def.isPositive &&
                 (getQualitiesKarma(false) + karmaCost) > 25)
        {
            APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_KARMA_LIMIT_NEG")
                                                .arg(p_id),
                                           5.0f, APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
    }
    // House rule: Only the sum counts
    else
    {
        int wouldBeBalance = getQualitiesKarma(true) - getQualitiesKarma(false);
        wouldBeBalance += def.isPositive ? karmaCost : -karmaCost;
        if (wouldBeBalance < -25 || wouldBeBalance > 25)
        {
            APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_KARMA_BALANCE")
                                                .arg(p_id),
                                           5.0f, APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
    }

    // Effects
    // First, check if all can be applied, as all have to be valid
    bool allValid = true;
    QString lastError = "";
    for (unsigned int i = 0; i < def.effects.size(); ++i)
    {
        // If the spell is user defined, give the custom choice as target, otherwise, the ID
        QString target = def.isUserDefined ? def.customString : def.id;
        if (!def.effects[i]->canBeApplied(target))
        {
            allValid = false;
            lastError = def.effects[i]->getError();
            APPSTATUS->setStatusBarMessage(Dictionary::getTranslation("ADD_QUALITY_EFFECT")
                                                .arg(p_id)
                                                .arg(lastError),
                                           5.0f,
                                           APPSTATUS->getHelperColors().statusBarMessage);
            return false;
        }
    }
    // Apply and add the effects
    if (allValid)
    {
        for (unsigned int i = 0; i < def.effects.size(); ++i)
        {
            EFFECT_REGISTRY->addActiveEffect(def.effects[i]);
        }
    }

    // Add the quality
    _qualities[p_id] = wouldBeLevel;

    return true;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::removeQuality(const QString& p_id)
{
    // Check if the quality is actually there
    if (!_qualities.contains(p_id))
    {
        return;
    }

    _qualities.remove(p_id);

    // If the removed quality had active effects, remove those, too
    const QualityDefinition& def = QUALITY_RULES->getDefinition(p_id);
    if (def.effects.size() > 0)
    {
        for (unsigned int i = 0; i < def.effects.size(); ++i)
        {
            EFFECT_REGISTRY->removeActiveEffect(def.effects[i]);
        }
    }
}

//---------------------------------------------------------------------------------
bool
CharacterChoices::validateQualities()
{
    bool allValid = true;

    // Remove all qualities, then re-add them to make sure they are all still valid
    qDebug() << "Validating qualities.";
    QMap<QString, int> temp = _qualities;
    resetQualities();
    QMap<QString, int>::iterator it;
    for (it = temp.begin(); it != temp.end(); ++it)
    {
        for (int i = 0; i < it.value(); ++i)
        {
            if (!addQuality(it.key()))
            {
                allValid = false;
            }
        }
    }

    if (allValid)
    {
        qDebug() << "All qualities are still valid.";
    }
    else
    {
        qDebug() << "Some qualities were invalid.";
    }

    return allValid;
}

//---------------------------------------------------------------------------------
void
CharacterChoices::resetQualities()
{
    for (int j = 0; j < _qualities.keys().size(); ++j)
    {
        // If the removed quality had active effects, remove those, too
        const QualityDefinition& def = QUALITY_RULES->getDefinition(_qualities.keys()[j]);
        if (def.effects.size() > 0)
        {
            for (unsigned int i = 0; i < def.effects.size(); ++i)
            {
                EFFECT_REGISTRY->removeActiveEffect(def.effects[i]);
            }
        }
    }

    _qualities.clear();
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getQualitiesKarma(bool p_positive) const
{
    int result = 0;

    // Get all qualities that are either positive or negative
    QMap<QString, int>::const_iterator it;
    for(it = _qualities.begin(); it != _qualities.end(); ++it)
    {
        const QualityDefinition& def = QUALITY_RULES->getDefinition(it.key());
        if (def.isPositive == p_positive)
        {
            // Get the cost, depending on the cost type and level
            if (def.costType == COSTTYPE_ARRAY)
            {
                result += def.costArray[it.value() - 1];
            }
            // Cost per level and normal cost can use the same calculation
            // As the level for normal cost is always 1
            else
            {
                result += def.costArray[0] * it.value();
            }
        }
    }

    return result;
}

