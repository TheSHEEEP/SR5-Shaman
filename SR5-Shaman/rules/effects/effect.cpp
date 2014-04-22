#include "effect.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "condition.h"
#include "rules/rules.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
const QString&
EffectSource::getID() const
{
    if (magicAbility) return magicAbility->id;
    if (quality) return quality->id;
}

//---------------------------------------------------------------------------------
bool
EffectSource::operator==(const EffectSource& p_other) const
{
    return (magicAbility == p_other.magicAbility) &&
            (quality == p_other.quality);
}

//---------------------------------------------------------------------------------
bool
EffectSource::operator!=(const EffectSource& p_other) const
{
    return !((*this) == p_other);
}

//---------------------------------------------------------------------------------
Effect::Effect(QJsonValueRef* p_jsonObject, const EffectSource& p_source)
    : _effectType(EFFECTTYPE_INVALID)
    , _source(p_source)
    , _target("")
    , _currentTarget("")
    , _limit("")
{
    QJsonValueRef val = *p_jsonObject;
    QJsonObject obj = val.toObject();
    // Effect type
    QString tempString = obj["type"].toString();
    if (tempString == "increase_skill")
    {
        _effectType = EFFECTTYPE_INCREASE_SKILL;
    }
    else if (tempString == "increase_skill_max")
    {
        // TODO: Mind this when handling skills
        _effectType = EFFECTTYPE_INCREASE_SKILL_MAX;
    }
    else if (tempString == "multiply_skill_category_cost")
    {
        // TODO: Mind this when handling skills
        _effectType = EFFECTTYPE_MULTIPLY_SKILL_CATEGORY_COST;
    }
    else if (tempString == "disable_skill_group")
    {
        // TODO: Mind this when handling skills
        _effectType = EFFECTTYPE_DISABLE_SKILL_GROUP;
    }
    else if (tempString == "allow_skill")
    {
        // TODO: Mind this when enabling skills
        _effectType = EFFECTTYPE_ALLOW_SKILL;
    }
    else if (tempString == "increase_attribute")
    {
        _effectType = EFFECTTYPE_INCREASE_ATTRIBUTE;
    }
    else if (tempString == "increase_attribute_max")
    {
        _effectType = EFFECTTYPE_INCREASE_ATTRIBUTE_MAX;
    }
    else if (tempString == "increase_damage_value")
    {
        // TODO: Mind this when calculating damage values of weapons
        _effectType = EFFECTTYPE_INCREASE_DAMAGE_VALUE;
    }
    else if (tempString == "increase_accuracy")
    {
        // TODO: Mind this when calculating accuracy values of weapons
        _effectType = EFFECTTYPE_INCREASE_ACCURACY;
    }
    else if (tempString == "increase_potential")
    {
        _effectType = EFFECTTYPE_INCREASE_POTENTIAL;
    }
    else if (tempString == "increase_initiative_dice")
    {
        _effectType = EFFECTTYPE_INCREASE_INI_DICE;
    }
    else if (tempString == "increase_armor")
    {
         // TODO: Mind this when calculating armor
        _effectType = EFFECTTYPE_INCREASE_ARMOR;
    }
    else if (tempString == "move_wound_modifier")
    {
         // TODO: Mind this when calculating wound modifiers
        _effectType = EFFECTTYPE_MOVE_WOUND_MODIFIERS;
    }
    else if (tempString == "increase_wound_modifier_section_length")
    {
         // TODO: Mind this when calculating wound modifiers
        _effectType = EFFECTTYPE_INCREASE_WOUND_MODIFIERS_SECTION_LENGTH;
    }
    else if (tempString == "add_free_languages")
    {
         // TODO: Mind this when calculating free languages
        _effectType = EFFECTTYPE_ADD_FREE_LANGUAGES;
    }
    else if (tempString == "increase_memory")
    {
        _effectType = EFFECTTYPE_INCREASE_MEMORY;
    }
    else if (tempString == "increase_notoriety")
    {
        // TODO: Mind this when calculating notoriety
        _effectType = EFFECTTYPE_INCREASE_NOTORIETY;
    }
    else if (tempString == "multiply_cyberware_essence_loss")
    {
        // TODO: Mind this when calculating essence cost of cyberware
        _effectType = EFFECTTYPE_MULTIPLY_CYBERWARE_ESSENCE_LOSS;
    }
    else if (tempString == "disable_bioware")
    {
        // TODO: Mind this when selecting gear
        _effectType = EFFECTTYPE_DISABLE_BIOWARE;
    }
    else if (tempString == "none")
    {
        _effectType = EFFECTTYPE_NONE;
    }

    // Common values
    // Value
    if (obj.contains("value"))
    {
        _value = obj["value"].toString();
    }
    // Limit
    if (obj.contains("limit"))
    {
        _limit = obj["limit"].toString();
    }

    // Type specific values
    // The rather obscure fromUtf16 is used to do a deep copy of the string.
    // If not done, the string will be "garbage collected" or something by Qt itself. Weird.
    // Skills
    if (_effectType == EFFECTTYPE_INCREASE_SKILL ||
        _effectType == EFFECTTYPE_INCREASE_SKILL_MAX ||
        _effectType == EFFECTTYPE_DISABLE_SKILL_GROUP ||
        _effectType == EFFECTTYPE_ALLOW_SKILL ||
        _effectType == EFFECTTYPE_INCREASE_ACCURACY ||
        _effectType == EFFECTTYPE_INCREASE_DAMAGE_VALUE)
    {
        _target = QString::fromUtf16(obj["skill"].toString().utf16());
        _currentTarget = _target;
    }
    // Skill category
    else if (_effectType == EFFECTTYPE_MULTIPLY_SKILL_CATEGORY_COST)
    {
        _target = QString::fromUtf16(obj["category"].toString().utf16());
        _currentTarget = _target;
    }
    // Attributes
    else if (_effectType == EFFECTTYPE_INCREASE_ATTRIBUTE ||
             _effectType == EFFECTTYPE_INCREASE_ATTRIBUTE_MAX)
    {
        _target = QString::fromUtf16(obj["attribute"].toString().utf16());
        _currentTarget = _target;
    }
    // Increase potential
    else if (_effectType == EFFECTTYPE_INCREASE_POTENTIAL)
    {
        _target = QString::fromUtf16(obj["potential"].toString().utf16());
        _currentTarget = _target;
    }
    // Wound modifier movement
    else if (_effectType == EFFECTTYPE_MOVE_WOUND_MODIFIERS ||
             _effectType == EFFECTTYPE_INCREASE_WOUND_MODIFIERS_SECTION_LENGTH)
    {
        _target = QString::fromUtf16(obj["affects"].toString().utf16());
        _currentTarget = _target;
    }

    // Each effect might have multiple conditions
    QJsonArray tempArray;
    if (obj.contains("conditions"))
    {
        tempArray = obj["conditions"].toArray();
        for (int i = 0; i < tempArray.size(); ++i)
        {
            QJsonValueRef valref = tempArray[i];
            _conditions.push_back(new Condition(this, &valref));
        }
    }
}

//---------------------------------------------------------------------------------
Effect::Effect(const Effect& p_other)
{
    _effectType = p_other._effectType;
    _source = p_other._source;
    _value = p_other._value;
    _limit = p_other._limit;
    _target = p_other._target;
    _currentTarget = _target;
    _conditions = p_other._conditions;
}

//---------------------------------------------------------------------------------
Effect::~Effect()
{
    for (unsigned int i = 0; i < _conditions.size(); ++i)
    {
        delete _conditions[i];
    }
}

//---------------------------------------------------------------------------------
bool
Effect::canBeApplied(const QString& p_target)
{
    // If the target is "selected", use the passed target as the current one
    if (_target == "selected")
    {
        _currentTarget = p_target;
    }

    // Only if all conditions are fulfilled, an effect can be applied
    bool result = true;
    for (unsigned int i = 0; i < _conditions.size(); ++i)
    {
        if (!_conditions[i]->isFulfilled())
        {
            _lastError = _conditions[i]->getError();
            result = false;
            break;
        }
    }

    // Check the limit
    if (result && _limit != "")
    {
        result = limitIsMet();
        if (!result)
        {
            _lastError = "Limit is exceeded.";
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
bool
Effect::limitIsMet()
{
    // Get the would-be-value
    float wouldBeValue;
    switch (_effectType)
    {
    case EFFECTTYPE_INCREASE_SKILL:
    case EFFECTTYPE_INCREASE_DAMAGE_VALUE:
    case EFFECTTYPE_INCREASE_ACCURACY:
        wouldBeValue = CHARACTER_VALUES->getSkill(_currentTarget, true, true) + _value.toDouble();
        break;

    case EFFECTTYPE_INCREASE_ATTRIBUTE:
        wouldBeValue = CHARACTER_VALUES->getAttribute(_currentTarget, true, true) + _value.toDouble();
        break;

    case EFFECTTYPE_INCREASE_ATTRIBUTE_MAX:
        wouldBeValue = CHARACTER_VALUES->getAttributeMax(_currentTarget) + _value.toDouble();
        break;

    case EFFECTTYPE_INCREASE_INI_DICE:
        wouldBeValue = CHARACTER_VALUES->getInitiativeDice(true) + _value.toDouble();
        break;

    default:
        wouldBeValue = 100000000.0f;
    }

    // Get the limit value
    // Calculation does not follow algebra sign order, it is only from left to right
    QStringList tokens = _limit.split(" ");
    float limitValue = -1000.0f;
    float previousValue = -1000.0f;
    float currentValue = -1000.0f;
    QString currentSign = "";
    QString token = "";
    for (int i = 0; i < tokens.size(); ++i)
    {
        token = tokens[i];

        // Signs
        if ((token == "+") || (token == "-") || (token == "*") || (token == "/"))
        {
            currentSign = token;
        }
        // Special values
        else if (token == "augmented_maximum")
        {
            currentValue = CHARACTER_VALUES->getAttributeMax(_currentTarget) + 4;
        }
        else if (token == "pure_skill_value")
        {
            currentValue = CHARACTER_VALUES->getSkill(_currentTarget, false, false);
        }
        // Every else must be a number
        else
        {
            currentValue = token.toDouble();
        }

        // Only one token is a special case
        if (tokens.size() == 1)
        {
            limitValue = currentValue;
            break;
        }

        // If we have a previous value, a sign and a currentValue, we can do a calculation step
        if (previousValue > -1000.0f && currentSign != "" && currentValue > -1000.0f)
        {
            // Calculate
            if (currentSign == "+")
            {
                limitValue = previousValue + currentValue;
            }
            else if (currentSign == "-")
            {
                limitValue = previousValue - currentValue;
            }
            else if (currentSign == "*")
            {
                limitValue = previousValue * currentValue;
            }
            else if (currentSign == "/")
            {
                limitValue = previousValue * currentValue;
            }

            // Reset values
            previousValue = limitValue;
            currentSign = "";
        }
        else
        {
            if (previousValue <= -1000.0f)
            {
                previousValue = currentValue;
            }
        }
        currentValue = -1000.0f;
    } // END calculate limit

    return wouldBeValue <= limitValue;
}
