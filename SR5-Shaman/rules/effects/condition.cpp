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
    // Must be the only effect that increases initiative somehow
    else if (_conditionType == CONDITIONTYPE_ONLY_INI_INCREASE)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(EFFECTTYPE_INCREASE_INI_DICE);
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            if (effects[i] != _parent)
            {
                return false;
            }
        }

        return true;
    }
    // Can only be taken once
    else if (_conditionType == CONDITIONTYPE_ONLY_ONCE)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsBySource(_parent->getSource());
        return effects.empty();
    }
    // Cannot be taken if another quality is present
    else if (_conditionType == CONDITIONTYPE_MUST_NOT_HAVE_QUALITIES)
    {
        for (int i = 0; i < _values.size(); ++i)
        {
            if (CHARACTER_CHOICES->getQualities().contains(_values[i]))
            {
                return false;
            }
        }
        return true;
    }
    // Must have a positive attribute value
    else if (_conditionType == CONDITIONTYPE_MUST_HAVE_ATTRIBUTE_VALUE)
    {
        return CHARACTER_VALUES->getAttribute(_values[0]) > 0;
    }

    return false;
}
