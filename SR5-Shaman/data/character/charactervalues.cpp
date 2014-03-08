#include "charactervalues.h"
#include <QDebug>

#include "rules/rules.h"
#include "data/character/characterchoices.h"
#include "data/character/effectregistry.h"

CharacterValues* CharacterValues::_instance = 0;

//---------------------------------------------------------------------------------
CharacterValues::CharacterValues()
    : _karmaPool(0)
{
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
    if (!ATTRIBUTE_RULES->isValidAttribute(p_attribute))
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
        attribValue += MAGIC_RULES->getMagicTypeDefinition(CHARACTER_CHOICES->getMagicUserTypeID())
                            .priorities[CHARACTER_CHOICES->getPriorityIndex(PRIORITY_MAGIC)]->startingMagic;
    }

    // Add freebie & karma influences
    attribValue += CHARACTER_CHOICES->getAttributeIncreases(p_attribute);

    // Add effects from augmentations, adept powers, qualities, etc.
    if (p_withAugmentations || p_withOther)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(EFFECTTYPE_INCREASE_ATTRIBUTE);
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            Effect* effect = effects[i];
            if (effect->getCurrentTarget() == p_attribute)
            {
                // From others
                if (p_withOther && (effect->getSource().magicAbility))
                {
                    attribValue += effect->getValue().toInt();
                }
                // From cyberware
                // TODO: Mind cyberware
                else
                {

                }
            }
        }
    }

    return attribValue;
}

//---------------------------------------------------------------------------------
float
CharacterValues::getEssence(bool p_withAugments) const
{
    // Sanity check - metatype
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

//---------------------------------------------------------------------------------
int
CharacterValues::getAdeptPowerLevel(const QString& p_id) const
{
    // Sanity check - magic user
    if (!CHARACTER_CHOICES->getIsMagicUser())
    {
        qWarning() << QString("Character is not even a magic user! Asked for adept power level: %1").arg(p_id);
        return 0;
    }

    // Get the spent freebies
    float freebies = CHARACTER_CHOICES->getSpellFreebies(p_id);

    // Calculate the level
    int level = 0;
    const AdeptPowerDefinition& def = MAGIC_RULES->getAdeptPowerDefinition(p_id);
    switch (def.costType)
    {
    case COSTTYPE_ARRAY:
    {
        for (unsigned int i = 0; i < def.costArray.size(); ++i)
        {
            if (freebies == def.costArray[i])
            {
                level = i + 1;
                break;
            }
        }
        break;
    }

    case COSTTYPE_PER_LEVEL:
        level = freebies / def.costArray.back();
        break;
    default:
        level = 0;
    }

    return level;
}

//---------------------------------------------------------------------------------
int
CharacterValues::getSkill(const QString& p_skill, bool p_withAugmentations, bool p_withOther) const
{
    // Sanity check - does the skill even exist?
    if (!SKILL_RULES->getIsValidSkill(p_skill))
    {
        qWarning() << QString("There is no skill with ID %1").arg(p_skill);
        return 0;
    }

    int skillValue = 0;

    // Skill increases (via points or karma)
    skillValue += CHARACTER_CHOICES->getSkillIncreases(p_skill);

    // Add effects from augmentations, adept powers, qualities, etc.
    if (p_withAugmentations || p_withOther)
    {
        std::vector<Effect*> effects = EFFECT_REGISTRY->getEffectsByType(EFFECTTYPE_INCREASE_SKILL);
        for (unsigned int i = 0; i < effects.size(); ++i)
        {
            Effect* effect = effects[i];
            // From others
            if (p_withOther && (effect->getSource().magicAbility))
            {
                skillValue += effect->getValue().toInt();
            }
            // From cyberware
            // TODO: Mind cyberware
            else
            {

            }
        }
    }

    return skillValue;
}
