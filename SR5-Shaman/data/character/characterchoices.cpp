#include "characterchoices.h"

#include <QDebug>
#include <QString>
#include <QColor>

#include "charactervalues.h"
#include "data/appstatus.h"

CharacterChoices* CharacterChoices::_instance = 0;

//---------------------------------------------------------------------------------
CharacterChoices::CharacterChoices()
    : _metatypeID("")
    , _magicUserType("")
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

    // If this is a "downgrade" we possibly need to clean up spent points
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

    return result;
}

//---------------------------------------------------------------------------------
int
CharacterChoices::getAvailableKarma() const
{
    return CHARACTER_VALUES->getKarmaPool() - getSpentKarma();
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
                                           2.5f, QColor(0, 0, 255));
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
        }

        // Subtract from freebies
        _attributeIncreasesFreebies[p_attribute] -= attemptedIncrease;
        return;
    } // END decrease

    // Check if an increase would be over the natural maximum
    int naturalMax = METATYPE_RULES->getDefinition(getMetatypeID()).attributesMax[p_attribute];
    if (wouldBeValue > naturalMax)
    {
        APPSTATUS->setStatusBarMessage(tr("Trying to increase %1 above natural maximum. Set to maximum.").arg(p_attribute),
                                       2.5f,
                                       QColor(0, 0, 255));
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
    if (actualFromFreebies < p_fromFreebies)
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
                                       2.5f,
                                       QColor(0, 0, 255));
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
    if (CHARACTER_VALUES->getValidBaseAttributes().find(p_attribute) == CHARACTER_VALUES->getValidBaseAttributes().end())
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
        qWarning() << QString("Could not remove free skill %1. Skill does not exist in freebies.")
                            .arg(p_id);
        return;
    }

    _skillIncreasesFreebies.remove(p_id);
}

