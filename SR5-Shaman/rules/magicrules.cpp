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
#include "rules/misc/customchoice.h"

//---------------------------------------------------------------------------------
MagicAbilityDefinition::MagicAbilityDefinition(MagicAbilityDefinition* p_parent)
    : parent(p_parent)
    , id("")
    , isCategory(false)
    , requiresCustom(false), customString(""), customChoices(NULL)
    , isUserDefined(false)
    , abilityType(MAGICABILITYTYPE_INVALID)
    , spell(NULL), adeptPower(NULL), complexForm(NULL)
{
    children.clear();
}

//---------------------------------------------------------------------------------
MagicAbilityDefinition::MagicAbilityDefinition(const MagicAbilityDefinition& p_other)
{
    parent = p_other.parent;
    id = p_other.id;
    isCategory = p_other.isCategory;
    requiresCustom = p_other.requiresCustom;
    customString = p_other.customString;
    customChoices = p_other.customChoices;
    isUserDefined = p_other.isUserDefined;
    abilityType = p_other.abilityType;
    spell = p_other.spell;
    adeptPower = p_other.adeptPower;
    complexForm = p_other.complexForm;
    children = p_other.children;
}

//---------------------------------------------------------------------------------
MagicAbilityDefinition::~MagicAbilityDefinition()
{
}

//---------------------------------------------------------------------------------
bool
MagicAbilityDefinition::hasChild(const QString& p_id) const
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
MagicAbilityDefinition*
MagicAbilityDefinition::getChild(const QString& p_id) const
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
MagicRules::MagicRules()
{
    // Construct root item
    _rootItem = new MagicAbilityDefinition();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // Spells
    MagicAbilityDefinition* category = new MagicAbilityDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_SPELLS";
    category->abilityType = MAGICABILITYTYPE_SPELL;
    _rootItem->children.push_back(category);
    // Adept powers
    category = new MagicAbilityDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_ADEPT_POWERS";
    category->abilityType = MAGICABILITYTYPE_ADEPT_POWER;
    _rootItem->children.push_back(category);
    // Complex forms
    category = new MagicAbilityDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_COMPLEX_FORMS";
    category->abilityType = MAGICABILITYTYPE_COMPLEX_FORM;
    _rootItem->children.push_back(category);
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

    QMap<QString, SpellDefinition*>::iterator it2;
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

    QMap<QString, ComplexFormDefinition*>::iterator it5;
    for (it5 = _complexFormDefinitions.begin(); it5 != _complexFormDefinitions.end(); ++it5)
    {
        delete (*it5);
    }
    _complexFormDefinitions.clear();
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
    SpellDefinition* spellCat = NULL;
    MagicAbilityDefinition* category = NULL;
    MagicAbilityDefinition* abilityDef = NULL;
    QString tempString;
    QString uniqueId = "";
    for (int i = 0; i < categoryArray.size(); ++i)
    {
        currentCat = categoryArray.at(i).toObject();

        uniqueId = currentCat["unique_id"].toString();

        // Make sure the definition doesn't already exist
        if (_spellCategoryDefinitions.contains(uniqueId))
        {
            qCritical() << "Spell category \"" << uniqueId << "\" already exists. Parsing aborted.";
            return;
        }

        // Get correct category
        category = _rootItem->children[MAGICABILITYTYPE_SPELL];

        // Add spell definition
        abilityDef = new MagicAbilityDefinition(category);
        abilityDef->id = uniqueId;
        abilityDef->abilityType = MAGICABILITYTYPE_SPELL;
        spellCat = new SpellDefinition();
        abilityDef->spell = spellCat;
        abilityDef->spell->isSpellCategory = true;
        category->children.push_back(abilityDef);

        // Translations
        tempObject = currentCat["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            abilityDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        _definitions[uniqueId] = abilityDef;
        _spellCategoryDefinitions[uniqueId] = spellCat;
    }

    // Parse each spell and add to the rules
    QJsonArray spellsArray = doc.object().value("spells").toArray();
    QJsonObject currentSpell;
    SpellDefinition* spellDef = 0;
    for (int i = 0; i < spellsArray.size(); ++i)
    {
        currentSpell = spellsArray.at(i).toObject();

        uniqueId = currentSpell["unique_id"].toString();

        // Make sure the definition doesn't already exist
        if (_spellDefinitions.contains(uniqueId))
        {
            qCritical() << "Spell \"" << uniqueId << "\" already exists. Parsing aborted.";
            return;
        }

        // Add spell definition
        abilityDef = new MagicAbilityDefinition();
        abilityDef->id = uniqueId;
        abilityDef->abilityType = MAGICABILITYTYPE_SPELL;
        spellDef = new SpellDefinition();
        abilityDef->spell = spellDef;

        // Translations
        tempObject = currentSpell["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            abilityDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Category
        spellDef->category = currentSpell["category"].toString();
        if (!_spellCategoryDefinitions.contains(spellDef->category))
        {
            qCritical() << "Spell \"" << currentSpell["unique_id"].toString() << "\" of non existing category \""
                           << spellDef->category << "\". Parsing aborted.";
            return;
        }

        // Get correct spell category and add to it
        category = _definitions[spellDef->category];
        abilityDef->parent = category;
        category->children.push_back(abilityDef);

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
            QVariantList list = currentSpell["descriptors"].toArray().toVariantList();
            QListIterator<QVariant> i(list);
            while (i.hasNext())
            {
                spellDef->descriptors << i.next().toString();
            }
        }

        // Does this spell require a custom choice?
        if (currentSpell.contains("requires_custom"))
        {
            abilityDef->requiresCustom = currentSpell["requires_custom"].toString() == "true";
        }

        // Is this spell affected by the essence value?
        if (currentSpell.contains("essence_effect"))
        {
            spellDef->essenceEffect = currentSpell["essence_effect"].toString() == "true";
        }

        _definitions[uniqueId] = abilityDef;
        _spellDefinitions[uniqueId] = spellDef;
    }// END spells

    // Parse each adept power and add to the rules
    QJsonArray powersArray = doc.object().value("adept_powers").toArray();
    QJsonObject currentPower;
    AdeptPowerDefinition* powerDef = 0;
    for (int i = 0; i < powersArray.size(); ++i)
    {
        currentPower = powersArray.at(i).toObject();

        uniqueId = currentPower["unique_id"].toString();

        // Make sure the definition doesn't already exist
        if (_adeptPowerDefinitions.contains(uniqueId))
        {
            qCritical() << "Adept Power \"" << uniqueId << "\" already exists. Parsing aborted.";
            return;
        }

        // Add type definition
        abilityDef = new MagicAbilityDefinition();
        abilityDef->id = uniqueId;
        abilityDef->abilityType = MAGICABILITYTYPE_ADEPT_POWER;
        powerDef = new AdeptPowerDefinition();
        abilityDef->adeptPower = powerDef;

        // Get correct spell category and add to it
        category = _rootItem->children[MAGICABILITYTYPE_ADEPT_POWER];
        abilityDef->parent = category;
        category->children.push_back(abilityDef);

        // Translations
        tempObject = currentPower["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            abilityDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Cost
        if (currentPower.contains("cost_per_level"))
        {
            powerDef->costType = COSTTYPE_PER_LEVEL;
            powerDef->costArray.push_back(currentPower["cost_per_level"].toString().toDouble());
        }
        else if (currentPower.contains("cost"))
        {
            QJsonValue::Type type = currentPower["cost"].type();

            // Normal cost or array
            if (type != QJsonValue::Array)
            {
                powerDef->costType = COSTTYPE_NORMAL;
                powerDef->costArray.push_back(currentPower["cost"].toString().toDouble());
            }
            else
            {
                powerDef->costType = COSTTYPE_ARRAY;
                tempArray = currentPower["cost"].toArray();

                // Add each array entry
                for (int j = 0; j < tempArray.size(); ++j)
                {
                    powerDef->costArray.push_back(tempArray[j].toString().toDouble());
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
            abilityDef->requiresCustom = currentPower["requires_custom"].toString() == "true";
            if (currentPower.contains("custom_choices"))
            {
                QJsonObject obj = currentPower["custom_choices"].toObject();
                abilityDef->customChoices = new CustomChoice(&obj);
            }
        }

        // Does this power have effects?
        // TODO: here

        _definitions[uniqueId] = abilityDef;
        _adeptPowerDefinitions[uniqueId] = powerDef;
    } // END adept powers

    // Parse each complex form and add to the rules
    QJsonArray formsArray = doc.object().value("complex_forms").toArray();
    QJsonObject currentForm;
    ComplexFormDefinition* formDef = 0;
    for (int i = 0; i < formsArray.size(); ++i)
    {
        currentForm = formsArray.at(i).toObject();

        uniqueId = currentForm["unique_id"].toString();

        // Make sure the definition doesn't already exist
        if (_complexFormDefinitions.contains(uniqueId))
        {
            qCritical() << "Complex Form \"" << uniqueId << "\" already exists. Parsing aborted.";
            return;
        }

        // Add form definition
        abilityDef = new MagicAbilityDefinition();
        abilityDef->id = uniqueId;
        abilityDef->abilityType = MAGICABILITYTYPE_COMPLEX_FORM;
        formDef = new ComplexFormDefinition();
        abilityDef->complexForm = formDef;

        // Get correct spell category and add to it
        category = _rootItem->children[MAGICABILITYTYPE_COMPLEX_FORM];
        abilityDef->parent = category;
        category->children.push_back(abilityDef);

        // Translations
        tempObject = currentForm["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            abilityDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Target
        tempString = currentForm["target"].toString();
        if (tempString == "persona")
        {
            formDef->targetType = TARGETTYPE_PERSONA;
        }
        else if (tempString == "device")
        {
            formDef->targetType = TARGETTYPE_DEVICE;
        }
        else if (tempString == "file")
        {
            formDef->targetType = TARGETTYPE_FILE;
        }
        else if (tempString == "sprite")
        {
            formDef->targetType = TARGETTYPE_SPRITE;
        }

        // Duration
        tempString = currentForm["duration"].toString();
        if (tempString == "I")
        {
            formDef->duration = SPELLDURATION_INSTANT;
        }
        else if (tempString == "S")
        {
            formDef->duration = SPELLDURATION_SUSTAINED;
        }
        else if (tempString == "P")
        {
            formDef->duration = SPELLDURATION_PERMANENT;
        }

        // Fading Value
        formDef->fadingValue = currentForm["fading_value"].toString();

        // Does this form require a custom choice?
        if (currentForm.contains("requires_custom"))
        {
            abilityDef->requiresCustom = currentForm["requires_custom"].toString() == "true";
        }

        _definitions[uniqueId] = abilityDef;
        _complexFormDefinitions[uniqueId] = formDef;
    } // END complex forms
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
    QMap<QString, SpellDefinition*>::const_iterator it;
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
    if (!_definitions.contains(p_uniqueID))
    {
        return QObject::tr("Category not found: %1").arg(p_uniqueID);
    }

    return _definitions[p_uniqueID]->translations[APPSTATUS->getCurrentLocale()];
}

//---------------------------------------------------------------------------------
QString
MagicRules::constructCustomizedSpell(const QString &p_id, const QString &p_customValue, const QString &p_translation)
{
    // Construct new ID
    const MagicAbilityDefinition& originalAbility = getDefinition(p_id);
    QString newID = p_id + "_" + p_customValue;

    // If a spell with that ID already exists, don't add it again
    if (_definitions.find(newID) != _definitions.end())
    {
        return newID;
    }

    // Create the new spell
    MagicAbilityDefinition* newAbility = new MagicAbilityDefinition();
    newAbility->id = newID;
    newAbility->customString = p_customValue;
    newAbility->requiresCustom = false;
    newAbility->customChoices = originalAbility.customChoices;
    newAbility->isUserDefined = true;
    newAbility->parent = originalAbility.parent;
    newAbility->children = originalAbility.children;
    newAbility->isCategory = originalAbility.isCategory;
    newAbility->parent->children.push_back(newAbility);
    newAbility->translations = originalAbility.translations;
    QMap<QString, QString>::iterator it;
    for (it = newAbility->translations.begin(); it != newAbility->translations.end(); ++it)
    {
        it.value().append(" (" + p_translation + ")");
    }
    newAbility->abilityType = originalAbility.abilityType;
    switch (newAbility->abilityType)
    {
        case MAGICABILITYTYPE_SPELL:
            newAbility->spell = originalAbility.spell;
            _spellDefinitions[newID] = newAbility->spell;
        break;

        case MAGICABILITYTYPE_ADEPT_POWER:
            newAbility->adeptPower = originalAbility.adeptPower;
            _adeptPowerDefinitions[newID] = newAbility->adeptPower;
        break;

        case MAGICABILITYTYPE_COMPLEX_FORM:
            newAbility->complexForm = originalAbility.complexForm;
            _complexFormDefinitions[newID] = newAbility->complexForm;
        break;

        default:
        break;
    }

    // Add the skill
    _definitions[newID] = newAbility;
    return newID;
}
