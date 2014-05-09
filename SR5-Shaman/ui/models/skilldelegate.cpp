#include "skilldelegate.h"

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
        SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());

        // Either create a spin box or a button to click
        if (!item->requiresCustom || item->custom != "")
        {
            QSpinBox* editor = new QSpinBox(p_parent);
            editor->setMinimum(0);
            editor->setMaximum(100);
            SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());
            editor->setProperty("skill", QVariant::fromValue(item));
            connect(editor, SIGNAL(valueChanged(int)), SLOT(spinBoxChanged(int)));

            return editor;
        }
        else
        {
            QPushButton* button = new QPushButton(p_parent);
            button->setText(Dictionary::getTranslation("ADD"));
            button->setProperty("skill", QVariant::fromValue(item));
            connect(button, SIGNAL(clicked()), SLOT(addButtonClicked()));

            emit addButtonClicked(item);
            return button;
        }
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
    SkillDefinition* item = static_cast<SkillDefinition*>(p_index.data().value<void*>());

    // Only do something here if this was sent by a spin box
    if (qobject_cast<QSpinBox*>(p_editor))
    {
        // Create the postfix
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
}

//---------------------------------------------------------------------------------
void
SkillDelegate::setModelData(QWidget* p_editor, QAbstractItemModel* p_model,
                            const QModelIndex& p_index) const
{
    // spinBoxChanged does this as it is called more reliably
    // Also, nothing to do in case of push button click
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
    // Or the button to add a custom variant of the skill
    else if (p_index.column() == 1 &&
             !item->isCategory)
    {
        // Show a SpinBox with the value
        if (!item->requiresCustom || item->custom != "")
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
            QSpinBox box;
            style->drawComplexControl(QStyle::CC_SpinBox, &spinBoxOption, p_painter,
                                      static_cast<QWidget*>(&box));
            style->drawItemText(p_painter, newOptions.rect, Qt::AlignHCenter, newOptions.palette, true,
                                QString("%1%2").arg(skillValuePure).arg(postfix));
            p_painter->restore();
        }
        else
        {
            // Draw a push button
            p_painter->save();
            QStyleOptionButton btnOption;
            btnOption.rect = newOptions.rect;
            btnOption.state = newOptions.state;
            btnOption.state |= QStyle::State_Enabled;
            btnOption.text = Dictionary::getTranslation("ADD");
            btnOption.palette = newOptions.palette;

            // Get style and draw
            QStyle* style = qApp->style();
            QPushButton btn;
            style->drawControl(QStyle::CE_PushButton, &btnOption, p_painter,
                               static_cast<QWidget*>(&btn));
            p_painter->restore();
        }
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
             !item->isCategory && !item->isGroup &&
             (!item->requiresCustom || item->custom != ""))
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

//---------------------------------------------------------------------------------
void
SkillDelegate::spinBoxChanged(int p_newValue)
{
    QSpinBox* spinBox = static_cast<QSpinBox*>(sender());
    SkillDefinition* item = spinBox->property("skill").value<SkillDefinition*>();
    int skillValuePure = CHARACTER_VALUES->getSkill(item->id, false);

    // Increase skill
    CHARACTER_CHOICES->increaseSkill(item->id, p_newValue - skillValuePure);

    // Update spinbox text
    int currentValuePure = CHARACTER_VALUES->getSkill(item->id, false);
    int currentValueModified = CHARACTER_VALUES->getSkill(item->id);
    int difference = currentValueModified - currentValuePure;
    int maxValue = CHARACTER_VALUES->getSkillMax(item->id);
    QString postfix = currentValueModified != currentValuePure ?
                QString(" (+%1)").arg(difference) :
                "";
    postfix.append(QString(" / %1").arg(maxValue));
    spinBox->setSuffix(postfix);
    spinBox->blockSignals(true);
    spinBox->setValue(currentValuePure);
    spinBox->blockSignals(false);

    emit skillChanged();
}

//---------------------------------------------------------------------------------
void
SkillDelegate::addButtonClicked()
{
    QPushButton* button = static_cast<QPushButton*>(sender());
    SkillDefinition* item = button->property("skill").value<SkillDefinition*>();

    emit addButtonClicked(item);
}
