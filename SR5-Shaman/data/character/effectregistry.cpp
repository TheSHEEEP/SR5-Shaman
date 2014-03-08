#include "effectregistry.h"

EffectRegistry* EffectRegistry::_instance = 0;

//---------------------------------------------------------------------------------
EffectRegistry::EffectRegistry()
{
}

//---------------------------------------------------------------------------------
EffectRegistry::~EffectRegistry()
{
}

//---------------------------------------------------------------------------------
void
EffectRegistry::addActiveEffect(Effect* p_effect)
{
    _activeEffects.push_back(p_effect);
}

//---------------------------------------------------------------------------------
void
EffectRegistry::removeActiveEffect(Effect* p_effect)
{
    for (unsigned int i = 0; i < _activeEffects.size(); ++i)
    {
        if (_activeEffects[i] == p_effect)
        {
            _activeEffects.erase(_activeEffects.begin() + i);
            break;
        }
    }
}

//---------------------------------------------------------------------------------
std::vector<Effect*>
EffectRegistry::getEffectsByType(EffectType p_type) const
{
    std::vector<Effect*> effects;
    for (unsigned int i = 0; i < _activeEffects.size(); ++i)
    {
        if (_activeEffects[i]->getType() == p_type)
        {
            effects.push_back(_activeEffects[i]);
        }
    }

    return effects;
}
