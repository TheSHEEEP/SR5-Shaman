#ifndef EFFECT_H
#define EFFECT_H

#include <vector>
#include <QString>
#include <QVariant>
#include "rules/commonrules.h"

class Condition;
class QJsonValueRef;
class MagicAbilityDefinition;
class QualityDefinition;

/**
 * @brief The source of an effect.
 */
struct EffectSource
{
    /**
     * @brief Constructor.
     */
    EffectSource()
        : magicAbility(NULL)
        , quality(NULL)
    {}

    /**
     * @brief Returns the ID of the source.
     */
    const QString& getID() const;

    /**
     * @brief Returns the ID of the base of the source (useful for customized).
     *          Returns "" if the source has no base.
     */
    const QString& getBaseID() const;

    /**
     * @brief Returns the cost type of the source.
     *          Returns COSTTYPE_INVALID if this is not applicable.
     */
    CostType getCostType() const;

    /**
     * @brief Returns true if this source is related with the passed source.
     *          This is the case if both have the same ID,
     *          or if one is based upon the other.
     */
    bool isRelatedWith(const EffectSource& p_other) const;

    /**
     * Comparison operators.
     */
    bool operator==(const EffectSource& p_other) const;
    bool operator!=(const EffectSource& p_other) const;

    MagicAbilityDefinition*     magicAbility;
    QualityDefinition*          quality;

private:
    static QString  _nothing;
};

/**
 * @brief The different types of effects.
 */
enum EffectType
{
    EFFECTTYPE_INVALID = -1,
    EFFECTTYPE_INCREASE_ATTRIBUTE,
    EFFECTTYPE_INCREASE_ATTRIBUTE_MAX,
    EFFECTTYPE_INCREASE_SKILL,
    EFFECTTYPE_INCREASE_SKILL_MAX,
    EFFECTTYPE_MULTIPLY_SKILL_CATEGORY_COST,
    EFFECTTYPE_DISABLE_SKILL_GROUP,
    EFFECTTYPE_ALLOW_SKILL,
    EFFECTTYPE_INCREASE_DAMAGE_VALUE,
    EFFECTTYPE_INCREASE_ACCURACY,
    EFFECTTYPE_INCREASE_POTENTIAL,
    EFFECTTYPE_INCREASE_INI_DICE,
    EFFECTTYPE_INCREASE_ARMOR,
    EFFECTTYPE_INCREASE_NOTORIETY,
    EFFECTTYPE_MOVE_WOUND_MODIFIERS,
    EFFECTTYPE_INCREASE_WOUND_MODIFIERS_SECTION_LENGTH,
    EFFECTTYPE_ADD_FREE_LANGUAGES,
    EFFECTTYPE_INCREASE_MEMORY,
    EFFECTTYPE_MULTIPLY_CYBERWARE_ESSENCE_LOSS,
    EFFECTTYPE_DISABLE_BIOWARE,
    EFFECTTYPE_NONE,
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
    Effect(QJsonValueRef* p_jsonObject, const EffectSource& p_source);

    /**
     * @brief Copy constructor.
     */
    Effect(const Effect& p_other);

    /**
     * @brief Destructor.
     */
    ~Effect();

    /**
     * @brief Checks if the effect can applied to the target.
     *          Will check if conditions are fulfilled, limits not exceeded, etc.
     * @note    This is the only function that sets the current target if the normal target is "selected"
     *          or similar. It must be called before the effect is applied.
     * @param p_target  The target of the effect. May be a skill, an attribute, etc.
     * @return  True if the effect can be applied.
     */
    bool canBeApplied(const QString& p_target);

    /**
     * @brief Returns the last error.
     */
    const QString& getError() const;

    /**
     * @param p_source  The new source of this effect.
     */
    void setSource(const EffectSource& p_source);

    /**
     * @brief Returns the source of this effect.
     */
    const EffectSource& getSource() const;

    /**
     * @brief Returns the type of this effect.
     */
    const EffectType getType() const;

    /**
     * @brief Returns the ID of the current target. May be a skill, an attribute, etc.
     */
    const QString& getCurrentTarget() const;

    /**
     * @brief returns the value of this effect.
     */
    const QVariant& getValue() const;

    /**
     * @brief Returns the vector of conditions.
     */
    const std::vector<Condition*>& getConditions() const;

protected:
    EffectType      _effectType;
    EffectSource    _source;
    QString         _target;
    QString         _currentTarget;
    QVariant        _value;
    QString         _limit;
    QString         _lastError;

    std::vector<Condition*>     _conditions;

    /**
     * @brief Returns true if the limit is still met if the effect was applied.
     *          canBeApplied calls this automatically if required.
     */
    bool limitIsMet();
};

//---------------------------------------------------------------------------------
inline
const QString&
Effect::getError() const
{
    return _lastError;
}

//---------------------------------------------------------------------------------
inline
void
Effect::setSource(const EffectSource& p_source)
{
    _source = p_source;
}

//---------------------------------------------------------------------------------
inline
const EffectSource&
Effect::getSource() const
{
    return _source;
}

//---------------------------------------------------------------------------------
inline
const EffectType
Effect::getType() const
{
    return _effectType;
}

//---------------------------------------------------------------------------------
inline
const QString&
Effect::getCurrentTarget() const
{
    return _currentTarget;
}

//---------------------------------------------------------------------------------
inline
const QVariant&
Effect::getValue() const
{
    return _value;
}

//---------------------------------------------------------------------------------
inline
const std::vector<Condition*>&
Effect::getConditions() const
{
    return _conditions;
}

#endif // EFFECT_H
