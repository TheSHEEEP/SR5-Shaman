#ifndef SKILLDELEGATE_H
#define SKILLDELEGATE_H

#include <QItemDelegate>

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
     * @brief Defines how the skill will be painted.
     * @param p_painter The painter to use.
     * @param p_option  The option with the default display parameters.
     * @param p_index   The model index to display.
     */
    void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;
};

#endif // SKILLDELEGATE_H
