#ifndef EFFECTREGISTRY_H
#define EFFECTREGISTRY_H

#include <vector>
#include "rules/effects/effect.h"

// Helpful defines
#define EFFECT_REGISTRY EffectRegistry::getSingletonPtr()

/**
 * @brief This class is responsible for effect creation, deletion and management.
 */
class EffectRegistry
{

private:
    static EffectRegistry* _instance;

    /**
     * @brief Constructor.
     */
    EffectRegistry();

public:
    /**
     * @brief Returns a pointer to the EffectRegistry.
     */
    static EffectRegistry* getSingletonPtr()
    {
        if (_instance == 0)
        {
            _instance = new EffectRegistry();
        }
        return _instance;
    }

    /**
     * @brief Constructor.
     */
    ~EffectRegistry();

    /**
     * @brief Adds the passed effect to the active effects. The same effect can be added multiple times.
     * @param p_effect   The effect to add.
     */
    void addActiveEffect(Effect* p_effect);

    /**
     * @brief Removes the passed effect from the active effects.
     * @param p_effect   The effect to add.
     */
    void removeActiveEffect(Effect* p_effect);

    /**
     * @brief Returns a vector with all active effects of the passed type.
     * @param p_type    The effect type to look for.
     */
    std::vector<Effect*> getEffectsByType(EffectType p_type) const;

private:
    std::vector<Effect*>    _activeEffects;
};

#endif // EFFECTREGISTRY_H
