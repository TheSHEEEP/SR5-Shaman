#ifndef ATTRIBUTERULES_H
#define ATTRIBUTERULES_H

#include <QString>
#include <QSet>
#include <vector>

/**
 * @brief Helper struct that holds values for lift and carry capacity.
 */
struct LiftCarryValues
{
public:
    int liftBase;
    int carryBase;
    int dicePool;
};
/**
 * @brief Helper struct that holds values for movement.
 */
struct MovementValues
{
public:
    int walking;
    int running;
    int sprintIncrease;
};

/**
 * @brief Contains all rules concerining direct and derived attributes.
 * @author  TheSHEEEP
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
     * @brief Returns true if the passed attribute is a vlid attribute.
     *         Example: "magic" or "strength" are valid
     *                  "magc" or "intelligence" are invalid
     */
    bool isValidAttribute(const QString& p_attribute) const;

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
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int calculatePhysicalLimit(int p_strength, int p_body, int p_reaction) const;

    /**
     * @brief Calculates and returns the mental limit for the given attributes.
     * @param p_modified    If this is true, modifications like effects will also be added to the value.
     */
    int calculateMentalLimit(int p_logic, int p_intuition, int p_willpower) const;

    /**
     * @brief Calculates and returns the social limit for the given attributes.
     */
    int calculateSocialLimit(int p_charisma, int p_willpower, int p_essence) const;

    /**
     * @brief Calculates and returns the initiative value for the given attributes.
     */
    int calculateInitiative(int p_intuition, int p_reaction) const;

    /**
     * @brief Calculates and returns the matrix initiative value for the given attributes.
     */
    int calculateMatrixInitiative(int p_intuition, int p_reaction) const;

    /**
     * @brief Calculates and returns the astral initiative value for the given attributes.
     */
    int calculateAstralInitiative(int p_intuition) const;

    /**
     * @brief Calculates and returns the composure value for the given attributes.
     */
    int calculateComposure(int p_charisma, int p_willpower) const;

    /**
     * @brief Calculates and returns the judge intentions value for the given attributes.
     */
    int calculateJudgeIntentions(int p_charisma, int p_intuition) const;

    /**
     * @brief Calculates and returns the memory value for the given attributes.
     */
    int calculateMemory(int p_logic, int p_willpower) const;

    /**
     * @brief Calculates and returns the base carry and lift values as well as
     *          the dice pool for checks value for the given attributes.
     */
    LiftCarryValues calculateLiftCarry(int p_strength, int p_body) const;

    /**
     * @brief Calculates and returns the walking and running distance as well as the
     *          sprint increase for the passed agility and metatype.
     */
    MovementValues calculateMovement(int p_agility, const QString& p_metatypeID) const;

private:
    QSet<QString>   _validBaseAttributes;

    std::vector<int>    _freebiesPerPrio;
};

#endif // ATTRIBUTERULES_H
