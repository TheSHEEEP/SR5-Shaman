#include "dictionary.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include <QStringList>
#include "data/appstatus.h"

QMap<QString,QMap<QString,QString> >   Dictionary::_translations;

//---------------------------------------------------------------------------------
Dictionary::Dictionary()
{
}

//---------------------------------------------------------------------------------
void
Dictionary::initialize(const QString& p_jsonFile)
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

    // Parse each key and add the translations
    QJsonArray dictionary = doc.object().value("dictionary").toArray();
    QJsonObject currentTranslation;
    QJsonObject translations;
    QString key;
    for (int i = 0; i < dictionary.size(); ++i)
    {
        currentTranslation = dictionary.at(i).toObject();
        key = currentTranslation["key"].toString();

        // Make sure the translation doesn't exist already
        if (_translations.contains(key))
        {
            qCritical() << "Translation for \"" << key << "\" already exists. Parsing aborted.";
            return;
        }

        // Create entry
        _translations[key] = QMap<QString, QString>();

        // Get translations and iterate over them
        translations = currentTranslation["translations"].toObject();
        for (int j = 0; j < translations.keys().size(); ++j)
        {
            _translations[key][translations.keys().at(j)] = translations[translations.keys().at(j)].toString();
        }
    }
}

//---------------------------------------------------------------------------------
QString
Dictionary::getTranslation(const QString& p_key)
{
    if (_translations.find(p_key) != _translations.end() &&
            _translations[p_key].find(APPSTATUS->getCurrentLocale()) != _translations[p_key].end())
    {
        return _translations[p_key][APPSTATUS->getCurrentLocale()];
    }
    return p_key;
}
