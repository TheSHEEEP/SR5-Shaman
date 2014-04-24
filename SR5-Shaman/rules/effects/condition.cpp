#include "condition.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDebug>
#include "effect.h"
#include "rules/rules.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"
#include "data/character/effectregistry.h"
#include "data/dictionary.h"

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
        else if (tempString == "must_be_decker")
        {
            _conditionType = CONDITIONTYPE_MUST_BE_DECKER;
        }
    }
    else if (val.type() == QJsonValue::Object)
    {
        QJsonObject tempObject = val.toObject();
        QJsonArray tempArray;
        if (tempObject.contains("min_skill_rating"))
        {
            _conditionType = CONDITIONTYPE_MIN_SKILL_RATING;
            _values.push_back(tempObject["min_skill_rating"].toString());
        }
        else if (tempObject.contains("must_have_quality_at_level"))
        {
            _conditionType = CONDITIONTYPE_MUST_HAVE_QUALITY_AT_LEVEL;
            tempArray = tempObject["must_have_quality_at_level"].toArray();
            for (int i = 0; i < tempArray.size(); ++i)
            {
                _values.push_back(tempArray[i].toString());
            }
        }
        else if (tempObject.contains("must_not_have_qualities"))
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
        else if (tempObject.contains("must_have_gear_of_type"))
        {
            _conditionType = CONDITIONTYPE_MUST_HAVE_GEAR_OF_TYPE;
            _values.push_back(tempObject["must_have_gear_of_type"].toString());
        }
        else if (tempObject.contains("one_of"))
        {
            _conditionType = CONDITIONTYPE_ONE_OF;
            tempArray = tempObject["one_of"].toArray();
            for (int i = 0; i < tempArray.size(); ++i)
            {
                QJsonValueRef valref = tempArray[i];
                _subConditions.push_back(new Condition(_parent, &valref));
            }
        }
    }
}

//---------------------------------------------------------------------------------
Condition::Condition(const Condition* p_other, Effect* p_parent)
{
    _conditionType = p_other->_conditionType;
    _values = p_other->_values;
    _parent = p_parent;
    _lastError = p_other->_lastError;
    for (unsigned int i = 0; i < p_other->_subConditions.size(); ++i)
    {
        _subConditions.push_back(new Condition(p_other->_subConditions[i], p_parent));
    }
}

//---------------------------------------------------------------------------------
Condition::~Condition()
{
    for (unsigned int i = 0; i < _subConditions.size(); ++i)
    {
        delete _subConditions[i];
    }
}

//---------------------------------------------------------------------------------
bool
Condition::isFulfilled()
{
    _lastError = "";

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
            _lastError = Dictionary::getTranslation("COND_SKILL_NOT_KNOWN").arg(targetSkill);
        }
    }
    // The skill must have a minimum rating
    else if (_conditionType == CONDITIONTYPE_MIN_SKILL_RATING)
    {
        // Get the rating and subtract the effect value
        QString targetSkill = _parent->getCurrentTarget();
        int rating = CHARACTER_VALUES->getSkill(targetSkill);
        rating -= _parent->getValue().toInt();

        // Check if it is same/above minimum rating
        if (rating >= _values[0].toInt())
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_SKILL_BELOW_MIN_RATING")
                                                    .arg(targetSkill)
                                                    .arg(_values[0].toInt());
        }
    }
    // Can only be taken once
    else if (_conditionType == CONDITIONTYPE_ONLY_ONCE)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(_parent->getType());

        // Now look through all effects to check if one source is related
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            if (effects[i]->getSource().isRelatedWith(_parent->getSource()))
            {
                // Exception: If it has the EXACT same ID
                // AND is levelled, then it is a valid replacement
                qDebug() << effects[i]->getSource().getID();
                qDebug() << _parent->getSource().getID();
                qDebug() << effects[i]->getSource().getCostType();
                qDebug() << COSTTYPE_ARRAY << " " << COSTTYPE_PER_LEVEL;
                if (effects[i]->getSource().getID() == _parent->getSource().getID() &&
                    (effects[i]->getSource().getCostType() == COSTTYPE_ARRAY ||
                     effects[i]->getSource().getCostType() == COSTTYPE_PER_LEVEL))
                {
                    continue;
                }
                _lastError = Dictionary::getTranslation("COND_ONLY_ONCE")
                                                        .arg(_parent->getSource().getID());
                return false;
            }
        }
        return true;
    }
    // Another quality must be present at a minimum level
    else if (_conditionType == CONDITIONTYPE_MUST_HAVE_QUALITY_AT_LEVEL)
    {
        const QMap<QString, int>& qualities = CHARACTER_CHOICES->getQualities();
        QMap<QString, int>::const_iterator it;
        for (it = qualities.begin(); it != qualities.end(); ++it)
        {
            if (it.key().contains(_values[0]) &&
                it.value() >= _values[1].toInt())
            {
                return true;
            }
        }
        _lastError = Dictionary::getTranslation("COND_QUAL_LEVEL_IS_NOT_PRESENT")
                                                .arg(_values[0])
                                                .arg(_values[1]);
    }
    // Cannot be taken if another quality is present
    else if (_conditionType == CONDITIONTYPE_MUST_NOT_HAVE_QUALITIES)
    {
        const QMap<QString, int>& qualities = CHARACTER_CHOICES->getQualities();
        QMap<QString, int>::const_iterator it;
        for (it = qualities.begin(); it != qualities.end(); ++it)
        {
            for (int i = 0; i < _values.size(); ++i)
            {
                if (it.key().contains(_values[i]))
                {
                    _lastError = Dictionary::getTranslation("COND_QUAL_IS_PRESENT")
                                                            .arg(_parent->getSource().getID())
                                                            .arg(it.key());
                    return false;
                }
            }
        }
        return true;
    }
    // Must have a positive attribute value
    else if (_conditionType == CONDITIONTYPE_MUST_HAVE_ATTRIBUTE_VALUE)
    {
        if (CHARACTER_VALUES->getAttribute(_values[0]) > 0)
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_ATTR_MUST_BE_POSITIVE")
                                                    .arg(_values[0]);
        }
    }
    // Must be of one of certain magic types
    else if (_conditionType == CONDITIONTYPE_MUST_BE_MAGIC_TYPE)
    {
        if (!CHARACTER_CHOICES->getIsMagicUser())
        {
            _lastError = Dictionary::getTranslation("COND_MUST_BE_MAGE");
            return false;
        }

        QString typeList = "";
        for (int i = 0; i < _values.size(); ++i)
        {
            typeList.append(", ");
            if (MAGIC_RULES->getMagicTypeDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                    .types.contains(_values[i]))
            {
                return true;
            }
            typeList.append(_values[i]);
        }
        _lastError = Dictionary::getTranslation("COND_MUST_BE_MAGIC_TYPE")
                                                .arg(typeList);
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

        if ((currentLevel + 1) <= _values[0].toInt())
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_MAXIMUM_LEVEL")
                                                    .arg(_parent->getSource().getID())
                                                    .arg(_values[0].toInt());
        }
    }
    // Has a maximum level (including custom versions of the effect)
    else if (_conditionType == CONDITIONTYPE_MAX_LEVEL_WITH_CUSTOMS)
    {
        int currentLevel = 0;

        // Get the level from the actual source
        if (_parent->getSource().magicAbility)
        {
            // Get all custom versions
            QStringList customVersions = MAGIC_RULES->getCustomVersions(_parent->getSource().magicAbility->id);

            for (int i = 0; i < customVersions.size(); ++i)
            {
                // Only adept powers can have levels
                currentLevel += CHARACTER_VALUES->getAdeptPowerLevel(customVersions[i]);
            }
        }
        else if (_parent->getSource().quality)
        {
            // Get all custom versions
            QStringList customVersions = QUALITY_RULES->getCustomVersions(_parent->getSource().quality->id);

            for (int i = 0; i < customVersions.size(); ++i)
            {
                currentLevel = CHARACTER_VALUES->getQualityLevel(customVersions[i]);
            }
        }

        if ((currentLevel + 1) <= _values[0].toInt())
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_MAXIMUM_LEVEL")
                                                    .arg(_parent->getSource().getID())
                                                    .arg(_values[0].toInt());
        }
    }
    // There can be only one source with this effect
    else if (_conditionType == CONDITIONTYPE_EXCLUSIVE_EFFECT)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(_parent->getType());
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            if (effects[i]->getSource() != _parent->getSource())
            {
                _lastError = Dictionary::getTranslation("COND_EXCLUSIVE_EFFECT");
                return false;
            }
        }

        return true;
    }
    // Must be of one of certain metatypes
    else if (_conditionType == CONDITIONTYPE_MUST_BE_METATYPE)
    {
        QString typeList = "";
        for (int i = 0; i < _values.size(); ++i)
        {
            typeList.append(", ");
            if (CHARACTER_CHOICES->getMetatypeID().contains(_values[i]))
            {
                return true;
            }
            typeList.append(_values[i]);
        }
        _lastError = Dictionary::getTranslation("COND_MUST_BE_METATYPE")
                                                .arg(typeList);
    }
    // Must be of one of certain metatypes
    else if (_conditionType == CONDITIONTYPE_MUST_BE_MUNDANE)
    {
        if (!CHARACTER_CHOICES->getIsMagicUser())
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_MUST_BE_MUNDANE");
        }
    }
    // Must be a decker
    else if (_conditionType == CONDITIONTYPE_MUST_BE_DECKER)
    {
        // TODO: There is no real rule definition for "decker", this is very arbitrary
        int skillValue = CHARACTER_VALUES->getSkill("CYBERCOMBAT");
        skillValue += CHARACTER_VALUES->getSkill("ELECTRONIC_WARFARE");
        skillValue += CHARACTER_VALUES->getSkill("HACKING");
        skillValue += CHARACTER_VALUES->getSkill("COMPUTER");
        skillValue += CHARACTER_VALUES->getSkill("HARDWARE");
        skillValue += CHARACTER_VALUES->getSkill("SOFTWARE");
        if (skillValue >= 12)
        {
            return true;
        }
        else
        {
            _lastError = Dictionary::getTranslation("COND_MUST_BE_DECKER");
        }
    }
    // Must have a gear of a certain type
    else if (_conditionType == CONDITIONTYPE_MUST_HAVE_GEAR_OF_TYPE)
    {
        // TODO: Implement gear
        return true;
    }
    // Must have a gear of a certain type
    else if (_conditionType == CONDITIONTYPE_ONE_OF)
    {
        _lastError = Dictionary::getTranslation("COND_ONE_OF");
        for (unsigned int i = 0; i < _subConditions.size(); ++i)
        {
            if (_subConditions[i]->isFulfilled())
            {
                return true;
            }
            else
            {
                _lastError.append(_subConditions[i]->getError());
                _lastError.append(Dictionary::getTranslation("COND_ONE_OF_OR"));
            }
        }
    }

    return false;
}
