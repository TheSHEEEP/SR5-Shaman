#ifndef ATTRIBUTERULES_H
#define ATTRIBUTERULES_H

#include <QString>
#include <vector>

/**
 * @brief Contains all rules concerining direct and derived attributes.
 */
class AttributeRules
{
public:
    /**
     * @brief Constructor.
     */
    AttributeRules();

    /**
     * @brief Destructor.
     */
    ~AttributeRules();

    /**
     * @brief Initializes the metatype rules by reading the passed JSON file.
     * @param p_jsonFile    The JSON file to parse.
     */
    void initialize(const QString& p_jsonFile);

    /**
     * @brief Returns the number of free attribute points for the passed priority index.
     * @param p_prioIndex   The priority index to look for. 0-4 for A-E.
     * @return The number of free attribute points.
     */
    int getNumFreebies(int p_prioIndex) const;

    /**
     * @brief Calculates and returns the maximum number of purchasable attribute increases.
     * @param p_currentValue    The current attribute value.
     * @param p_maxValue        The maximum attribute value (natural maximum).
     * @param p_availableKarma  The karma points available to spend.
     * @return The maximum number of purchasable attribute increases.
     */
    int calculateMaximumAttributeIncrease(int p_currentValue, int p_maxValue, int p_availableKarma) const;

    /**
     * @brief Returns the cost of the increase from the old to the new attribute value.
     * @param p_oldValue    The old attribute value.
     * @param p_newValue    the new attribute value.
     * @return The cost of the increase/ gain from the decrease (negative).
     */
    int calculateAttributeIncreaseCost(int p_oldValue, int p_newValue) const;

    /**
     * @brief Calculates and returns the physical limit for the given attributes.
     */
    int calculatePhysicalLimit(int p_strength, int p_body, int p_reaction) const;

    /**
     * @brief Calculates and returns the mental limit for the given attributes.
     */
    int calculateMentalLimit(int p_logic, int p_intuition, int p_willpower) const;

    /**
     * @brief Calculates and returns the social limit for the given attributes.
     */
    int calculateSocialLimit(int p_charisma, int p_willpower, int p_essence) const;

private:
    std::vector<int>    _freebiesPerPrio;
};


//---------------------------------------------------------------------------------
inline
int
AttributeRules::getNumFreebies(int p_prioIndex) const
{
    return _freebiesPerPrio[p_prioIndex];
}
#endif // ATTRIBUTERULES_H
