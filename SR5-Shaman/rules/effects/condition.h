#ifndef CONDITION_H
#define CONDITION_H

#include "QString.h"

class QJsonValueRef;
class Effect;

/**
 * @brief The different types of conditions.
 */
enum ConditionType
{
    CONDITIONTYPE_INVALID = -1,
    CONDITIONTYPE_SKILL_KNOWN,
    CONDITIONTYPE_ONLY_INI_INCREASE,
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
     * @brief Destructor.
     */
    ~Condition();

    /**
     * @brief Returns true if the condition is fulfilled.
     */
    bool isFulfilled();

    /**
     * @brief Returns the last fulfillment error.
     */
    const QString& getError() const;

private:
    ConditionType   _conditionType;
    Effect*         _parent;
    QString         _lastError;
};


//---------------------------------------------------------------------------------
inline
const QString&
Condition::getError() const
{
    return _lastError;
}
#endif // CONDITION_H
