#include "resourceeffect.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "rules/rules.h"

//---------------------------------------------------------------------------------
ResourceEffect::ResourceEffect(QJsonValueRef* p_jsonObject)
    : _resourceEffectType(RESOURCE_EFFECTTYPE_INVALID)
    , _target("")
    , _currentTarget("")
{
    QJsonValueRef val = *p_jsonObject;
    QJsonObject obj = val.toObject();
    // ResourceEffect type
    QString tempString = obj["type"].toString();
    if (tempString == "increase_skill")
    {
        _resourceEffectType = RESOURCE_EFFECTTYPE_INVALID;
    }
    else if (tempString == "none")
    {
        _resourceEffectType = RESOURCE_EFFECTTYPE_NONE;
    }
}

//---------------------------------------------------------------------------------
ResourceEffect::ResourceEffect(const ResourceEffect& p_other)
{
    _resourceEffectType = p_other._resourceEffectType;
    _value = p_other._value;
    _target = p_other._target;
    _currentTarget = _target;
}

//---------------------------------------------------------------------------------
ResourceEffect::~ResourceEffect()
{

}
