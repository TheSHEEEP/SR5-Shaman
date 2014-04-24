#ifndef CONDITION_H
#define CONDITION_H

#include <QString>
#include <QStringList>

class QJsonValueRef;
class Effect;

/**
 * @brief The different types of conditions.
 */
enum ConditionType
{
    CONDITIONTYPE_INVALID = -1,
    CONDITIONTYPE_SKILL_KNOWN,
    CONDITIONTYPE_MIN_SKILL_RATING,
    CONDITIONTYPE_ONLY_ONCE,
    CONDITIONTYPE_MUST_HAVE_QUALITY_AT_LEVEL,
    CONDITIONTYPE_MUST_NOT_HAVE_QUALITIES,
    CONDITIONTYPE_MUST_HAVE_ATTRIBUTE_VALUE,
    CONDITIONTYPE_MUST_BE_MAGIC_TYPE,
    CONDITIONTYPE_MAX_LEVEL,
    CONDITIONTYPE_MAX_LEVEL_WITH_CUSTOMS,
    CONDITIONTYPE_EXCLUSIVE_EFFECT,
    CONDITIONTYPE_MUST_BE_METATYPE,
    CONDITIONTYPE_MUST_BE_MUNDANE,
    CONDITIONTYPE_MUST_BE_DECKER,
    CONDITIONTYPE_MUST_HAVE_GEAR_OF_TYPE,
    CONDITIONTYPE_ONE_OF,
    NUM_CONDITIONTYPES
};


/**
 * @brief This class represents a single condition to be used in an effect.
 */
class Condition
{
public:
    /**
     * @brief Constructor.
     * @param p_parent      The parent effect. May be used in validation.
     * @param p_jsonObject  The JSON value to construct the condition from.
     */
    Condition(Effect* p_parent, QJsonValueRef* p_jsonObject);

    /**
     * @brief Copy constructor (kind of).
     * @param p_other   The condition to copy from.
     * @param p_parent  The new parent.
     */
    Condition(const Condition* p_other, Effect* p_parent);

    /**
     * @brief Destructor.
     */
    ~Condition();

    /**
     * @brief Returns true if the condition is fulfilled.
     */
    bool isFulfilled();

    /**
     * @brief Returns the type of the condition.
     */
    ConditionType getType() const;

    /**
     * @brief Returns the type of the condition.
     */
    const QStringList& getValues() const;

    /**
     * @brief Returns the last fulfillment error.
     */
    const QString& getError() const;

private:
    ConditionType           _conditionType;
    QStringList             _values;
    std::vector<Condition*> _subConditions;
    Effect*                 _parent;
    QString                 _lastError;
};


//---------------------------------------------------------------------------------
inline
const QString&
Condition::getError() const
{
    return _lastError;
}

//---------------------------------------------------------------------------------
inline
ConditionType
Condition::getType() const
{
    return _conditionType;
}

//---------------------------------------------------------------------------------
inline
const QStringList&
Condition::getValues() const
{
    return _values;
}

#endif // CONDITION_H
