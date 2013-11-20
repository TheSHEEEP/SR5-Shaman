#include "charactervalues.h"
#include <QDebug>

#include "rules/rules.h"
#include "data/character/characterchoices.h"

CharacterValues* CharacterValues::_instance = 0;

//---------------------------------------------------------------------------------
CharacterValues::CharacterValues()
    : _karmaPool(0)
{
    // Sanity checkers
    _validBaseAttributes.insert("body");
    _validBaseAttributes.insert("agility");
    _validBaseAttributes.insert("reaction");
    _validBaseAttributes.insert("strength");
    _validBaseAttributes.insert("willpower");
    _validBaseAttributes.insert("logic");
    _validBaseAttributes.insert("intuition");
    _validBaseAttributes.insert("charisma");
    _validBaseAttributes.insert("edge");
    _validBaseAttributes.insert("magic");
}

//---------------------------------------------------------------------------------
int
CharacterValues::getPhysicalLimit() const
{
    return ATTRIBUTE_RULES->calculatePhysicalLimit(getAttribute("strength"),
                                                   getAttribute("body"),
                                                   getAttribute("reaction"));
}

//---------------------------------------------------------------------------------
int
CharacterValues::getAttribute(const QString& p_attribute, bool p_withAugmentations, bool p_withOther) const
{
    // Sanity check - attribute name
    if (_validBaseAttributes.find(p_attribute) == _validBaseAttributes.end())
    {
        qWarning() << QString("Attribute \"%1\" is not a valid base attribute.")
                            .arg(p_attribute);
        return 0;
    }
    // Sanity check 2 - metatype
    if (CHARACTER_CHOICES->getMetatypeID() == "")
    {
        qWarning() << QString("Cannot calculate attribute \"%1\" without a chosen metatype.")
                            .arg(p_attribute);
        return 0;
    }

    // Get metatype minimal value
    int attribValue = METATYPE_RULES->getDefinition(CHARACTER_CHOICES->getMetatypeID()).attributesMin[p_attribute];

    // Get possible modification from magic user
    if (p_attribute == "magic" &&
        CHARACTER_CHOICES->getIsMagicUser())
    {
        attribValue += MAGIC_RULES->getDefinition(CHARACTER_CHOICES->getMagicUserType())
                            .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)]->startingMagic;
    }

    // Add freebie & karma influences
    attribValue += CHARACTER_CHOICES->getAttributeIncreases(p_attribute);

    // Add augmentation influences
    // TODO: Add augmentation influences

    // Add other influences
    // TODO: Add other influences

    return attribValue;
}

//---------------------------------------------------------------------------------
float
CharacterValues::getEssence(bool p_withAugments) const
{
    // Sanity check 2 - metatype
    if (CHARACTER_CHOICES->getMetatypeID() == "")
    {
        qWarning() << QString("Cannot calculate essence without a chosen metatype.");
        return 0.0f;
    }

    float result = 0.0f;

    // Get starting value
    result += METATYPE_RULES->getDefinition(CHARACTER_CHOICES->getMetatypeID()).startingEssence;

    // TODO: get augmentation influence

    return result;
}
