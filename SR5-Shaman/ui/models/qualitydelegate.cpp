#include "qualitydelegate.h"
#include <QPainter>

#include "rules/qualityrules.h"

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
    QString text = "a";

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(p_option.rect);

    drawBackground(p_painter, p_option, p_index);
    drawDisplay(p_painter, p_option, p_option.rect, text);
    drawFocus(p_painter, p_option, p_option.rect);

    p_painter->restore();
}
