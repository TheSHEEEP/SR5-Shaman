#include "magicrules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>

//---------------------------------------------------------------------------------
MagicRules::MagicRules()
{

}

//---------------------------------------------------------------------------------
MagicRules::~MagicRules()
{
    // Clean definitions
    QMap<QString, MagicTypeDefinition*>::iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        QMap<int, MagicTypePriorityDefinition*>::iterator prioIt;
        for (prioIt = (*it)->priorities.begin(); prioIt != (*it)->priorities.end(); ++prioIt)
        {
            delete (*prioIt);
        }

        delete (*it);
    }
}

//---------------------------------------------------------------------------------
void
MagicRules::initialize(const QString& p_jsonFile)
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

    // Parse each magic type and add to the rules
    QJsonArray magicTypesArray = doc.object().value("magic_types").toArray();
    QJsonObject currentType;
    MagicTypeDefinition* typeDef = 0;
    MagicTypePriorityDefinition* prioDef = 0;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QJsonObject tempObject2;
    for (int i = 0; i < magicTypesArray.size(); ++i)
    {
        currentType = magicTypesArray.at(i).toObject();

        // Add type definition
        typeDef = new MagicTypeDefinition();

        // Translations
        tempObject = currentType["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            typeDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Types
        tempArray = currentType["types"].toArray();
        for (int j = 0; j < tempArray.size(); ++j)
        {
            typeDef->types.push_back(tempArray.at(j).toString());
        }

        // Priorities
        tempObject = currentType["priorities"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            tempObject2 = tempObject[tempObject.keys().at(j)].toObject();

            prioDef = new MagicTypePriorityDefinition();

            // Starting magic
            if (tempObject2.contains("starting_magic"))
            {
                prioDef->startingMagic = tempObject2.value("starting_magic").toString().toInt();
            }

            // Free spells
            if (tempObject2.contains("free_spells"))
            {
                prioDef->freeSpells = tempObject2.value("free_spells").toString().toInt();
            }

            // Free skills
            if (tempObject2.contains("free_skills"))
            {
                prioDef->freeSkills.first = tempObject2.value("free_skills").toArray().at(0).toString().toInt();
                prioDef->freeSkills.second = tempObject2.value("free_skills").toArray().at(1).toString().toInt();
            }

            // Forced skill group
            if (tempObject2.contains("forced_skill_groups"))
            {
                prioDef->freeSkillGroup.first = tempObject2.value("forced_skill_groups").toArray().at(0).toString().toInt();
                prioDef->freeSkillGroup.second = tempObject2.value("forced_skill_groups").toArray().at(1).toString().toInt();
            }

            // Free power points
            if (tempObject2.contains("free_power_points"))
            {
                prioDef->freePowerPoints = tempObject2.value("free_power_points").toString().toInt();
            }

            // Store priority
            typeDef->priorities[tempObject.keys().at(j).toInt()] = prioDef;
        }

        _definitions[currentType["unique_id"].toString()] = typeDef;
    }
}
