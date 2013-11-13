#include "attributerules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>

//---------------------------------------------------------------------------------
AttributeRules::AttributeRules()
{

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
        for (int i = p_oldValue; i >= p_newValue; --i)
        {
            result -= 5 * i;
        }
    }
    // Calculate the cost
    else
    {
        for (int i = p_oldValue; i >= p_newValue; ++i)
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
