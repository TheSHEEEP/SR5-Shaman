#include "skilldelegate.h"

#include <QPainter>

#include "data/appstatus.h"
#include "rules/rules.h"
#include "ui/models/skilltreemodel.h"
#include "ui/models/skillsortfilterproxymodel.h"

//---------------------------------------------------------------------------------
SkillDelegate::SkillDelegate()
{

}

//---------------------------------------------------------------------------------
SkillDelegate::~SkillDelegate()
{

}


//---------------------------------------------------------------------------------
void
SkillDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get the item
    SkillModelItem* item = static_cast<SkillModelItem*>(p_index.data().value<void*>());

    // Get category name
    QString text = "";
    if (item->id.startsWith("CATEGORY"))
    {
        if (item->id == "CATEGORY_GROUPS")
        {
            // Ugly workaround to be able to handle the "groups" category the same way
            text = tr("Skill Groups");
        }
        else
        {
            text = SKILL_RULES->getTypeString(item->type);
        }
    }

    // Get translation
    if (text == "")
    {
        text = SKILL_RULES->getDefinition(item->id).translations[APPSTATUS->getCurrentLocale()];
    }

    // If this is the child of a group, change color for clarity to make it look disabled
    QStyleOptionViewItem newOptions(p_option);
    if (item->parent && !item->parent->isCategory && item->parent->id != "DEFINITION" &&
        SKILL_RULES->getDefinition(item->parent->id).isGroup)
    {
        newOptions.palette.setColor(QPalette::Text, QColor(100, 100, 100));
    }

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(newOptions.rect);

    drawBackground(p_painter, newOptions, p_index);
    drawDisplay(p_painter, newOptions, newOptions.rect, text);
    drawFocus(p_painter, newOptions, newOptions.rect);

    p_painter->restore();
}
