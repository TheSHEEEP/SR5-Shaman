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
    if (tempString == "mod_ac")
    {
        _resourceEffectType = RESOURCE_EFFECTTYPE_MOD_AC;
    }
    else if (tempString == "mod_rc")
    {
        _resourceEffectType = RESOURCE_EFFECTTYPE_MOD_RC;
    }

    // Get value
    if (obj.contains("value"))
    {
        _value = obj["value"].toString().toInt();
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
