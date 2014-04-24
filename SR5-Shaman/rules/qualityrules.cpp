#include "qualityrules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>
#include <QJsonParseError>
#include <QDebug>
#include <QString>
#include "rules/effects/effect.h"
#include "rules/misc/customchoice.h"

//---------------------------------------------------------------------------------
QualityDefinition::QualityDefinition(QualityDefinition* p_parent)
    : parent(p_parent)
    , id("")
    , isCategory(false), isPositive(false), isUserDefined(false), requiresCustom(false)
    , customString(""), customChoices(NULL)
    , base("")
{
    children.clear();
}

//---------------------------------------------------------------------------------
QualityDefinition::QualityDefinition(const QualityDefinition& p_other)
{
    parent = p_other.parent;
    id = p_other.id;
    base = p_other.base;
    isPositive = p_other.isPositive;
    isUserDefined = p_other.isUserDefined;
    isCategory = p_other.isCategory;
    requiresCustom = p_other.requiresCustom;
    customChoices = p_other.customChoices;
    children = p_other.children;
    translations = p_other.translations;
}

//---------------------------------------------------------------------------------
QualityDefinition::~QualityDefinition()
{

}

//---------------------------------------------------------------------------------
bool
QualityDefinition::hasChild(const QString& p_id) const
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
QualityDefinition*
QualityDefinition::getChild(const QString& p_id) const
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
QualityRules::QualityRules()
{
    // Construct root item definition (this is used for display in trees
    // Construct root item
    _rootItem = new QualityDefinition();
    _rootItem->id = "DEFINITION";
    _rootItem->parent = NULL;

    // Add categories
    // Positive
    QualityDefinition* category = new QualityDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_POSITIVE";
    _rootItem->children.push_back(category);
    // Negative
    category = new QualityDefinition(_rootItem);
    category->isCategory = true;
    category->id = "CATEGORY_NEGATIVE";
    _rootItem->children.push_back(category);
}

//---------------------------------------------------------------------------------
QualityRules::~QualityRules()
{
    // Clean definitions
    QMap<QString, QualityDefinition*>::iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        delete (*it);
    }
}

//---------------------------------------------------------------------------------
void
QualityRules::initialize(const QString& p_jsonFile)
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

    // Parse each quality and add to the rules
    QJsonArray qualitiesArray = doc.object().value("qualities").toArray();
    QJsonObject currentQuality;
    QualityDefinition* qualityDef = NULL;
    QJsonArray tempArray;
    QJsonObject tempObject;
    QualityDefinition* category = NULL;
    QString uniqueId = "";
    QString tempString = "";
    for (int i = 0; i < qualitiesArray.size(); ++i)
    {
        currentQuality = qualitiesArray.at(i).toObject();

        // ID
        uniqueId = currentQuality["unique_id"].toString();

        // Add type definition
        qualityDef = new QualityDefinition();
        qualityDef->id = uniqueId;

        // Translations
        tempObject = currentQuality["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            qualityDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Requires custom?
        if (currentQuality.contains("requires_custom"))
        {
            qualityDef->requiresCustom = currentQuality["requires_custom"].toString() == "true";
            if (currentQuality.contains("custom_choices"))
            {
                QJsonObject obj = currentQuality["custom_choices"].toObject();
                qualityDef->customChoices = new CustomChoice(&obj);
            }
        }

        // Positive or negative
        tempString = currentQuality["positive"].toString();
        qualityDef->isPositive = (tempString == "true");

        // Cost
        if (currentQuality.contains("cost_per_level"))
        {
            qualityDef->costType = COSTTYPE_PER_LEVEL;
            qualityDef->costArray.push_back(currentQuality["cost_per_level"].toString().toInt());
        }
        else if (currentQuality.contains("cost"))
        {
            QJsonValue::Type type = currentQuality["cost"].type();

            // Normal cost or array
            if (type != QJsonValue::Array)
            {
                qualityDef->costType = COSTTYPE_NORMAL;
                qualityDef->costArray.push_back(currentQuality["cost"].toString().toInt());
            }
            else
            {
                qualityDef->costType = COSTTYPE_ARRAY;
                tempArray = currentQuality["cost"].toArray();

                // Add each array entry
                for (int j = 0; j < tempArray.size(); ++j)
                {
                    qualityDef->costArray.push_back(tempArray[j].toString().toInt());
                }
            }
        }

        // Effects
        if (currentQuality.contains("effects"))
        {
            tempArray = currentQuality["effects"].toArray();
            for (int j = 0; j < tempArray.size(); ++j)
            {
                QJsonValueRef obj = tempArray[j];
                EffectSource source;
                source.quality = qualityDef;
                qualityDef->effects.push_back(new Effect(&obj, source));
            }
        }

        // Get the correct category
        category = _rootItem->children[qualityDef->isPositive ? 0 : 1];
        qualityDef->parent = category;

        // Add to category
        category->children.push_back(qualityDef);

        // Make sure the definition doesn't already exist
        if (_definitions.contains(uniqueId))
        {
            qCritical() << "Quality \"" << uniqueId << "\" already exists. Quality parsing aborted.";
            return;
        }
        _definitions[uniqueId] = qualityDef;
    }
}

//---------------------------------------------------------------------------------
std::vector<std::pair<QString, QualityDefinition*> >
QualityRules::getDefinitionsContaining(const QString& p_idPart) const
{
    std::vector<std::pair<QString, QualityDefinition*> > result;

    // Iterate over all definitions to find those that fit the parameters
    QMap<QString, QualityDefinition*>::const_iterator it;
    QMap<QString, QualityDefinition*>::const_iterator groupIt;
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
QString
QualityRules::constructCustomizedQuality(const QString& p_id, const QString& p_customValue,
                                         const QString& p_translation)
{
    // Construct new ID
    const QualityDefinition& originalQuality = getDefinition(p_id);
    QString newID = p_id + "_" + p_customValue;

    // If a skill with that ID already exists, don't add it again
    if (_definitions.find(newID) != _definitions.end())
    {
        return newID;
    }

    // Create the new quality
    QualityDefinition* newQuality = new QualityDefinition();
    newQuality->id = newID;
    newQuality->customString = p_customValue;
    newQuality->customChoices = originalQuality.customChoices;
    newQuality->requiresCustom = false;
    newQuality->isUserDefined = true;
    newQuality->isPositive = originalQuality.isPositive;
    newQuality->base = originalQuality.id;
    newQuality->parent = originalQuality.parent;
    newQuality->children = originalQuality.children;
    newQuality->isCategory = originalQuality.isCategory;
    newQuality->costType = originalQuality.costType;
    newQuality->costArray = originalQuality.costArray;
    originalQuality.parent->children.push_back(newQuality);
    // Translations
    newQuality->translations = originalQuality.translations;
    QMap<QString, QString>::iterator it;
    for (it = newQuality->translations.begin(); it != newQuality->translations.end(); ++it)
    {
        it.value().append(" (" + p_translation + ")");
    }
    // Effects
    for (unsigned int i = 0; i < originalQuality.effects.size(); ++i)
    {
        newQuality->effects.push_back(new Effect(*(originalQuality.effects[i])));
        EffectSource source;
        source.quality = newQuality;
        newQuality->effects.back()->setSource(source);
    }

    // Add the skill
    _definitions[newID] = newQuality;
    return newID;
}

//---------------------------------------------------------------------------------
QStringList
QualityRules::getCustomVersions(const QString& p_id) const
{
    // Get the base ID
    const QString& base = _definitions[p_id]->base != "" ? _definitions[p_id]->base : p_id;

    // Look for all custom versions
    QStringList result;
    std::vector<std::pair<QString, QualityDefinition*> > definitions = getDefinitionsContaining(base);
    for (unsigned int i = 0; i < definitions.size(); ++i)
    {
        result.push_back(definitions[i].first);
    }
    return result;
}
