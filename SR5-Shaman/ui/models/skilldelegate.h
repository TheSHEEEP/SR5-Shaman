#ifndef SKILLDELEGATE_H
#define SKILLDELEGATE_H

#include <QItemDelegate>

class SkillDefinition;

/**
 * @brief This delegate can be configred to display a skill in different variations.
 *          Default is to display the translation.
 */
class SkillDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    SkillDelegate();

    /**
     * @brief Destructor.
     */
    ~SkillDelegate();

    /**
     * @brief Creates an editor widget for some items.
     */
    QWidget* createEditor(QWidget* p_parent, const QStyleOptionViewItem& p_option,
                          const QModelIndex& p_index) const;

    /**
     * @brief Sets the editor's data from the passed model index.
     */
    void setEditorData(QWidget* p_editor, const QModelIndex& p_index) const;

    /**
     * @brief Updates the models data by reading from the editor.
     */
    void setModelData(QWidget* p_editor, QAbstractItemModel* p_model,
                      const QModelIndex& p_index) const;

    /**
     * @brief Returns the correct size hint for the passed index.
     * @param p_option  The style options.
     * @param p_index   The index.
     */
    QSize sizeHint(const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

    /**
     * @brief Defines how the skill will be painted.
     * @param p_painter The painter to use.
     * @param p_option  The option with the default display parameters.
     * @param p_index   The model index to display.
     */
    void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

signals:
    /**
     * @brief Emitted when the user changes a skill level in the editor.
     */
    void skillChanged();

    /**
     * @brief Emitted when the user changes a skill level in the editor.
     */
    void addButtonClicked(SkillDefinition* p_skill) const;

    /**
     * @brief Emitted when the user clicks on a specializations button in the editor.
     */
    void specializationsClicked(SkillDefinition* p_skill) const;

private slots:
    /**
     * @brief Triggered when the value in the spinbox changed.
     * @param p_newValue    The new value in the spinbox.
     */
    void spinBoxChanged(int p_newValue);

    /**
     * @brief Triggered when an add-button was clicked.
     */
    void addButtonClicked();

    /**
     * @brief Triggered when a specializations button was clicked.
     */
    void specializationsClicked();
};

#endif // SKILLDELEGATE_H
