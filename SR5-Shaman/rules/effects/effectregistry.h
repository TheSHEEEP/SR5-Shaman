#ifndef EFFECTREGISTRY_H
#define EFFECTREGISTRY_H

#include <vector>

class Effect;

/**
 * @brief This class is responsible for effect creation, deletion and management.
 */
class EffectRegistry
{
public:
    /**
     * @brief Constructor.
     */
    EffectRegistry();

    /**
     * @brief Constructor.
     */
    ~EffectRegistry();

    /**
     * @brief Will parse the JSON object and create an effect from it.
     * @param p_jsonObject  The JSON object to create the effect from.
     * @return The created effect.
     */
    Effect* createEffectFromJson(QJsonObject* p_jsonObject) const;

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

private:
    std::vector<Effect*>    _activeEffects;
};

#endif // EFFECTREGISTRY_H
