#include "resourcedelegate.h"

#include <QPainter>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QApplication>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "data/dictionary.h"
#include "rules/rules.h"
#include "ui/models/Resourcetreemodel.h"
#include "ui/models/Resourcesortfilterproxymodel.h"

//---------------------------------------------------------------------------------
ResourceDelegate::ResourceDelegate()
{

}

//---------------------------------------------------------------------------------
ResourceDelegate::~ResourceDelegate()
{

}

//---------------------------------------------------------------------------------
QWidget*
ResourceDelegate::createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option,
                                       const QModelIndex& p_index) const
{
    if (p_index.column() == 1)
    {
        return new QLabel("You should never see this");
    }
    else
    {
        return new QLabel("You should never see this");
    }
}

//---------------------------------------------------------------------------------
void
ResourceDelegate::setEditorData(QWidget* p_editor, const QModelIndex& p_index) const
{
    ResourceDefinition* item = static_cast<ResourceDefinition*>(p_index.data().value<void*>());
}

//---------------------------------------------------------------------------------
void
ResourceDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model,
                            const QModelIndex& p_index) const
{

}

//---------------------------------------------------------------------------------
QSize
ResourceDelegate::sizeHint(const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    QSize result = QItemDelegate::sizeHint(p_option, p_index);

    result.setHeight(result.height() * 1.5);

    return result;
}

//---------------------------------------------------------------------------------
void
ResourceDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get the item
    ResourceDefinition* item = static_cast<ResourceDefinition*>(p_index.data().value<void*>());
    QString text = "";

    // First column is the name of the Resource
    QStyleOptionViewItem newOptions(p_option);
    if (p_index.column() == 0)
    {
        // Get category name
        if (item->isCategory)
        {
            text = RESOURCE_RULES->getTypeString(item->type);
        }

        // Get translation
        if (text == "")
        {
            text = RESOURCE_RULES->getDefinition(item->id).translations[APPSTATUS->getCurrentLocale()];
        }
    }

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(newOptions.rect);

    drawBackground(p_painter, newOptions, p_index);
    drawDisplay(p_painter, newOptions, newOptions.rect, text);
    drawFocus(p_painter, newOptions, newOptions.rect);

    p_painter->restore();
}
