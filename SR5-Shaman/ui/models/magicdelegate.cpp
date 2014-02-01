#include "magicdelegate.h"

#include <QPainter>

#include "data/appstatus.h"
#include "rules/rules.h"
#include "ui/models/magictreemodel.h"

//---------------------------------------------------------------------------------
MagicDelegate::MagicDelegate()
{

}

//---------------------------------------------------------------------------------
MagicDelegate::~MagicDelegate()
{

}


//---------------------------------------------------------------------------------
void
MagicDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get the item
    MagicAbilityDefinition* item = static_cast<MagicAbilityDefinition*>(p_index.data().value<void*>());

    // Get category name
    QString text = "";
    if (item->id.startsWith("CATEGORY"))
    {
        switch (item->abilityType)
        {
        case MAGICABILITYTYPE_INVALID:
        case NUM_MAGICABILITYTYPES:
            text = tr("Invalid Magic Type for %1").arg(item->id);
            break;

        case MAGICABILITYTYPE_SPELL:
            text = tr("Spells");
            break;

        case MAGICABILITYTYPE_ADEPT_POWER:
            text = tr("Adept Powers");
            break;

        case MAGICABILITYTYPE_COMPLEX_FORM:
            text = tr("Complex Forms");
            break;
        }
    }

    // Get translation
    if (text == "")
    {
        switch (item->abilityType)
        {
        case MAGICABILITYTYPE_INVALID:
        case NUM_MAGICABILITYTYPES:
            text = tr("Invalid Magic Type for %1").arg(item->id);
            break;

        case MAGICABILITYTYPE_SPELL:
            // The spell category has sub-categories, so if it has children, it must be such a category
            if (item->children.size() > 0)
            {
                text = MAGIC_RULES->getSpellCategoryTranslation(item->id);
            }
            else
            {
                text = item->translations[APPSTATUS->getCurrentLocale()];
            }
            break;

        case MAGICABILITYTYPE_COMPLEX_FORM:
        case MAGICABILITYTYPE_ADEPT_POWER:
            text = item->translations[APPSTATUS->getCurrentLocale()];
            break;
        }
    }

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(p_option.rect);

    drawBackground(p_painter, p_option, p_index);
    drawDisplay(p_painter, p_option, p_option.rect, text);
    drawFocus(p_painter, p_option, p_option.rect);

    p_painter->restore();
}
