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
Condition::Condition(Effect *p_parent, QJsonObject *p_jsonObject)
    : _conditionType(CONDITIONTYPE_INVALID)
    , _parent(p_parent)
{

}

//---------------------------------------------------------------------------------
Condition::~Condition()
{
}

//---------------------------------------------------------------------------------
bool
Condition::isFullfilled() const
{

}
