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
    // THE ORDER IS VERY IMPORTANT HERE, DO NOT CHANGE IT
    // Melee Weapons
    ResourceDefinition* category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_MELEE_WEAPONS";
    category->type = RESOURCE_TYPE_MELEE_WEAPON;
    _rootItem->children.push_back(category);
        // Blades
        ResourceDefinition* subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_MELEE_BLADES";
        subCategory->type = RESOURCE_TYPE_MELEE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_MELEE_BLADES;
        category->children.push_back(subCategory);
        // Clubs
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_MELEE_CLUBS";
        subCategory->type = RESOURCE_TYPE_MELEE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_MELEE_CLUBS;
        category->children.push_back(subCategory);
        // Other
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_MELEE_OTHER";
        subCategory->type = RESOURCE_TYPE_MELEE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_MELEE_OTHER;
        category->children.push_back(subCategory);
    // Projectile Weapons
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_PROJECTILE_WEAPONS";
    category->type = RESOURCE_TYPE_PROJECTILE_WEAPON;
    _rootItem->children.push_back(category);
        // Bows
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_PROJECTILE_BOWS";
        subCategory->type = RESOURCE_TYPE_PROJECTILE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_PROJECTILE_BOWS;
        category->children.push_back(subCategory);
        // Crossbows
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_PROJECTILE_CROSSBOWS";
        subCategory->type = RESOURCE_TYPE_PROJECTILE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_PROJECTILE_CROSSBOWS;
        category->children.push_back(subCategory);
        // Other
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_PROJECTILE_THROWING";
        subCategory->type = RESOURCE_TYPE_PROJECTILE_WEAPON;
        subCategory->subType = RESOURCE_SUBTYPE_PROJECTILE_THROWING;
        category->children.push_back(subCategory);
    // Firearms
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_FIREARMS";
    category->type = RESOURCE_TYPE_FIREARM;
    _rootItem->children.push_back(category);
        // Tasers
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_TASERS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_TASERS;
        category->children.push_back(subCategory);
        // Hold-Outs
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_HOLDOUTS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_HOLDOUTS;
        category->children.push_back(subCategory);
        // Light Pistols
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_LPISTOLS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_LPISTOLS;
        category->children.push_back(subCategory);
        // Heavy Pistols
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_HPISTOLS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_HPISTOLS;
        category->children.push_back(subCategory);
        // Machine Pistols
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_MPISTOLS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_MPISTOLS;
        category->children.push_back(subCategory);
        // SMGs
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_SMGS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_SMGS;
        category->children.push_back(subCategory);
        // Assault Rifles
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_ASSAULTR";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_ASSAULTR;
        category->children.push_back(subCategory);
        // Sniper Rifles
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_SNIPERR";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_SNIPERR;
        category->children.push_back(subCategory);
        // Shotguns
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_SHOTGUNS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_SHOTGUNS;
        category->children.push_back(subCategory);
        // Special Weapons
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_SPECIALW";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_SPECIALW;
        category->children.push_back(subCategory);
        // Machine Guns
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_MACHINEG";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_MACHINEG;
        category->children.push_back(subCategory);
        // Cannons & Launcher
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_FIREARM_CANNONS_LAUNCHERS";
        subCategory->type = RESOURCE_TYPE_FIREARM;
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_LAUNCHERS;
        category->children.push_back(subCategory);
    // Ammunition
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_AMMUNITION";
    category->type = RESOURCE_TYPE_AMMUNITION;
    _rootItem->children.push_back(category);
        // Grenades
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AMMU_GRENADES";
        subCategory->type = RESOURCE_TYPE_AMMUNITION;
        subCategory->subType = RESOURCE_SUBTYPE_AMMU_GRENADES;
        category->children.push_back(subCategory);
        // Grenades
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AMMU_ROCKETS";
        subCategory->type = RESOURCE_TYPE_AMMUNITION;
        subCategory->subType = RESOURCE_SUBTYPE_AMMU_ROCKETS;
        category->children.push_back(subCategory);
    // Explosives
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_EXPLOSIVES";
    category->type = RESOURCE_TYPE_EXPLOSIVES;
    _rootItem->children.push_back(category);
    // Clothing and armor
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_CLOTHING_ARMOR";
    category->type = RESOURCE_TYPE_CLOTHING_ARMOR;
    _rootItem->children.push_back(category);
        // Clothing
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_CLOTHING";
        subCategory->type = RESOURCE_TYPE_CLOTHING_ARMOR;
        category->children.push_back(subCategory);
        // Armor
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ARMOR";
        subCategory->type = RESOURCE_TYPE_CLOTHING_ARMOR;
        category->children.push_back(subCategory);
    // Electronics
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_ELECTRONICS";
    category->type = RESOURCE_TYPE_ELECTRONICS;
    _rootItem->children.push_back(category);
        // Commlinks
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_COMMLINKS";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Cyberdecks
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_CYBERDECKS";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Accessories
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_ACCESSORIES";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Rigger consoles
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_RIGGERC";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // RFID tags
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_RFID";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Communication
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_COMMUNICATION";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Software
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_SOFTWARE";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Skillsoft
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_SKILLSOFT";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Credsticks
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_CREDSTICKS";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Identification
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_ID";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Tools
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_TOOLS";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Optical & imaging
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_OPTICAL_IMAGING";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Vision enhancements
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_VISION_ENH";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Audio devices
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_AUDIO_DEV";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Audio enhancements
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_AUDIO_ENH";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Sensors
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_SENSORS";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Security devices
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_SECURITY_DEV";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
        // Breaking & entering
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_ELECTRONICS_BE_GEAR";
        subCategory->type = RESOURCE_TYPE_ELECTRONICS;
        category->children.push_back(subCategory);
    // Industrial chemicals
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_INDUSTRIAL_CHEMICALS";
    category->type = RESOURCE_TYPE_INDUSTRIAL_CHEMICALS;
    _rootItem->children.push_back(category);
    // Survival gear
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_SURVIVAL_GEAR";
    category->type = RESOURCE_TYPE_SURVIVAL_GEAR;
    _rootItem->children.push_back(category);
    // Grapple & flare gun
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_GRAPPLE_FLARE";
    category->type = RESOURCE_TYPE_GRAPPLE_FLARE;
    _rootItem->children.push_back(category);
    // Biotech
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_BIOTECH";
    category->type = RESOURCE_TYPE_BIOTECH;
    _rootItem->children.push_back(category);
    // Docwagon
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_DOCWAGON";
    category->type = RESOURCE_TYPE_DOCWAGON;
    _rootItem->children.push_back(category);
    // Augmentation
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_AUGMENTATION";
    category->type = RESOURCE_TYPE_AUGMENTATION;
    _rootItem->children.push_back(category);
        // Headware
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_HEADWARE";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
        // Eyeware
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_EYEWARE";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
        // Earware
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_EARWARE";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
        // Bodyware
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_BODYWARE";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
        // Cyberlimbs
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_CYBERLIMBS";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
        // Bioware
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_AUG_BIOWARE";
        subCategory->type = RESOURCE_TYPE_AUGMENTATION;
        category->children.push_back(subCategory);
    // Magical equipment
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_MAGICAL_EQUIPMENT";
    category->type = RESOURCE_TYPE_MAGICAL_EQUIPMENT;
    _rootItem->children.push_back(category);
    // Vehicles and drones
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_VEHICLES_DRONES";
    category->type = RESOURCE_TYPE_VEHICLES_DRONES;
    _rootItem->children.push_back(category);
        // Bikes
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_BIKES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Cars
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_CARS";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Trucks and vans
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_TRUCKS_VANS";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Boats
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_BOATS";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Submarines
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_SUBMARINES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Fixed-wing aircraft
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_FWAIRCRAFT";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Rotorcraft
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_ROTORCRAFT";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // VTOL/VSTOL
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_VTOL";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Microdrones
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_MICRODRONES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Minidrohnen
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_MINIDRONES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Small drones
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_SMALL_DRONES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Medium drones
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_MEDIUM_DRONES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
        // Large drones
        subCategory = new ResourceDefinition(category);
        subCategory->isCategory = true;
        subCategory->id = "CATEGORY_VD_LARGE_DRONES";
        subCategory->type = RESOURCE_TYPE_VEHICLES_DRONES;
        category->children.push_back(subCategory);
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
    /*QJsonArray pointsArray = doc.object().value("skill_points_per_prio").toArray();
    _skillPoints.push_back(QPair<int,int>(  pointsArray[0].toArray()[0].toString().toInt(),
                                            pointsArray[0].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[1].toArray()[0].toString().toInt(),
                                            pointsArray[1].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[2].toArray()[0].toString().toInt(),
                                            pointsArray[2].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[3].toArray()[0].toString().toInt(),
                                            pointsArray[3].toArray()[1].toString().toInt()));
    _skillPoints.push_back(QPair<int,int>(  pointsArray[4].toArray()[0].toString().toInt(),
                                            pointsArray[4].toArray()[1].toString().toInt()));*/

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
