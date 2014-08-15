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
#include "rules/effects/resourceeffect.h"

//---------------------------------------------------------------------------------
ResourceDefinition::ResourceDefinition(ResourceDefinition* p_parent)
    : parent(p_parent)
    , id("")
    , isCategory(false)
    , isUserDefined(false), requiresCustom(false)
    , custom("")
    , type(RESOURCE_TYPE_INVALID)
    , subType(RESOURCE_SUBTYPE_INVALID)
    , availabilityNum("") , availabilityClass(AVAILABILITY_CLASS_INVALID), availabilityIsPerRating(false)
    , cost(""), costIsPerRating(false)
    , maxRating(0)
    , stacks(false)
    , wireless(false)
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
    subType = p_other.subType;
    custom = p_other.custom;
    children = p_other.children;

    availabilityNum = p_other.availabilityNum;
    availabilityClass = p_other.availabilityClass;
    availabilityIsPerRating = p_other.availabilityIsPerRating;
    cost = p_other.cost;
    costIsPerRating = p_other.costIsPerRating;
    maxRating = p_other.maxRating;
    stacks = p_other.stacks;
    wireless = p_other.wireless;
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
WeaponDefinition::WeaponDefinition(ResourceDefinition* p_parent)
    : ResourceDefinition(p_parent)
    , accuracy(0)
    , reach(0)
    , damageNum(""), damageType("")
    , ap(0), apIsModding(false), rc(0), rcIsModding(false)
    , clipSize(0)
    , clipType(WEAPON_CLIP_TYPE_INVALID)
    , blast("")
{
}

//---------------------------------------------------------------------------------
WeaponDefinition::WeaponDefinition(const WeaponDefinition& p_other)
    : ResourceDefinition(p_other)
{
    accuracy = p_other.accuracy;
    reach = p_other.reach;
    damageNum = p_other.damageNum;
    damageType = p_other.damageType;
    ap = p_other.ap;
    apIsModding = p_other.apIsModding;
    fireModi = p_other.fireModi;
    rc = p_other.rc;
    rcIsModding = p_other.rcIsModding;
    clipSize = p_other.clipSize;
    clipType = p_other.clipType;
    blast = p_other.blast;
}

//---------------------------------------------------------------------------------
WeaponDefinition::~WeaponDefinition()
{
}

//---------------------------------------------------------------------------------
MountDefinition::MountDefinition(ResourceDefinition* p_parent)
    : ResourceDefinition(p_parent)
    , permanent(false)
    , effectsArePerRating(false)
    , requiresWirelessHost(false)
    , requiresSmartlinkOnHost(false)
{
}

//---------------------------------------------------------------------------------
MountDefinition::MountDefinition(const MountDefinition& p_other)
    : ResourceDefinition(p_other)
{
    locations = p_other.locations;
    validSubtypes = p_other.validSubtypes;
    validItems = p_other.validItems;
    permanent = p_other.permanent;
    effectsArePerRating = p_other.effectsArePerRating;
    requiresWirelessHost = p_other.requiresWirelessHost;
    requiresSmartlinkOnHost = p_other.requiresSmartlinkOnHost;
}

//---------------------------------------------------------------------------------
MountDefinition::~MountDefinition()
{
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
        subCategory->subType = RESOURCE_SUBTYPE_FIREARM_CANNONS_LAUNCHERS;
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
    // Mounts
    category = new ResourceDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_MOUNTS";
    category->type = RESOURCE_TYPE_MOUNT;
    _rootItem->children.push_back(category);
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

// This will make the parsing function cost some lines less
#define CHECKIF(PARAM) \
    if (tempString == #PARAM) \
    { \
        resourceDef->subType = RESOURCE_SUBTYPE_##PARAM; \
    }
#define CHECKELSEIF(PARAM) \
    else if (tempString == #PARAM) \
    { \
        resourceDef->subType = RESOURCE_SUBTYPE_##PARAM; \
    }
#define ENDCHECK() \
    else \
    { \
        qCritical() << "Resource \"" << uniqueId << "\" of unknown subtype \"" \
                    << tempString << "\". Resource parsing aborted."; \
        return; \
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

    // Read the nuyen per priority points
    QJsonArray nuyenArray = doc.object().value("nuyen_per_prio").toArray();
    _nuyenPerPrio.push_back(nuyenArray[0].toString().toInt());
    _nuyenPerPrio.push_back(nuyenArray[1].toString().toInt());
    _nuyenPerPrio.push_back(nuyenArray[2].toString().toInt());
    _nuyenPerPrio.push_back(nuyenArray[3].toString().toInt());
    _nuyenPerPrio.push_back(nuyenArray[4].toString().toInt());

    // Parse each mount
    QJsonArray mountArray = doc.object().value("mounts").toArray();
    QJsonObject currentResource;
    ResourceDefinition* resourceDef = NULL;
    WeaponDefinition* weaponDef = NULL;
    MountDefinition* mountDef = NULL;
    QJsonArray tempArray;
    QJsonObject tempObject;
    ResourceDefinition* category = NULL;
    QString uniqueId = "";
    QString tempString = "";
    for (int i = 0; i < mountArray.size(); ++i)
    {
        currentResource = mountArray.at(i).toObject();

        // ID
        uniqueId = currentResource["unique_id"].toString();

        // Create correct type definition
        mountDef = new MountDefinition();
        mountDef->id = uniqueId;
        mountDef->type = RESOURCE_TYPE_MOUNT;

        // Translations
        tempObject = currentResource["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            mountDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Locations
        tempArray = currentResource["locations"].toArray();
        for (int j = 0; j < tempArray.size(); ++j)
        {
            mountDef->locations.push_back(tempArray[j].toString());
        }

        // Permanent
        mountDef->permanent = currentResource["permanent"].toString() == "true";

        // Max rating
        if (currentResource.contains("max_rating"))
        {
            mountDef->maxRating = currentResource["max_rating"].toString().toInt();
        }

        // Availabilities can have different keys
        fillAvailability(mountDef, &currentResource);

        // Cost can have different keys
        fillCost(mountDef, &currentResource);

        // Effects
        bool gotEffects = false;
        if (currentResource.contains("effects_per_rating"))
        {
            gotEffects = true;
            tempString = "effects_per_rating";
            mountDef->effectsArePerRating = true;
        }
        else if (currentResource.contains("effects"))
        {
            gotEffects = true;
            tempString = "effects";
        }
        if (gotEffects)
        {
            tempArray = currentResource[tempString].toArray();
            for (int j = 0; j < tempArray.size(); ++j)
            {
                QJsonValueRef obj = tempArray[j];
                mountDef->effects.push_back(new ResourceEffect(&obj));
            }
        }

        // Wireless
        if (currentResource.contains("wireless"))
        {
            mountDef->wireless = currentResource["wireless"].toString() == "true";
        }

        // Requires wireless host
        if (currentResource.contains("req_wireless"))
        {
            mountDef->requiresWirelessHost = currentResource["req_wireless"].toString() == "true";
        }

        // Requires smartlinked host
        if (currentResource.contains("req_smartlink"))
        {
            mountDef->requiresSmartlinkOnHost = currentResource["req_smartlink"].toString() == "true";
        }

        // Valid subtypes
        if (currentResource.contains("subtypes"))
        {
            tempArray = currentResource["subtypes"].toArray();
            for (int j = 0; j < tempArray.size(); ++j)
            {
                mountDef->validSubtypes.push_back(tempArray[j].toString());
            }
        }

        // Valid specific items
        if (currentResource.contains("items"))
        {
            tempArray = currentResource["items"].toArray();
            for (int j = 0; j < tempArray.size(); ++j)
            {
                mountDef->validItems.push_back(tempArray[j].toString());
            }
        }

        // Get the correct category
        category = _rootItem->children[mountDef->type];
        mountDef->parent = category;

        // Add to category
        category->children.push_back(mountDef);

        // Make sure the definition doesn't already exist
        if (_definitions.contains(uniqueId))
        {
            qCritical() << "Mount \"" << uniqueId << "\" already exists. Resource parsing aborted.";
            return;
        }
        _definitions[currentResource["unique_id"].toString()] = mountDef;
    }

    // Parse each resource
    QJsonArray resourceArray = doc.object().value("resources").toArray();
    currentResource;
    resourceDef = NULL;
    weaponDef = NULL;
    mountDef = NULL;
    category = NULL;
    uniqueId = "";
    tempString = "";
    ResourceType type;
    QJsonArray tempArray2;
    for (int i = 0; i < resourceArray.size(); ++i)
    {
        currentResource = mountArray.at(i).toObject();

        // ID
        uniqueId = currentResource["unique_id"].toString();

        // Get the correct type and create the according resource definition
        tempString = currentResource["type"].toString();
        if (tempString == "MELEE_WEAPON")
        {
            type = RESOURCE_TYPE_MELEE_WEAPON;
            weaponDef = new WeaponDefinition();
            resourceDef = static_cast<ResourceDefinition*>(weaponDef);
        }
        else if (tempString == "PROJECTILE_WEAPON")
        {
            type = RESOURCE_TYPE_PROJECTILE_WEAPON;
            weaponDef = new WeaponDefinition();
            resourceDef = static_cast<ResourceDefinition*>(weaponDef);
        }
        else if (tempString == "FIREARM")
        {
            type = RESOURCE_TYPE_FIREARM;
            weaponDef = new WeaponDefinition();
            resourceDef = static_cast<ResourceDefinition*>(weaponDef);
        }
        else if (tempString == "AMMUNITION")
        {
            type = RESOURCE_TYPE_AMMUNITION;
            weaponDef = new WeaponDefinition();
            resourceDef = static_cast<ResourceDefinition*>(weaponDef);
        }
        else
        {
            qCritical() << "resource \"" << uniqueId << "\" of unknown type \""
                        << tempString << "\". Resource parsing aborted.";
            return;
        }
        resourceDef->type = type;

        // Get the category & subtype
        // Most items are of a subtype and thus not in a top level category
        if (currentResource.contains("subtype"))
        {
            tempString = currentResource["subtype"].toString();

            switch (type)
            {
            case RESOURCE_TYPE_MELEE_WEAPON:
                CHECKIF(MELEE_BLADES)
                CHECKELSEIF(MELEE_CLUBS)
                CHECKELSEIF(MELEE_OTHER)
                ENDCHECK()
                break;

            case RESOURCE_TYPE_PROJECTILE_WEAPON:
                CHECKIF(PROJECTILE_BOWS)
                CHECKELSEIF(PROJECTILE_CROSSBOWS)
                CHECKELSEIF(PROJECTILE_THROWING)
                ENDCHECK()
                break;

            case RESOURCE_TYPE_FIREARM:
                CHECKIF(FIREARM_TASERS)
                CHECKELSEIF(FIREARM_HOLDOUTS)
                CHECKELSEIF(FIREARM_LPISTOLS)
                CHECKELSEIF(FIREARM_HPISTOLS)
                CHECKELSEIF(FIREARM_MPISTOLS)
                CHECKELSEIF(FIREARM_SMGS)
                CHECKELSEIF(FIREARM_ASSAULTR)
                CHECKELSEIF(FIREARM_SNIPERR)
                CHECKELSEIF(FIREARM_SHOTGUNS)
                CHECKELSEIF(FIREARM_SPECIALW)
                CHECKELSEIF(FIREARM_MACHINEG)
                CHECKELSEIF(FIREARM_CANNONS_LAUNCHERS)
                ENDCHECK()
                break;

            case RESOURCE_TYPE_AMMUNITION:
                CHECKIF(AMMU_GRENADES)
                CHECKELSEIF(AMMU_ROCKETS)
                ENDCHECK()
                break;

            default:
                break;
            }

            // Set the category
            category = _rootItem->children[type]->children[resourceDef->subType];
        }
        // Otherwise, the resource sits in a top category
        else
        {
            category = _rootItem->children[type];
        }

        // ---------------------------------------------------------------
        // Parse the information common to all types of resources
        // ---------------------------------------------------------------
        // Translations
        tempObject = currentResource["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            resourceDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Max rating
        if (currentResource.contains("max_rating"))
        {
            resourceDef->maxRating = currentResource["max_rating"].toString().toInt();
        }

        // Stacks or not
        if (currentResource.contains("stacks"))
        {
            resourceDef->stacks = currentResource["stacks"].toString() == "true";
        }

        // Availabilities can have different keys
        fillAvailability(resourceDef, &currentResource);

        // Cost can have different keys
        fillCost(resourceDef, &currentResource);

        // Wireless
        if (currentResource.contains("wireless"))
        {
            resourceDef->wireless = currentResource["wireless"].toString() == "true";
        }

        // Possible mount locations
        tempArray = currentResource["mounts"].toArray();
        for (int j = 0; j < tempArray.size(); ++j)
        {
            resourceDef->mounts.push_back(tempArray[j].toString());
        }

        // Premounted mounts
        tempArray = currentResource["premounted"].toArray();
        MountInfo info;
        for (int j = 0; j < tempArray.size(); ++j)
        {
            tempArray2 = tempArray[j].toArray();
            info.id = tempArray2[0].toString();
            if (!_definitions.contains(info.id))
            {
                qCritical() << "Resource \"" << uniqueId << "\" has unknown mount \""
                            << info.id << "\". Resource parsing aborted.";
                return;
            }
            info.location = tempArray2[1].toString();
            info.removable = tempArray2[2].toString() == true;
            if (tempArray2.size() > 3)
            {
                info.rating = tempArray2[3].toString().toInt();
            }
            resourceDef->attachedMounts.push_back(info);
        }

        // ---------------------------------------------------------------
        // Parse the information specific to certain types
        // ---------------------------------------------------------------
        switch(type)
        {
        case RESOURCE_TYPE_MELEE_WEAPON:
        case RESOURCE_TYPE_PROJECTILE_WEAPON:
        case RESOURCE_TYPE_FIREARM:
        case RESOURCE_TYPE_AMMUNITION:
            fillWeaponDefinition(weaponDef, &currentResource);
            break;

        default:
            break;
        }

        // Add to category
        category->children.push_back(resourceDef);

        // Make sure the definition doesn't already exist
        if (_definitions.contains(uniqueId))
        {
            qCritical() << "Resource \"" << uniqueId << "\" already exists. Resource parsing aborted.";
            return;
        }
        _definitions[currentResource["unique_id"].toString()] = resourceDef;
    }
}


//---------------------------------------------------------------------------------
void
ResourceRules::fillAvailability(ResourceDefinition* p_resourceDef, QJsonObject* p_currentResource)
{
    QJsonObject currentResource = *p_currentResource;

    // Get the correct availability string
    QString tempString;
    if (currentResource.contains("availability"))
    {
        tempString = currentResource["availability"].toString();
    }
    else if (currentResource.contains("availability_per_rating"))
    {
        tempString = currentResource["availability_per_rating"].toString();
        p_resourceDef->availabilityIsPerRating = true;
    }
    else
    {
        qCritical() << "Resource \"" << p_resourceDef->id << "\" has no availability!";
        return;
    }

    // Availability class
    bool hasAvClass = false;
    if (tempString.endsWith("R"))
    {
        hasAvClass = true;
        p_resourceDef->availabilityClass = AVAILABILITY_CLASS_RESTRICTED;
    }
    else if (tempString.endsWith("F"))
    {
        hasAvClass = true;
        p_resourceDef->availabilityClass = AVAILABILITY_CLASS_FORBIDDEN;
    }
    else
    {
        p_resourceDef->availabilityClass = AVAILABILITY_CLASS_NORMAL;
    }

    // Availability
    if (hasAvClass)
    {
        tempString = tempString.mid(0, tempString.length() - 1);
    }
    p_resourceDef->availabilityNum = tempString;
}

//---------------------------------------------------------------------------------
void
ResourceRules::fillCost(ResourceDefinition* p_resourceDef, QJsonObject* p_currentResource)
{
    QJsonObject currentResource = *p_currentResource;

    // Get the correct cost
    if (currentResource.contains("cost"))
    {
        p_resourceDef->cost = currentResource["cost"].toString();
    }
    else if (currentResource.contains("cost_per_rating"))
    {
        p_resourceDef->cost = currentResource["cost_per_rating"].toString();
        p_resourceDef->costIsPerRating = true;
    }
    else
    {
        qCritical() << "Resource \"" << p_resourceDef->id << "\" has no cost!";
        return;
    }
}

//---------------------------------------------------------------------------------
void
ResourceRules::fillWeaponDefinition(WeaponDefinition* p_weaponDef, QJsonObject* p_currentResource)
{
    QJsonObject currentResource = *p_currentResource;

    // TODO: here
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
int
ResourceRules::getNuyenForPriority(int p_prioIndex) const
{
    if (p_prioIndex < 0 || p_prioIndex > 4)
    {
        qCritical() << QString("Invalid priority passed: %1").arg(p_prioIndex);
        return -10000000;
    }

    return _nuyenPerPrio[p_prioIndex];
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
