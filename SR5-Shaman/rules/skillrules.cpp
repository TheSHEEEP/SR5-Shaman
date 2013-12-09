#include "skillrules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>
#include <QString>

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
    SkillDefinition* skillDef = 0;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QJsonObject tempObject2;
    for (int i = 0; i < skillsArray.size(); ++i)
    {
        currentSkill = skillsArray.at(i).toObject();

        // Add type definition
        skillDef = new SkillDefinition();
        skillDef->isGroup = true;

        // Translations
        tempObject = currentSkill["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            skillDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Make sure the definition doesn't already exist
        if (_definitions.contains(currentSkill["unique_id"].toString()))
        {
            qCritical() << "Skill \"" << currentSkill["unique_id"].toString() << "\" already exists. Skill parsing aborted.";
            return;
        }

        _definitions[currentSkill["unique_id"].toString()] = skillDef;
    }

    // Parse each skill and add to the rules
    skillsArray = doc.object().value("skills").toArray();
    skillDef = 0;
    QString group = "";
    QString uniqueId = "";
    QString type = "";
    for (int i = 0; i < skillsArray.size(); ++i)
    {
        currentSkill = skillsArray.at(i).toObject();

        // ID
        uniqueId = currentSkill["unique_id"].toString();

        // Add type definition
        skillDef = new SkillDefinition();

        // Translations
        tempObject = currentSkill["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            skillDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Attribute
        skillDef->attribute = currentSkill["attribute"].toString();

        // Group
        group = currentSkill["group"].toString();
        skillDef->group = group;
        if (group != "none")
        {
            // Add to the group
            if (_definitions.contains(group))
            {
                _definitions[group]->groupSkills[uniqueId] = skillDef;
            }
            // Error! group doesn't exist
            else
            {
                qCritical() << "Skill group \"" << group << "\" doesn't exist. Requested by skill \""
                            <<  uniqueId << "\". Skill parsing aborted.";
                return;
            }
        }

        // Requires custom?
        if (currentSkill.contains("requires_custom"))
        {
            skillDef->requiresCustom = currentSkill["requires_custom"].toString() == "true";
        }

        // Type
        type = currentSkill["type"].toString();
        if (type == "combat")
        {
            skillDef->type = SKILL_TYPE_COMBAT;
        }
        else if (type == "physical")
        {
            skillDef->type = SKILL_TYPE_PHYSICAL;
        }
        else if (type == "social")
        {
            skillDef->type = SKILL_TYPE_SOCIAL;
        }
        else if (type == "magic")
        {
            skillDef->type = SKILL_TYPE_MAGIC;
        }
        else if (type == "resonance")
        {
            skillDef->type = SKILL_TYPE_RESONANCE;
        }
        else if (type == "technical")
        {
            skillDef->type = SKILL_TYPE_TECHNICAL;
        }
        else if (type == "vehicle")
        {
            skillDef->type = SKILL_TYPE_VEHICLE;
        }
        else if (type == "knowledge")
        {
            skillDef->type = SKILL_TYPE_KNOWLEDGE;
        }
        else
        {
            qCritical() << "Skill \"" << uniqueId << "\" does not have a valid type. Parsing aborted.";
            return;
        }

        // Make sure the definition doesn't already exist
        if (_definitions.contains(uniqueId))
        {
            qCritical() << "Skill \"" << uniqueId << "\" already exists. Skill parsing aborted.";
            return;
        }
        _definitions[currentSkill["unique_id"].toString()] = skillDef;
    }
}

//---------------------------------------------------------------------------------
QString
SkillRules::getTypeString(SkillType p_type) const
{
    switch (p_type)
    {
    case SKILL_TYPE_COMBAT:
        return QObject::tr("Combat");

    case SKILL_TYPE_KNOWLEDGE:
        return QObject::tr("Knowledge");

    case SKILL_TYPE_MAGIC:
        return QObject::tr("Magic");

    case SKILL_TYPE_PHYSICAL:
        return QObject::tr("Physical");

    case SKILL_TYPE_RESONANCE:
        return QObject::tr("Resonance");

    case SKILL_TYPE_SOCIAL:
        return QObject::tr("Social");

    case SKILL_TYPE_TECHNICAL:
        return QObject::tr("Technical");

    case SKILL_TYPE_VEHICLE:
        return QObject::tr("Vehicle");

    case NUM_SKILL_TYPES:
    case SKILL_TYPE_INVALID:
        return QObject::tr("Invalid Skilltype");
    }
}

//---------------------------------------------------------------------------------
std::vector<std::pair<QString, SkillDefinition*> >
SkillRules::getDefinitionsByType(SkillType p_type, bool p_onlyGroups) const
{
    std::vector<std::pair<QString, SkillDefinition*> > result;

    // Iterate over all definitions to find those that fit the parameters
    QMap<QString, SkillDefinition*>::const_iterator it;
    QMap<QString, SkillDefinition*>::const_iterator groupIt;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        if (!p_onlyGroups && it.value()->type == p_type)
        {
            result.push_back(std::make_pair(it.key(), it.value()));
        }
        else if (p_onlyGroups && it.value()->isGroup)
        {
            // For groups, we need to check each child and only if all children fit the type
            // the group is returned
            bool add = true;
            for (groupIt = it.value()->groupSkills.begin(); groupIt != it.value()->groupSkills.end(); ++groupIt)
            {
                if (groupIt.value()->type != p_type)
                {
                    add = false;
                    break;
                }
            }

            if (add)
            {
                result.push_back(std::make_pair(it.key(), it.value()));
            }
        }
    }

    return result;
}
