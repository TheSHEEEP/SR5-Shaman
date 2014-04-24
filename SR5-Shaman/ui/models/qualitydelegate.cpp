#include "qualitydelegate.h"
#include <QPainter>

#include "rules/qualityrules.h"
#include "data/dictionary.h"
#include "data/appstatus.h"
#include "data/character/charactervalues.h"

//---------------------------------------------------------------------------------
QualityDelegate::QualityDelegate()
    : _showQualityLevel(false)
{

}

//---------------------------------------------------------------------------------
QualityDelegate::~QualityDelegate()
{

}

//---------------------------------------------------------------------------------
void
QualityDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get the item
    QualityDefinition* item = static_cast<QualityDefinition*>(p_index.data().value<void*>());

    // Get the name
    QString text = "";
    if (item->isCategory)
    {
        if (item->id.contains("CATEGORY_POSITIVE"))
        {
            text = Dictionary::getTranslation("CATEGORY_POSITIVE");
        }
        else
        {
            text = Dictionary::getTranslation("CATEGORY_NEGATIVE");
        }
    }
    else
    {
        text = item->translations[APPSTATUS->getCurrentLocale()];
    }

    // If the quality has levels, show the levels
    if (_showQualityLevel &&
        (item->costType == COSTTYPE_ARRAY || item->costType == COSTTYPE_PER_LEVEL))
    {
        int level = CHARACTER_VALUES->getQualityLevel(item->id);
        text.append(QString(" (%1)").arg(level));
    }

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(p_option.rect);

    drawBackground(p_painter, p_option, p_index);
    drawDisplay(p_painter, p_option, p_option.rect, text);
    drawFocus(p_painter, p_option, p_option.rect);

    p_painter->restore();
}
