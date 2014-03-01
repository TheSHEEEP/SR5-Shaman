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
Effect::Effect(QJsonObject* p_jsonObject)
    : _effectType(EFFECTTYPE_INVALID)
    , _target("")
    , _limit("")
{

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
Effect::canBeApplied(const QString& p_target) const
{

}
