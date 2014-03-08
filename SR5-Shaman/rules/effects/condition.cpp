#include "condition.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include "effect.h"
#include "rules/rules.h"
#include "data/character/charactervalues.h"
#include "data/character/characterchoices.h"

//---------------------------------------------------------------------------------
Condition::Condition(Effect* p_parent, QJsonValueRef* p_jsonObject)
    : _conditionType(CONDITIONTYPE_INVALID)
    , _parent(p_parent)
{
    // A condition might be anything from a simple string to a more complex object
    QJsonValueRef val = *p_jsonObject;
    QString tempString;
    if (val.type() == QJsonValue::String)
    {
        tempString = val.toString();
        if (tempString == "skill_known")
        {
            _conditionType = CONDITIONTYPE_SKILL_KNOWN;
        }
    }
    else if (val.type() == QJsonValue::Object)
    {

    }
}

//---------------------------------------------------------------------------------
Condition::~Condition()
{
}

//---------------------------------------------------------------------------------
bool
Condition::isFulfilled()
{
    if (_conditionType == CONDITIONTYPE_SKILL_KNOWN)
    {
        // Get the target skill and check if it is known to the character
        QString targetSkill = _parent->getCurrentTarget();
        if (CHARACTER_VALUES->getSkill(targetSkill) > 0)
        {
            return true;
        }
        else
        {
            _lastError = QObject::tr("Skill %1 not known to the character.").arg(targetSkill);
        }
    }

    return false;
}
