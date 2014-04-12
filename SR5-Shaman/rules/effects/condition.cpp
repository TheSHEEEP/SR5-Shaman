#include "condition.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "effect.h"
#include "rules/rules.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"
#include "data/character/effectregistry.h"

//---------------------------------------------------------------------------------
Condition::Condition(Effect* p_parent, QJsonValueRef* p_jsonObject)
    : _conditionType(CONDITIONTYPE_INVALID)
    , _parent(p_parent)
{
    // A condition might be anything from a simple string to a more complex object
    QJsonValueRef val = *p_jsonObject;
    if (val.type() == QJsonValue::String)
    {
        QString tempString = val.toString();
        if (tempString == "skill_known")
        {
            _conditionType = CONDITIONTYPE_SKILL_KNOWN;
        }
        else if (tempString == "only_initiative_increase")
        {
            _conditionType = CONDITIONTYPE_ONLY_INI_INCREASE;
        }
        else if (tempString == "only_once")
        {
            _conditionType = CONDITIONTYPE_ONLY_ONCE;
        }
        else if (tempString == "exclusive_effect")
        {
            _conditionType = CONDITIONTYPE_EXCLUSIVE_EFFECT;
        }
        else if (tempString == "must_be_mundane")
        {
            _conditionType = CONDITIONTYPE_MUST_BE_MUNDANE;
        }
    }
    else if (val.type() == QJsonValue::Object)
    {
        QJsonObject tempObject = val.toObject();
        QJsonArray tempArray;
        if (tempObject.contains("must_not_have_qualities"))
        {
            _conditionType = CONDITIONTYPE_MUST_NOT_HAVE_QUALITIES;
            tempArray = tempObject["must_not_have_qualities"].toArray();
            for (int i = 0; i < tempArray.size(); ++i)
            {
                _values.push_back(tempArray[i].toString());
            }
        }
        else if (tempObject.contains("must_have_attribute_value"))
        {
            _conditionType = CONDITIONTYPE_MUST_HAVE_ATTRIBUTE_VALUE;
            _values.push_back(tempObject["must_have_attribute_value"].toString());
        }
        else if (tempObject.contains("must_be_magic_type"))
        {
            _conditionType = CONDITIONTYPE_MUST_BE_MAGIC_TYPE;
            tempArray = tempObject["must_be_magic_type"].toArray();
            for (int i = 0; i < tempArray.size(); ++i)
            {
                _values.push_back(tempArray[i].toString());
            }
        }
        else if (tempObject.contains("max_level"))
        {
            _conditionType = CONDITIONTYPE_MAX_LEVEL;
            _values.push_back(tempObject["max_level"].toString());
        }
        else if (tempObject.contains("max_level_with_customs"))
        {
            _conditionType = CONDITIONTYPE_MAX_LEVEL_WITH_CUSTOMS;
            _values.push_back(tempObject["max_level_with_customs"].toString());
        }
        else if (tempObject.contains("must_be_metatype"))
        {
            _conditionType = CONDITIONTYPE_MUST_BE_METATYPE;
            tempArray = tempObject["must_be_metatype"].toArray();
            for (int i = 0; i < tempArray.size(); ++i)
            {
                _values.push_back(tempArray[i].toString());
            }
        }
    }
}

//---------------------------------------------------------------------------------
Condition::~Condition()
{
}

//---------------------------------------------------------------------------------
bool
Condition::isFulfilled()
{
    // Skill known
    if (_conditionType == CONDITIONTYPE_SKILL_KNOWN)
    {
        // Get the target skill and check if it is known to the character
        QString targetSkill = _parent->getCurrentTarget();
        if (CHARACTER_VALUES->getSkill(targetSkill) > 0)
        {
            return true;
        }
        else
        {
            _lastError = QObject::tr("Skill %1 not known to the character.").arg(targetSkill);
        }
    }
    // Can only be taken once
    else if (_conditionType == CONDITIONTYPE_ONLY_ONCE)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(_parent->getType());

        // Now look through all effects to check if one source has the same ID
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            if (effects[i]->getSource.getID().contains(_parent->getSource().getID()))
            {
                return false;
            }
        }
        return true;
    }
    // Cannot be taken if another quality is present
    else if (_conditionType == CONDITIONTYPE_MUST_NOT_HAVE_QUALITIES)
    {
        const QMap<QString, int>& qualities = CHARACTER_CHOICES->getQualities();
        const QMap<QString, int>::iterator it;
        for (it = qualities.begin(); it != qualities.end(); ++it)
        {
            for (int i = 0; i < _values.size(); ++i)
            {
                if (it.key().contains(_values[i]))
                {
                    return false;
                }
            }
        }
        return true;
    }
    // Must have a positive attribute value
    else if (_conditionType == CONDITIONTYPE_MUST_HAVE_ATTRIBUTE_VALUE)
    {
        return CHARACTER_VALUES->getAttribute(_values[0]) > 0;
    }
    // Must be of one of certain magic types
    else if (_conditionType == CONDITIONTYPE_MUST_BE_MAGIC_TYPE)
    {
        if (!CHARACTER_CHOICES->getIsMagicUser())
        {
            return false;
        }

        for (int i = 0; i < _values.size(); ++i)
        {
            if (MAGIC_RULES->getMagicTypeDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                    .types.contains(_values[i]))
            {
                return true;
            }
        }
    }
    // Has a maximum level
    else if (_conditionType == CONDITIONTYPE_MAX_LEVEL)
    {
        int currentLevel = 0;

        // Get the level from the actual source
        if (_parent->getSource().magicAbility)
        {
            // Only adept powers can have levels
            currentLevel = CHARACTER_VALUES->getAdeptPowerLevel(_parent->getSource().magicAbility->id);
        }
        else if (_parent->getSource().quality)
        {
            currentLevel = CHARACTER_VALUES->getQualityLevel(_parent->getSource().quality->id);
        }

        return (currentLevel + 1) <= _values[0].toInt();
    }
    // Has a maximum level (including custom versions of the effect)
    else if (_conditionType == CONDITIONTYPE_MAX_LEVEL_WITH_CUSTOMS)
    {
        int currentLevel = 0;

        // Get the level from the actual source
        if (_parent->getSource().magicAbility)
        {
            // Get all custom versions
            QStringList customVersions = MAGIC_RULES->getCustomVersions(_parent->getSource().magicAbility);

            for (int i = 0; i < customVersions.size(); ++i)
            {
                // Only adept powers can have levels
                currentLevel += CHARACTER_VALUES->getAdeptPowerLevel(customVersions[i]);
            }
        }
        else if (_parent->getSource().quality)
        {
            // Get all custom versions
            QStringList customVersions = QUALITY_RULES->getCustomVersions(_parent->getSource().quality);

            for (int i = 0; i < customVersions.size(); ++i)
            {
                currentLevel = CHARACTER_VALUES->getQualityLevel(customVersions[i]);
            }
        }

        return (currentLevel + 1) <= _values[0].toInt();
    }
    // There can be only one source with this effect
    else if (_conditionType == CONDITIONTYPE_EXCLUSIVE_EFFECT)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(_parent->getType());
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            if (effects[i]->getSource() != _parent->getSource())
            {
                return false;
            }
        }

        return true;
    }
    // Must be of one of certain metatypes
    else if (_conditionType == CONDITIONTYPE_MUST_BE_METATYPE)
    {
        for (int i = 0; i < _values.size(); ++i)
        {
            if (CHARACTER_CHOICES->getMetatypeID().contains(_values[i]))
            {
                return true;
            }
        }
    }
    // Must be of one of certain metatypes
    else if (_conditionType == CONDITIONTYPE_MUST_BE_MUNDANE)
    {
        return !CHARACTER_CHOICES->getIsMagicUser();
    }

    return false;
}
