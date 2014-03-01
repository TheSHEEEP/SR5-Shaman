#include "dictionary.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
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
