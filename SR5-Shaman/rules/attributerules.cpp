#include "attributerules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>

#include "rules/rules.h"

//---------------------------------------------------------------------------------
AttributeRules::AttributeRules()
{
    _validBaseAttributes.insert("body");
    _validBaseAttributes.insert("agility");
    _validBaseAttributes.insert("reaction");
    _validBaseAttributes.insert("strength");
    _validBaseAttributes.insert("willpower");
    _validBaseAttributes.insert("logic");
    _validBaseAttributes.insert("intuition");
    _validBaseAttributes.insert("charisma");
    _validBaseAttributes.insert("edge");
    _validBaseAttributes.insert("magic");
}

//---------------------------------------------------------------------------------
AttributeRules::~AttributeRules()
{

}

//---------------------------------------------------------------------------------
void
AttributeRules::initialize(const QString& p_jsonFile)
{
    // Read the JSON file
    QString val;
    QFile file;
    file.setFileName(p_jsonFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    // Read document
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError)
    {
        qCritical() << QString("Error while reading file: %1\nError: %2").arg(p_jsonFile, error.errorString());
        return;
    }

    // Get the number of free attribute points per priority
    QJsonArray freebieArray = doc.object().value("priority_freebies").toArray();
    _freebiesPerPrio.push_back(freebieArray.at(0).toVariant().toInt());
    _freebiesPerPrio.push_back(freebieArray.at(1).toVariant().toInt());
    _freebiesPerPrio.push_back(freebieArray.at(2).toVariant().toInt());
    _freebiesPerPrio.push_back(freebieArray.at(3).toVariant().toInt());
    _freebiesPerPrio.push_back(freebieArray.at(4).toVariant().toInt());
}

//---------------------------------------------------------------------------------
bool
AttributeRules::isValidAttribute(const QString& p_attribute) const
{
    return _validBaseAttributes.find(p_attribute) != _validBaseAttributes.end();
}

//---------------------------------------------------------------------------------
int
AttributeRules::getNumFreebies(int p_prioIndex) const
{
    if (p_prioIndex == -1)
    {
        return 0;
    }

    return _freebiesPerPrio[p_prioIndex];
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateMaximumAttributeIncrease(int p_currentValue, int p_maxValue, int p_availableKarma) const
{
    int cost = 0;
    int increases = 0;
    for (int i = p_currentValue; i < p_maxValue; ++i)
    {
        cost += calculateAttributeIncreaseCost(i, i+1);

        if (cost > p_availableKarma)
        {
            return increases;
        }

        ++increases;
    }

    // Shouldn't be reached
    return 0;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateAttributeIncreaseCost(int p_oldValue, int p_newValue) const
{
    int result = 0;
    bool decrease = p_oldValue > p_newValue;

    // Calculate the gain
    if (decrease)
    {
        for (int i = p_oldValue; i > p_newValue; --i)
        {
            result -= 5 * i;
        }
    }
    // Calculate the cost
    else
    {
        for (int i = p_newValue; i > p_oldValue; --i)
        {
            result += 5 * i;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculatePhysicalLimit(int p_strength, int p_body, int p_reaction) const
{
    float temp = (p_strength * 2.0f + p_body + p_reaction) / 3.0f;
    return temp + 0.5f;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateMentalLimit(int p_logic, int p_intuition, int p_willpower) const
{
    float temp = (p_logic * 2.0f + p_intuition + p_willpower) / 3.0f;
    return temp + 0.5f;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateSocialLimit(int p_charisma, int p_willpower, int p_essence) const
{
    float temp = (p_charisma * 2.0f + p_willpower + p_essence) / 3.0f;
    return temp + 0.5f;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateInitiative(int p_intuition, int p_reaction) const
{
    return (p_intuition + p_reaction);
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateMatrixInitiative(int p_intuition, int p_reaction) const
{
    return (p_intuition + p_reaction);
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateAstralInitiative(int p_intuition) const
{
    return p_intuition << 1;
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateComposure(int p_charisma, int p_will) const
{
    return (p_charisma + p_will);
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateJudgeIntentions(int p_charisma, int p_intuition) const
{
    return (p_charisma + p_intuition);
}

//---------------------------------------------------------------------------------
int
AttributeRules::calculateMemory(int p_logic, int p_willpower) const
{
    return (p_logic + p_willpower);
}

//---------------------------------------------------------------------------------
LiftCarryValues
AttributeRules::calculateLiftCarry(int p_strength, int p_body) const
{
    LiftCarryValues result;
    result.liftBase = 15 * p_strength;
    result.carryBase = 10 * p_strength;
    result.dicePool = p_strength + p_body;

    return result;
}

//---------------------------------------------------------------------------------
MovementValues
AttributeRules::calculateMovement(int p_agility, const QString& p_metatypeID) const
{
    // Sanity check - metatype
    if (p_metatypeID == "")
    {
        qWarning() << QString("Cannot calculate movement without a set metatype.");
        MovementValues result;
        result.running = 0;
        result.sprintIncrease = 0;
        result.walking = 0;
        return result;
    }

    MovementValues result;
    result.walking = p_agility << 1;
    result.running = p_agility << 2;
    result.sprintIncrease = METATYPE_RULES->getDefinition(p_metatypeID).sprintIncrease;

    return result;
}
