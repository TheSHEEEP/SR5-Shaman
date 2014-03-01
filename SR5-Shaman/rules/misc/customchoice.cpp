#include "customchoice.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>
#include "rules/rules.h"
#include "data/dictionary.h"
#include "data/appstatus.h"
#include "ui/character/popups/customdescriptorpopup.h"

#define ADD_SKILL_BY_TYPE(theType) \
    tempSkills = SKILL_RULES->getDefinitionsByType(theType, false); \
    skills.insert(skills.end(), tempSkills.begin(), tempSkills.end());\
    if (_groupsAllowed)\
    {\
        tempSkills = SKILL_RULES->getDefinitionsByType(theType, true);\
        skills.insert(skills.end(), tempSkills.begin(), tempSkills.end());\
    }

//---------------------------------------------------------------------------------
CustomChoice::CustomChoice(QJsonObject* p_jsonObject)
    : _type(CHOICETYPE_INVALID)
    , _groupsAllowed(false)
{
    QJsonObject jsonObject = *p_jsonObject;

    // Type
    QString type = jsonObject["type"].toString();
    if (type == "attribute_selection")
    {
        _type = CHOICETYPE_ATTRIBUTE_SELECTION;
    }
    else if (type == "skill_selection")
    {
        _type = CHOICETYPE_SKILL_SELECTION;
    }

    // Attribute selection
    if (_type == CHOICETYPE_ATTRIBUTE_SELECTION)
    {
        QJsonArray attributes = jsonObject["attributes"].toArray();

        for (unsigned int i = 0; i < attributes.size(); ++i)
        {
            QString attribute = attributes[i].toString();

            // Do not add non existing attributes
            if (!ATTRIBUTE_RULES->isValidAttribute(attribute))
            {
                qWarning() << QString("Attribute \"%1\" is not a valid base attribute.")
                                    .arg(attribute);
                continue;
            }
            _choiceStrings.push_back(Dictionary::getTranslation(attribute));
            _choiceValues.push_back(attribute);
        }
    }

    // Skill selection
    if (_type == CHOICETYPE_SKILL_SELECTION)
    {
        // Are groups allowed?
        _groupsAllowed = false;
        if (jsonObject.contains("groups_allowed"))
        {
            _groupsAllowed = jsonObject["groups_allowed"].toString() == "true";
        }

        // Get the categories
        QJsonArray categoriesArray = jsonObject["categories"].toArray();
        std::vector<std::pair<QString,SkillDefinition*> > skills;
        skills.reserve(100);
        std::vector<std::pair<QString,SkillDefinition*> > tempSkills;
        for (unsigned int i = 0; i < categoriesArray.size(); ++i)
        {
            QString category = categoriesArray[i].toString();

            if (category == "combat")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_COMBAT);
            }
            else if (category == "social")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_SOCIAL);
            }
            else if (category == "physical")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_PHYSICAL);
            }
            else if (category == "vehicle")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_VEHICLE);
            }
            else if (category == "magic")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_MAGIC);
            }
            else if (category == "resonance")
            {
                ADD_SKILL_BY_TYPE(SKILL_TYPE_RESONANCE);
            }
        }

        // Add the skills
        // TODO: Having each single skill in a ComboBox is a bit ugly.
        //       It would be much better if we could add a Skill Tree View to the CustomDescriptorPopup
        for (unsigned int i = 0; i < skills.size(); ++i)
        {
            _choiceStrings.push_back(skills[i].second->translations[APPSTATUS->getCurrentLocale()]);
            _choiceValues.push_back(skills[i].first);
        }
    }
}

//---------------------------------------------------------------------------------
void
CustomChoice::fillDescriptorPopup(CustomDescriptorPopup* p_popup)
{
    p_popup->setChoices(_choiceStrings, _choiceValues);
}

