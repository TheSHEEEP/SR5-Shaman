#include "skillrules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>

//---------------------------------------------------------------------------------
SkillRules::SkillRules()
{

}

//---------------------------------------------------------------------------------
SkillRules::~SkillRules()
{
    // Clean definitions
    QMap<QString, SkillDefinition*>::iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        delete (*it);
    }
}

//---------------------------------------------------------------------------------
void
SkillRules::initialize(const QString& p_jsonFile)
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

    // Parse each skill group and add to the rules
    QJsonArray skillsArray = doc.object().value("skill_groups").toArray();
    QJsonObject currentSkill;
    SkillDefinition* typeDef = 0;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QJsonObject tempObject2;
    for (int i = 0; i < skillsArray.size(); ++i)
    {
        currentSkill = skillsArray.at(i).toObject();

        // Add type definition
        typeDef = new SkillDefinition();

        // TODO: here
    }

    // Parse each skill and add to the rules
    skillsArray = doc.object().value("skills").toArray();
    typeDef = 0;
    for (int i = 0; i < skillsArray.size(); ++i)
    {
        currentSkill = skillsArray.at(i).toObject();

        // Add type definition
        typeDef = new SkillDefinition();

        // Translations
        tempObject = currentSkill["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            typeDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        /*// Types
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
        }*/

        _definitions[currentSkill["unique_id"].toString()] = typeDef;
    }
}
