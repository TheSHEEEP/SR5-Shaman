#include "magicrules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>

#include "data/appstatus.h"

//---------------------------------------------------------------------------------
MagicRules::MagicRules()
{

}

//---------------------------------------------------------------------------------
MagicRules::~MagicRules()
{
    // Clean definitions
    QMap<QString, MagicTypeDefinition*>::iterator it;
    for (it = _typeDefinitions.begin(); it != _typeDefinitions.end(); ++it)
    {
        QMap<int, MagicTypePriorityDefinition*>::iterator prioIt;
        for (prioIt = (*it)->priorities.begin(); prioIt != (*it)->priorities.end(); ++prioIt)
        {
            delete (*prioIt);
        }

        delete (*it);
    }
    _typeDefinitions.clear();

    QMap<QString, SpellCategoryDefinition*>::iterator it2;
    for (it2 = _spellCategoryDefinitions.begin(); it2 != _spellCategoryDefinitions.end(); ++it2)
    {
        delete (*it2);
    }
    _spellCategoryDefinitions.clear();

    QMap<QString, SpellDefinition*>::iterator it3;
    for (it3 = _spellDefinitions.begin(); it3 != _spellDefinitions.end(); ++it3)
    {
        delete (*it3);
    }
    _spellDefinitions.clear();

    QMap<QString, AdeptPowerDefinition*>::iterator it4;
    for (it4 = _adeptPowerDefinitions.begin(); it4 != _adeptPowerDefinitions.end(); ++it4)
    {
        delete (*it4);
    }
    _adeptPowerDefinitions.clear();
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
            if (tempObject2.contains("free_skill_groups"))
            {
                prioDef->freeSkillGroup.first = tempObject2.value("free_skill_groups").toArray().at(0).toString().toInt();
                prioDef->freeSkillGroup.second = tempObject2.value("free_skill_groups").toArray().at(1).toString().toInt();
            }

            // Free power points
            if (tempObject2.contains("free_power_points"))
            {
                prioDef->freePowerPoints = tempObject2.value("free_power_points").toString().toInt();
            }

            // Store priority
            typeDef->priorities[tempObject.keys().at(j).toInt()] = prioDef;
        }

        // Make sure the definition doesn't already exist
        if (_typeDefinitions.contains(currentType["unique_id"].toString()))
        {
            qCritical() << "Magic type \"" << currentType["unique_id"].toString() << "\" already exists. Parsing aborted.";
            return;
        }

        _typeDefinitions[currentType["unique_id"].toString()] = typeDef;
    }// END magic types

    // Parse spell category strings
    QJsonArray categoryArray = doc.object().value("spell_categories").toArray();
    QJsonObject currentCat;
    SpellCategoryDefinition* spellCal = 0;
    QString tempString;
    for (int i = 0; i < categoryArray.size(); ++i)
    {
        currentCat = categoryArray.at(i).toObject();

        // Make sure the definition doesn't already exist
        if (_spellCategoryDefinitions.contains(currentCat["unique_id"].toString()))
        {
            qCritical() << "Spell category \"" << currentCat["unique_id"].toString() << "\" already exists. Parsing aborted.";
            return;
        }

        // Add spell definition
        spellCal = new SpellCategoryDefinition();

        // Translations
        tempObject = currentSpell["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            spellCal->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        _spellCategoryDefinitions[currentCat["unique_id"].toString()] = spellCal;
    }

    // Parse each spell and add to the rules
    QJsonArray spellsArray = doc.object().value("spells").toArray();
    QJsonObject currentSpell;
    SpellDefinition* spellDef = 0;
    for (int i = 0; i < spellsArray.size(); ++i)
    {
        currentSpell = spellsArray.at(i).toObject();

        // Make sure the definition doesn't already exist
        if (_spellDefinitions.contains(currentSpell["unique_id"].toString()))
        {
            qCritical() << "Spell \"" << currentSpell["unique_id"].toString() << "\" already exists. Parsing aborted.";
            return;
        }

        // Add spell definition
        spellDef = new SpellDefinition();

        // Translations
        tempObject = currentSpell["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            spellDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Category
        spellDef->category = currentSpell["category"].toString();
        if (!_spellCategoryDefinitions.contains(spellDef->category))
        {
            qCritical() << "Spell \"" << currentSpell["unique_id"].toString() << "\" of non existing category \""
                           << spellDef->category << "\". Parsing aborted.";
            return;
        }

        // Type
        tempString = currentSpell["type"].toString();
        if (tempString == "P")
        {
            spellDef->type = SPELLTYPE_PHYSICAL;
        }
        else if (tempString == "M")
        {
            spellDef->type = SPELLTYPE_MAGICAL;
        }

        // Range
        tempString = currentSpell["range"].toString();
        if (tempString == "T")
        {
            spellDef->range = SPELLRANGE_TOUCH;
        }
        else if (tempString == "LOS")
        {
            spellDef->range = SPELLRANGE_LOS;
        }
        else if (tempString == "LOS(A)")
        {
            spellDef->range = SPELLRANGE_LOS_AREA;
        }

        // Damage (optional)
        if (currentSpell.contains("damage"))
        {
            tempString = currentSpell["damage"].toString();
            if (tempString == "P")
            {
                spellDef->damageType = SPELLDAMAGE_PHYSICAL;
            }
            else if (tempString == "M")
            {
                spellDef->damageType = SPELLDAMAGE_MENTAL;
            }
        }

        // Duration
        tempString = currentSpell["duration"].toString();
        if (tempString == "I")
        {
            spellDef->duration = SPELLDURATION_INSTANT;
        }
        else if (tempString == "S")
        {
            spellDef->duration = SPELLDURATION_SUSTAINED;
        }
        else if (tempString == "P")
        {
            spellDef->duration = SPELLDURATION_PERMANENT;
        }

        // Drain
        spellDef->drain = currentSpell["drain"].toString();

        // Descriptors (optional)
        if (currentSpell.contains("descriptors"))
        {
            spellDef->descriptors = currentSpell["descriptors"].toArray().toVariantList().to;
        }

        // Does this spell require a custom choice?
        if (currentSpell.contains("requires_custom"))
        {
            spellDef->requiresCustom = currentSpell["requires_custom"].toString() == "true";
        }

        // Is this spell affected by the essence value?
        if (currentSpell.contains("essence_effect"))
        {
            spellDef->essenceEffect = currentSpell["essence_effect"].toString() == "true";
        }

        _spellDefinitions[currentSpell["unique_id"].toString()] = spellDef;
    }// END spells

    // Parse each adept power and add to the rules
    QJsonArray powersArray = doc.object().value("adept_powers").toArray();
    QJsonObject currentPower;
    AdeptPowerDefinition* powerDef = 0;
    for (int i = 0; i < spellsArray.size(); ++i)
    {
        currentPower = powersArray.at(i).toObject();

        // Make sure the definition doesn't already exist
        if (_adeptPowerDefinitions.contains(currentPower["unique_id"].toString()))
        {
            qCritical() << "Adept Power \"" << currentSpell["unique_id"].toString() << "\" already exists. Parsing aborted.";
            return;
        }

        // Add type definition
        powerDef = new AdeptPowerDefinition();

        // Translations
        tempObject = currentPower["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            powerDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Cost
        if (currentPower.contains("cost_per_level"))
        {
            powerDef->costType = COSTTYPE_PER_LEVEL;
            powerDef->costArray.push_back(currentPower["cost_per_level"].toDouble());
        }
        else if (currentPower.contains("cost"))
        {
            // Normal cost or array
            if (currentPower["cost"].type() == QJsonValue::Double)
            {
                powerDef->costType = COSTTYPE_NORMAL;
                powerDef->costArray.push_back(currentPower["cost"].toDouble());
            }
            else
            {
                powerDef->costType = COSTTYPE_ARRAY;
                tempArray = currentPower["cost"].toArray();

                // Add each array entry
                for (int j = 0; j < tempArray.size(); ++j)
                {
                    powerDef->costArray.push_back(tempArray[i].toDouble());
                }
            }
        }

        // Activation (optional)
        if (currentPower.contains("activation"))
        {
            tempString = currentPower["activation"].toString();
            if (tempString == "interrupt")
            {
                powerDef->activationType = ACTIVATIONTYPE_INTERRUPT;
            }
            else if (tempString == "free")
            {
                powerDef->activationType = ACTIVATIONTYPE_FREE;
            }
            else if (tempString == "simple")
            {
                powerDef->activationType = ACTIVATIONTYPE_SIMPLE;
            }
        }

        // Does this power require a custom choice?
        if (currentPower.contains("requires_custom"))
        {
            powerDef->requiresCustom = currentPower["requires_custom"].toString() == "true";
        }

        _adeptPowerDefinitions[currentPower["unique_id"].toString()] = powerDef;
    } // END adept powers
}

//---------------------------------------------------------------------------------
QMap<QString, SpellDefinition*>
MagicRules::getAllSpellDefinitionsByCategory(const QString& p_uniqueID) const
{
    QMap<QString, SpellDefinition*> result;

    // Sanity check - category exists
    if (!_spellCategoryDefinitions.contains(p_uniqueID))
    {
        qWarning() << "Spell category \"" << p_uniqueID << "\" doesn't exist.";
        return result;
    }

    // Iterate over all spells to get those of the passed category
    QMap<QString, SpellDefinition*>::iterator it;
    for (it = _spellDefinitions.begin(); it != _spellDefinitions.end(); ++it)
    {
        if ((*it)->category == p_uniqueID)
        {
            result[it.key()] == *it;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------
QString
MagicRules::getSpellCategoryTranslation(const QString& p_uniqueID) const
{
    if (!_spellCategoryDefinitions.contains(p_uniqueID))
    {
        return QObject::tr("Category not found: %1").arg(p_uniqueID);
    }

    return _spellCategoryDefinitions[p_uniqueID]->translations[APPSTATUS->getCurrentLocale()];
}
