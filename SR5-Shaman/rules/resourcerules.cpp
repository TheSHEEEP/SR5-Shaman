#include "resourcerules.h"

#include <utility>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>
#include <QString>
#include "data/dictionary.h"

//---------------------------------------------------------------------------------
ResourceDefinition::ResourceDefinition(ResourceDefinition* p_parent)
    : parent(p_parent)
    , id("")
    , isCategory(false)
    , isUserDefined(false), requiresCustom(false)
    , custom("")
    , type(RESOURCE_TYPE_INVALID)
{
    children.clear();
}

//---------------------------------------------------------------------------------
ResourceDefinition::ResourceDefinition(const ResourceDefinition& p_other)
{
    parent = p_other.parent;
    id = p_other.id;
    isCategory = p_other.isCategory;
    isUserDefined = p_other.isUserDefined;
    requiresCustom = p_other.requiresCustom;
    type = p_other.type;
    custom = p_other.custom;
    children = p_other.children;
}

//---------------------------------------------------------------------------------
ResourceDefinition::~ResourceDefinition()
{

}

//---------------------------------------------------------------------------------
bool
ResourceDefinition::hasChild(const QString& p_id) const
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
ResourceDefinition*
ResourceDefinition::getChild(const QString& p_id) const
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
ResourceRules::ResourceRules()
{
    // Construct root item definition (this is used for display in trees
    // Construct root item
    _rootItem = new ResourceDefinition();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // THE ORDER IS VERY IMPORTANT HERE, DEPENDS ON THE SKILL TYPE ENUM
    // Combat
    /*SkillDefinition* category = new SkillDefinition(_rootItem);
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
    category->id = "CATEGORY_RESONANCE";
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
    // Language
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_LANGUAGE";
    category->type = SKILL_TYPE_LANGUAGE;
    _rootItem->children.push_back(category);
    // Groups - this is a workaround
    category = new SkillDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_GROUPS";
    category->type = NUM_SKILL_TYPES;
    _rootItem->children.push_back(category);*/
}

//---------------------------------------------------------------------------------
ResourceRules::~ResourceRules()
{
    // Clean definitions
    QMap<QString, ResourceDefinition*>::iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        delete (*it);
    }
}

//---------------------------------------------------------------------------------
void
ResourceRules::initialize(const QString& p_jsonFile)
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

    // Read the resource points
    QJsonArray pointsArray = doc.object().value("skill_points_per_prio").toArray();
    _skillPoints.push_back(QPair<int,int>(  pointsArray[0].toArray()[0].toString().toInt(),
                                            pointsArray[0].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[1].toArray()[0].toString().toInt(),
                                            pointsArray[1].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[2].toArray()[0].toString().toInt(),
                                            pointsArray[2].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[3].toArray()[0].toString().toInt(),
                                            pointsArray[3].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[4].toArray()[0].toString().toInt(),
                                            pointsArray[4].toArray()[1].toString().toInt()));

    /*// Parse each skill group and add to the rules
    QJsonArray skillsArray = doc.object().value("skill_groups").toArray();
    QJsonObject currentSkill;
    SkillDefinition* skillDef = NULL;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QJsonObject tempObject2;
    SkillDefinition* category = NULL;

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
    }*/
}

//---------------------------------------------------------------------------------
QString
ResourceRules::getTypeString(ResourceType p_type) const
{
    switch (p_type)
    {

    case NUM_RESOURCE_TYPES:
    case RESOURCE_TYPE_INVALID:
        return Dictionary::getTranslation("INVALID_RESOURCE_TYPE");
    }

    return QObject::tr("This shouldn't happen");
}

//---------------------------------------------------------------------------------
std::vector<std::pair<QString, ResourceDefinition*> >
ResourceRules::getDefinitionsContaining(const QString& p_idPart) const
{
    std::vector<std::pair<QString, ResourceDefinition*> > result;

    // Iterate over all definitions to find those that fit the parameters
    QMap<QString, ResourceDefinition*>::const_iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        if (it.value()->id.contains(p_idPart))
        {
            result.push_back(std::make_pair(it.key(), it.value()));
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
std::vector<std::pair<QString, ResourceDefinition*> >
ResourceRules::getDefinitionsByType(ResourceType p_type) const
{
    std::vector<std::pair<QString, ResourceDefinition*> > result;

    // Iterate over all definitions to find those that fit the parameters
    QMap<QString, ResourceDefinition*>::const_iterator it;
    QMap<QString, ResourceDefinition*>::const_iterator groupIt;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        if (it.value()->type == p_type && !it.value()->isUserDefined)
        {
            result.push_back(std::make_pair(it.key(), it.value()));
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
void
ResourceRules::removeResource(const QString& p_resource)
{
    // Remove from parent
    ResourceDefinition* resource = _definitions[p_resource];
    ResourceDefinition* parent = resource->parent;
    for (unsigned int i = 0; i < parent->children.size(); ++i)
    {
        if (parent->children[i] == resource)
        {
            parent->children.erase(parent->children.begin() + i);
            break;
        }
    }

    // Remove from definitions
    _definitions.remove(p_resource);

    // Clean up
    delete resource;
}

//---------------------------------------------------------------------------------
QString
ResourceRules::constructCustomizedResource(const QString& p_id, const QString& p_customValue)
{
    // Construct new ID
    const ResourceDefinition& originalResource = getDefinition(p_id);
    QString newID = p_id + "_" + p_customValue;

    // If a resource with that ID already exists, don't add it again
    if (_definitions.find(newID) != _definitions.end())
    {
        return newID;
    }

    // Create the new skill
    ResourceDefinition* newResource = new ResourceDefinition();
    newResource->id = newID;
    newResource->custom = p_customValue;
    newResource->requiresCustom = false;
    newResource->isUserDefined = true;
    newResource->parent = originalResource.parent;
    newResource->children = originalResource.children;
    newResource->isCategory = originalResource.isCategory;
    newResource->type = originalResource.type;
    _rootItem->children[newResource->type]->children.push_back(newResource);
    newResource->translations = originalResource.translations;
    QMap<QString, QString>::iterator it;
    for (it = newResource->translations.begin(); it != newResource->translations.end(); ++it)
    {
        it.value().append(" (" + p_customValue + ")");
    }

    // Add the resource
    _definitions[newID] = newResource;
    return newID;
}
