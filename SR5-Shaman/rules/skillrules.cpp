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
SkillDefinition::SkillDefinition(SkillDefinition* p_parent)
    : parent(p_parent)
    , id("")
    , isCategory(false), isGroup(false), isLanguage(false), isUserDefined(false), requiresCustom(false)
    , type(SKILL_TYPE_INVALID)
    , knowledgeType(KNOWLEDGE_TYPE_INVALID)
    , custom("")
    , attribute("")
    , group("")
{
    children.clear();
}

//---------------------------------------------------------------------------------
SkillDefinition::SkillDefinition(const SkillDefinition& p_other)
{
    parent = p_other.parent;
    id = p_other.id;
    isCategory = p_other.isCategory;
    isGroup = p_other.isGroup;
    isLanguage = p_other.isLanguage;
    isUserDefined = p_other.isUserDefined;
    requiresCustom = p_other.requiresCustom;
    type = p_other.type;
    knowledgeType = p_other.knowledgeType;
    custom = p_other.custom;
    attribute = p_other.attribute;
    group = p_other.group;
    children = p_other.children;
}

//---------------------------------------------------------------------------------
SkillDefinition::~SkillDefinition()
{

}

//---------------------------------------------------------------------------------
bool
SkillDefinition::hasChild(const QString& p_id) const
{
    for (unsigned int i = 0; i < children.size(); ++i)
    {
        if (children[i]->id == p_id)
        {
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------
SkillDefinition*
SkillDefinition::getChild(const QString& p_id) const
{
    for (unsigned int i = 0; i < children.size(); ++i)
    {
        if (children[i]->id == p_id)
        {
            return children[i];
        }
    }
    return NULL;
}

//---------------------------------------------------------------------------------
SkillRules::SkillRules()
{
    // Construct root item definition (this is used for display in trees
    // Construct root item
    _rootItem = new SkillDefinition();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // THE ORDER IS VERY IMPORTANT HERE, DEPENDS ON THE SKILL TYPE ENUM
    // Combat
    SkillDefinition* category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_COMBAT";
    category->type = SKILL_TYPE_COMBAT;
    _rootItem->children.push_back(category);
    // Physical
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_PHYSICAL";
    category->type = SKILL_TYPE_PHYSICAL;
    _rootItem->children.push_back(category);
    // Social
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_SOCIAL";
    category->type = SKILL_TYPE_SOCIAL;
    _rootItem->children.push_back(category);
    // Magical
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_MAGIC";
    category->type = SKILL_TYPE_MAGIC;
    _rootItem->children.push_back(category);
    // Resonance
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY";
    category->type = SKILL_TYPE_RESONANCE;
    _rootItem->children.push_back(category);
    // Technical
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_TECHNICAL";
    category->type = SKILL_TYPE_TECHNICAL;
    _rootItem->children.push_back(category);
    // Vehicle
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_VEHICLE";
    category->type = SKILL_TYPE_VEHICLE;
    _rootItem->children.push_back(category);
    // Knowledge
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_KNOWLEDGE";
    category->type = SKILL_TYPE_KNOWLEDGE;
    _rootItem->children.push_back(category);
    // Groups - this is a workaround
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_GROUPS";
    category->type = NUM_SKILL_TYPES;
    _rootItem->children.push_back(category);
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
    SkillDefinition* skillDef = NULL;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QJsonObject tempObject2;
    SkillDefinition* category = NULL;
    for (int i = 0; i < skillsArray.size(); ++i)
    {
        currentSkill = skillsArray.at(i).toObject();

        // Add type definition
        skillDef = new SkillDefinition();
        skillDef->isGroup = true;
        skillDef->id = currentSkill["unique_id"].toString();

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

        // Get the correct category - groups
        category = _rootItem->children[NUM_SKILL_TYPES];
        skillDef->parent = category;
        skillDef->type = NUM_SKILL_TYPES;

        // Add to category
        category->children.push_back(skillDef);

        _definitions[skillDef->id] = skillDef;
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
        skillDef->id = uniqueId;

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
                _definitions[group]->children.push_back(skillDef);
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

        // Get the correct category
        category = _rootItem->children[skillDef->type];
        skillDef->parent = category;

        // Add to category
        category->children.push_back(skillDef);

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

    return QObject::tr("This shouldn't happen");
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
        if (!p_onlyGroups && it.value()->type == p_type && !it.value()->isUserDefined)
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

            if (add && !it.value()->isUserDefined)
            {
                result.push_back(std::make_pair(it.key(), it.value()));
            }
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
QString
SkillRules::constructCustomizedSkill(const QString& p_id, const QString& p_customValue)
{
    // Construct new ID
    const SkillDefinition& originalSkill = getDefinition(p_id);
    QString newID = p_id + "_" + p_customValue;

    // If a skill with that ID already exists, don't add it again
    if (_definitions.find(newID) != _definitions.end())
    {
        return newID;
    }

    // Create the new skill
    SkillDefinition* newSkill = new SkillDefinition();
    newSkill->id = newID;
    newSkill->attribute = originalSkill.attribute;
    newSkill->custom = p_customValue;
    newSkill->requiresCustom = false;
    newSkill->isUserDefined = true;
    newSkill->parent = originalSkill.parent;
    newSkill->children = originalSkill.children;
    newSkill->group = originalSkill.group;
    if (newSkill->group != "" && newSkill->group != "none")
    {
        _definitions[newSkill->group]->children.push_back(newSkill);
        _definitions[newSkill->group]->groupSkills[newID] = newSkill;
    }
    newSkill->groupSkills = originalSkill.groupSkills;
    newSkill->isCategory = originalSkill.isCategory;
    newSkill->isGroup = originalSkill.isGroup;
    newSkill->isLanguage = originalSkill.isLanguage;
    newSkill->type = originalSkill.type;
    _rootItem->children[newSkill->type]->children.push_back(newSkill);
    newSkill->knowledgeType = originalSkill.knowledgeType;
    newSkill->translations = originalSkill.translations;
    QMap<QString, QString>::iterator it;
    for (it = newSkill->translations.begin(); it != newSkill->translations.end(); ++it)
    {
        it.value().append(" (" + p_customValue + ")");
    }

    // Add the skill
    _definitions[newID] = newSkill;
    return newID;
}
