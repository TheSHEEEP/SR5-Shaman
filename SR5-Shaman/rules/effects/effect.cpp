#include "effect.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "condition.h"
#include "rules/rules.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
Effect::Effect(QJsonValueRef* p_jsonObject, const EffectSource& p_source)
    : _effectType(EFFECTTYPE_INVALID)
    , _source(p_source)
    , _target("")
    , _currentTarget("")
    , _limit("")
{
    QJsonValueRef val = *p_jsonObject;
    QJsonObject obj = val.toObject();
    // Effect type
    QString tempString = obj["type"].toString();
    if (tempString == "increase_skill")
    {
        _effectType = EFFECTTYPE_INCREASE_SKILL;
    }
    else if (tempString == "increase_attribute")
    {
        _effectType = EFFECTTYPE_INCREASE_ATTRIBUTE;
    }

    // Common values
    // Value
    if (obj.contains("value"))
    {
        _value = obj["value"].toString();
    }
    // Limit
    if (obj.contains("limit"))
    {
        _limit = obj["limit"].toString();
    }

    // Type specific values
    // Increase skill
    if (_effectType == EFFECTTYPE_INCREASE_SKILL)
    {
        _target = QString::fromUtf16(obj["skill"].toString().utf16());
        _currentTarget = _target;
    }
    // Increase attribute
    else if (_effectType == EFFECTTYPE_INCREASE_ATTRIBUTE)
    {
        _target = QString::fromUtf16(obj["attribute"].toString().utf16());
        _currentTarget = _target;
    }

    // Each effect might have multiple conditions
    QJsonArray tempArray;
    if (obj.contains("conditions"))
    {
        tempArray = obj["conditions"].toArray();
        for (int i = 0; i < tempArray.size(); ++i)
        {
            QJsonValueRef valref = tempArray[i];
            _conditions.push_back(new Condition(this, &valref));
        }
    }
}

//---------------------------------------------------------------------------------
Effect::Effect(const Effect& p_other)
{
    _effectType = p_other._effectType;
    _source = p_other._source;
    _value = p_other._value;
    _limit = p_other._limit;
    _target = p_other._target;
    _currentTarget = _target;
    _conditions = p_other._conditions;
}

//---------------------------------------------------------------------------------
Effect::~Effect()
{
    for (unsigned int i = 0; i < _conditions.size(); ++i)
    {
        delete _conditions[i];
    }
}

//---------------------------------------------------------------------------------
bool
Effect::canBeApplied(const QString& p_target)
{
    // If the target is "selected", use the passed target as the current one
    if (_target == "selected")
    {
        _currentTarget = p_target;
    }

    // Only if all conditions are fulfilled, an effect can be applied
    bool result = true;
    for (unsigned int i = 0; i < _conditions.size(); ++i)
    {
        if (!_conditions[i]->isFulfilled())
        {
            _lastError = _conditions[i]->getError();
            result = false;
            break;
        }
    }

    return result;
}
