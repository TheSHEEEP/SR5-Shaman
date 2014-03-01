#ifndef CONDITION_H
#define CONDITION_H

#include "QString.h"

class QJsonObject;
class Effect;

/**
 * @brief The different types of conditions.
 */
enum ConditionType
{
    CONDITIONTYPE_INVALID = -1,
    CONDITIONTYPE_SKILL_KNOWN,
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
     * @param p_jsonObject  The JSON object to construct the condition from.
     */
    Condition(Effect* p_parent, QJsonObject* p_jsonObject);

    /**
     * @brief Destructor.
     */
    ~Condition();

    /**
     * @brief Returns true if the condition is fulfilled.
     */
    bool isFullfilled() const;

private:
    ConditionType   _conditionType;
    Effect*         _parent;
};

#endif // CONDITION_H
