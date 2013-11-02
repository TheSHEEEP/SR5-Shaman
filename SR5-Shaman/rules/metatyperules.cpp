#include "metatyperules.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QStringList>

#include <QDebug>

//---------------------------------------------------------------------------------
MetatypeRules::MetatypeRules()
{

}

//---------------------------------------------------------------------------------
MetatypeRules::~MetatypeRules()
{
    // Clean definitions
    QMap<QString, MetatypeDefinition*>::iterator it;
    for (it = _definitions.begin(); it != _definitions.end(); ++it)
    {
        delete (*it);
    }
}

//---------------------------------------------------------------------------------
void
MetatypeRules::initialize(const QString& p_jsonFile)
{
    // Read the JSON file
    QString val;
    QFile file;
    file.setFileName(p_jsonFile);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    // Parse each metatype and add to the rules
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonArray metatypesArray = doc.object().value("metatypes").toArray();
    QJsonObject currentType;
    MetatypeDefinition* typeDef = 0;
    QJsonArray tempArray;
    QJsonObject tempObject;
    for (int i = 0; i < metatypesArray.size(); ++i)
    {
        currentType = metatypesArray.at(i).toObject();

        // Add type definition
        typeDef = new MetatypeDefinition();

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
        _definitions[currentType["unique_id"].toString()] = typeDef;
    }
}
