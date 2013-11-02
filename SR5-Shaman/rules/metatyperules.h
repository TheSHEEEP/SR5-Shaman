#ifndef METATYPERULES_H
#define METATYPERULES_H

#include <QString>
#include <QMap>
#include <vector>

/**
 * @brief The definition of one metatype.
 */
struct MetatypeDefinition
{
    QMap<QString, QString>  translations;
    std::vector<int>        specialAttribPointsPerPrio;
};

/**
 * @brief This class holds all information regarding metatype specific rules.
 *          For example, this class holds all metatype definitions.
 */
class MetatypeRules
{
public:
    /**
     * @brief Constructor.
     */
    MetatypeRules();

    /**
     * @brief Destructor.
     */
    ~MetatypeRules();

    /**
     * @brief Initializes the metatype rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the map of metatype definitions.
     */
    const QMap<QString, MetatypeDefinition*>& getAllDefinitions() const;

    /**
     * @brief Returns the definition of the metatype with the passed ID.
     * @note    Does NOT check if the id exists. Use getAllDefinitions() for that.
     */
    const MetatypeDefinition& getDefinition(const QString& p_uniqueId) const;

private:
    QMap<QString, MetatypeDefinition*>  _definitions;
};

//---------------------------------------------------------------------------------
inline
const QMap<QString, MetatypeDefinition*>&
MetatypeRules::getAllDefinitions() const
{
    return _definitions;
}

//---------------------------------------------------------------------------------
inline
const MetatypeDefinition&
MetatypeRules::getDefinition(const QString& p_uniqueId) const
{
    return *(_definitions[p_uniqueId]);
}


#endif // METATYPERULES_H
