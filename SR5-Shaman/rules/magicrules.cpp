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
    // TODO: here
    return;

    QJsonArray magicTypesArray = doc.object().value("magic_types").toArray();
    QJsonObject currentType;
    MagicTypeDefinition* typeDef = 0;
    QJsonArray tempArray;
    QJsonObject tempObject;
    for (int i = 0; i < magicTypesArray.size(); ++i)
    {
        /*currentType = metatypesArray.at(i).toObject();

        // Add type definition
        typeDef = new MagicTypeDefinition();

        // Translations
        tempObject = currentType["translations"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            typeDef->translations[tempObject.keys().at(j)] = tempObject[tempObject.keys().at(j)].toString();
        }

        // Special Attribute Points
        tempArray = currentType["special_attrib_points"].toArray();
        for (int j = 0; j < 5; ++j)
        {
            typeDef->specialAttribPointsPerPrio.push_back(tempArray.at(j).toVariant().toInt());
        }

        // Attributes
        tempObject = currentType["attribute_values"].toObject();
        for (int j = 0; j < tempObject.keys().size(); ++j)
        {
            typeDef->attributesMin[tempObject.keys().at(j)] =
                    tempObject[tempObject.keys().at(j)].toArray().at(0).toVariant().toInt();
            typeDef->attributesMax[tempObject.keys().at(j)] =
                    tempObject[tempObject.keys().at(j)].toArray().at(1).toVariant().toInt();
        }

        // Starting essence
        typeDef->startingEssence = currentType.value("starting_essence").toString().toFloat();

        // Sprint increase
        typeDef->sprintIncrease = currentType.value("sprint_increase").toString().toInt();*/

        _definitions[currentType["unique_id"].toString()] = typeDef;
    }
}
