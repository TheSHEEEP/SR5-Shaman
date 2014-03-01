#include "effectregistry.h"
#include "effect.h"

//---------------------------------------------------------------------------------
EffectRegistry::EffectRegistry()
{
}

//---------------------------------------------------------------------------------
EffectRegistry::~EffectRegistry()
{
}

//---------------------------------------------------------------------------------
Effect*
EffectRegistry::createEffectFromJson(QJsonObject* p_jsonObject) const
{
    Effect* result = new Effect(p_jsonObject);
    return result;
}

//---------------------------------------------------------------------------------
void
EffectRegistry::addActiveEffect(Effect* p_effect)
{

}

//---------------------------------------------------------------------------------
void
EffectRegistry::removeActiveEffect(Effect* p_effect)
{

}
