#ifndef CUSTOMCHOICE_H
#define CUSTOMCHOICE_H

#include <QStringList>

class CustomDescriptorPopup;
class QJsonObject;
class SkillDefinition;


/**
 * @brief The fifferent types of custom choices.
 */
enum ChoiceType
{
    CHOICETYPE_INVALID = -1,
    CHOICETYPE_ATTRIBUTE_SELECTION,
    CHOICETYPE_SKILL_SELECTION,
    CHOICETYPE_POTENTIAL_SELECTION,
    CHOICETYPE_MATRIX_ATTRIBUTE_SELECTION,
    CHOICETYPE_CUSTOM_SELECTION,
    NUM_CHOICETYPES
};

/**
 * @brief This class reads a custom choice from JSON and can fill
 *         a CustomDescriptorPopup with its choices.
 */
class CustomChoice
{
public:
    /**
     * @brief Constructor.
     * @param p_jsonObject The JSON object that contains this choice.
     */
    CustomChoice(QJsonObject* p_jsonObject);

    /**
     * @brief Returns the type of this custom choice.
     */
    ChoiceType getType() const;

    /**
     * @brief Will fill the passed CustomDescriptorPopup with its values.
     */
    void fillDescriptorPopup(CustomDescriptorPopup* p_popup) const;

private:

    ChoiceType      _type;
    bool            _groupsAllowed;
    bool            _groupsOnly;
    int             _minRating;
    QStringList     _choiceStrings;
    QStringList     _choiceValues;

    /**
     * @brief Will throw out each skill in the vector that does not at least have the minimum
     *          skill rating.
     * @param p_rating  The minimal rating the skill must have.
     * @param p_skills  The vector of skills to filter.
     */
    void filterByMinRating(int p_rating, std::vector<std::pair<QString,SkillDefinition*> >& p_skills);
};

#endif // CUSTOMCHOICE_H
