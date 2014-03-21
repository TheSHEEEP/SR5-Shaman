#ifndef CUSTOMCHOICE_H
#define CUSTOMCHOICE_H

#include <QStringList>

class CustomDescriptorPopup;
class QJsonObject;


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
    void fillDescriptorPopup(CustomDescriptorPopup* p_popup);

private:

    ChoiceType      _type;
    bool            _groupsAllowed;
    QStringList     _choiceStrings;
    QStringList     _choiceValues;
};

#endif // CUSTOMCHOICE_H
