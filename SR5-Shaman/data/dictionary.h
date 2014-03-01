#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QMap>

/**
 * @brief This class holds the translations of all string that are not handled via the Qt tr() method.
 */
class Dictionary
{
public:
    /**
     * @brief Constructor.
     */
    Dictionary();

    /**
     * @brief Initializes the dictionary, reading all translations.
     * @param p_jsonFile    The path to the JSON file to read from.
     */
    static void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the translation for the passed key for the currently set language.
     *          If the key was not found, the key itself is returned.
     * @param p_key The key to look for.
     */
    static QString getTranslation(const QString& p_key);

private:
    static QMap<QString,QMap<QString,QString> >   _translations;   // <key, <language_short, translation> >
};

#endif // DICTIONARY_H
