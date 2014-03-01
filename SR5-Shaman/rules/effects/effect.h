#ifndef EFFECT_H
#define EFFECT_H

#include <vector>
#include <QString>
#include <QVariant>

class Condition;
class QJsonObject;

/**
 * @brief The different types of effects.
 */
enum EffectType
{
    EFFECTTYPE_INVALID = -1,
    EFFECTTYPE_INCREASE_ATTRIBUTE,
    EFFECTTYPE_INCREASE_SKILL,
    NUM_EFFECTTYPES
};

/**
 * @brief This class holds a single effect and can be used to apply its rule effects, check conditions, etc.
 */
class Effect
{
public:
    /**
     * @brief Constructor.
     * @param p_jsonObject  The JSON node to construct this effect from.
     */
    Effect(QJsonObject* p_jsonObject);

    /**
     * @brief Destructor.
     */
    ~Effect();

    /**
     * @brief Checks if the effect can applied to the target.
     *          Will check if conditions are fulfilled, limits not exceeded, etc.
     * @param p_target  The target of the effect. May be a skill, an attribute, etc.
     * @return
     */
    bool canBeApplied(const QString& p_target) const;

protected:
    EffectType      _effectType;
    QString         _target;
    QVariant        _value;
    QString         _limit;

    std::vector<Condition*>     _conditions;

};

#endif // EFFECT_H
