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

    // Set the priotiy
    _selectedPriorities[p_priorityIndex] = p_prio;

    // Make sure we only have this priority once
    for (int i = 0; i < 5; ++i)
    {
        if (i != p_priorityIndex &&
            _selectedPriorities[i] == p_prio)
        {
            _selectedPriorities[i] = PRIORITY_INVALID;
        }
    }

    // Make sure non-magic stays at lowest priority
    if (!magicUser && p_priorityIndex != 4)
    {
        setIsMagicUser(false);
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
