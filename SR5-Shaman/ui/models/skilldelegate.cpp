#include "skilldelegate.h"

#include <QPainter>
#include <QLabel>
#include <QSpinBox>
#include <QApplication>

#include "data/appstatus.h"
#include "data/character/characterchoices.h"
#include "data/character/charactervalues.h"
#include "data/dictionary.h"
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
QWidget*
SkillDelegate::createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option,
                                       const QModelIndex& p_index) const
{
    if (p_index.column() == 1)
    {
        QSpinBox* editor = new QSpinBox(p_parent);
        editor->setMinimum(0);
        editor->setMaximum(100);
        // TODO: here
        // Connect the editor to the skill tab if present

        return editor;
    }
    else
    {
        return new QLabel("Nothing here");
    }
}

//---------------------------------------------------------------------------------
void
SkillDelegate::setEditorData(QWidget* p_editor, const QModelIndex& p_index) const
{
    // Create the postfix
    SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());
    int skillValuePure = CHARACTER_VALUES->getSkill(item->id, false);
    int skillValueMod = CHARACTER_VALUES->getSkill(item->id);
    int skillMax = CHARACTER_VALUES->getSkillMax(item->id);
    QString postfix = skillValueMod != skillValuePure ?
                QString(" (+%1)").arg(skillValueMod - skillValuePure) :
                "";
    postfix.append(QString(" / %1").arg(skillMax));

    // Apply value & postfix
    QSpinBox* spinBox = static_cast<QSpinBox*>(p_editor);
    spinBox->setValue(skillValuePure);
    spinBox->setSuffix(postfix);
    spinBox->setAlignment(Qt::AlignHCenter);
    spinBox->setMaximum(skillMax);
}

//---------------------------------------------------------------------------------
void
SkillDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model,
                            const QModelIndex& p_index) const
{
    // Get pure skill value
    SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());
    int skillValuePure = CHARACTER_VALUES->getSkill(item->id, false);

    // Increase skill
    QSpinBox* spinBox = static_cast<QSpinBox*>(p_editor);
    int value = spinBox->value();
    CHARACTER_CHOICES->increaseSkill(item->id, value - skillValuePure);
}

//---------------------------------------------------------------------------------
void
SkillDelegate::paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const
{
    // Get the item
    SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());
    QString text = "";

    // First column is the name of the skill
    QStyleOptionViewItem newOptions(p_option);
    if (p_index.column() == 0)
    {
        // Get category name
        if (item->isCategory)
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
        if (item->parent && !item->parent->isCategory && item->parent->id != "DEFINITION" &&
            SKILL_RULES->getDefinition(item->parent->id).isGroup)
        {
            newOptions.palette.setColor(QPalette::Text, QColor(100, 100, 100));
        }
    }
    // Second column is the skill value, presented as a spin box
    else if (p_index.column() == 1 &&
             !item->isCategory)
    {
        // Get the postfix
        int skillValuePure = CHARACTER_VALUES->getSkill(item->id, false);
        int skillValueMod = CHARACTER_VALUES->getSkill(item->id);
        int skillMax = CHARACTER_VALUES->getSkillMax(item->id);
        QString postfix = skillValueMod != skillValuePure ?
                    QString(" (+%1)").arg(skillValueMod - skillValuePure) :
                    "";
        postfix.append(QString(" / %1").arg(skillMax));

        // Draw a custom spin box
        p_painter->save();
        QStyleOptionSpinBox spinBoxOption;
        spinBoxOption.rect = newOptions.rect;
        spinBoxOption.state = newOptions.state;
        spinBoxOption.state |= QStyle::State_Enabled;
        spinBoxOption.activeSubControls = QStyle::SC_All;
        spinBoxOption.subControls = QStyle::SC_All;
        spinBoxOption.buttonSymbols = QAbstractSpinBox::UpDownArrows;
        spinBoxOption.frame = true;
        spinBoxOption.palette = newOptions.palette;

        // Get style and draw
        QStyle* style = qApp->style();
        style->drawComplexControl(QStyle::CC_SpinBox, &spinBoxOption, p_painter, 0);
        style->drawItemText(p_painter, newOptions.rect, Qt::AlignHCenter, newOptions.palette, true,
                            QString("%1%2").arg(skillValuePure).arg(postfix));
        p_painter->restore();
        return;
    }
    // Third column is the connected attribute
    else if (p_index.column() == 2 &&
             !item->isCategory && !item->isGroup)
    {
        text = Dictionary::getTranslation(item->attribute);
        newOptions.displayAlignment = Qt::AlignHCenter;
    }
    // Fourth column is the dice pool
    else if (p_index.column() == 3 &&
             !item->isCategory && !item->isGroup)
    {
        int value = CHARACTER_VALUES->getAttribute(item->attribute);
        value += CHARACTER_VALUES->getSkill(item->id);
        text = QString("%1").arg(value);
        newOptions.displayAlignment = Qt::AlignHCenter;
    }

    // Call painter methods for drawing
    p_painter->save();
    p_painter->setClipRect(newOptions.rect);

    drawBackground(p_painter, newOptions, p_index);
    drawDisplay(p_painter, newOptions, newOptions.rect, text);
    drawFocus(p_painter, newOptions, newOptions.rect);

    p_painter->restore();
}
