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
    else if (type == "potential_selection")
    {
        _type = CHOICETYPE_POTENTIAL_SELECTION;
    }
    else if (type == "matrix_attribute_selection")
    {
        _type = CHOICETYPE_MATRIX_ATTRIBUTE_SELECTION;
    }
    else if (type == "custom_selection")
    {
        _type = CHOICETYPE_CUSTOM_SELECTION;
    }

    // Attribute selection
    if (_type == CHOICETYPE_ATTRIBUTE_SELECTION)
    {
        QJsonArray attributes = jsonObject["attributes"].toArray();

        for (int i = 0; i < attributes.size(); ++i)
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
        std::vector<std::pair<QString,SkillDefinition*> > skills;
        if (jsonObject.contains("categories"))
        {
            QJsonArray categoriesArray = jsonObject["categories"].toArray();
            skills.reserve(100);
            std::vector<std::pair<QString,SkillDefinition*> > tempSkills;
            for (int i = 0; i < categoriesArray.size(); ++i)
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
        }
        else if (jsonObject.contains("skills_containing"))
        {
            QJsonArray skillsArray = jsonObject["categories"].toArray();
            skills.reserve(10);
            for (int i = 0; i < skillsArray.size(); ++i)
            {
                QString skill = skillsArray[i].toString();
                std::vector<std::pair<QString,SkillDefinition*> > tempSkills =
                        SKILL_RULES->getDefinitionsContaining(skill, false);
                skills.insert(skills.end(), tempSkills.begin(), tempSkills.end());
                if (_groupsAllowed)
                {
                    tempSkills = SKILL_RULES->getDefinitionsContaining(skill, true);
                    skills.insert(skills.end(), tempSkills.begin(), tempSkills.end());
                }
            }
        }

        // Add the skills
        // TODO: Having each single skill in a ComboBox is a bit ugly for large numbers of skills.
        //       Maybe we could add a Skill Tree View to the CustomDescriptorPopup
        for (unsigned int i = 0; i < skills.size(); ++i)
        {
            _choiceStrings.push_back(skills[i].second->translations[APPSTATUS->getCurrentLocale()]);
            _choiceValues.push_back(skills[i].first);
        }
    } // END Skill selection

    // Potential selection
    if (_type == CHOICETYPE_POTENTIAL_SELECTION)
    {
        _choiceStrings.push_back(Dictionary::getTranslation("Physical Limit"));
        _choiceValues.push_back("physical");
        _choiceStrings.push_back(Dictionary::getTranslation("Social Limit"));
        _choiceValues.push_back("social");
        _choiceStrings.push_back(Dictionary::getTranslation("Mental Limit"));
        _choiceValues.push_back("mental");
    }

    // Matrix attribute selection
    if (_type == CHOICETYPE_MATRIX_ATTRIBUTE_SELECTION)
    {
        _choiceStrings.push_back(Dictionary::getTranslation("Attack"));
        _choiceValues.push_back("attack");
        _choiceStrings.push_back(Dictionary::getTranslation("Sleaze"));
        _choiceValues.push_back("sleaze");
        _choiceStrings.push_back(Dictionary::getTranslation("Data Processing"));
        _choiceValues.push_back("data processing");
        _choiceStrings.push_back(Dictionary::getTranslation("Firewall"));
        _choiceValues.push_back("firewall");
    }

    // Custom selection
    if (_type == CHOICETYPE_CUSTOM_SELECTION)
    {
        QJsonArray array = jsonObject["strings"].toArray();
        for (int i = 0; i < array.size(); ++i)
        {
            _choiceStrings.push_back(Dictionary::getTranslation(array[i].toString()));
            _choiceValues.push_back(array[i].toString());
        }
    }
}

//---------------------------------------------------------------------------------
void
CustomChoice::fillDescriptorPopup(CustomDescriptorPopup* p_popup)
{
    p_popup->setChoices(_choiceStrings, _choiceValues);
}

