#ifndef MAGICDELEGATE_H
#define MAGICDELEGATE_H

#include <QItemDelegate>

/**
 * @brief This delegate can be configured to display a magic definition in different variations.
 *          Default is to display the translation only.
 */
class MagicDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    MagicDelegate();

    /**
     * @brief Destructor.
     */
    ~MagicDelegate();

    /**
     * @brief Defines how the magic definition will be painted.
     * @param p_painter The painter to use.
     * @param p_option  The option with the default display parameters.
     * @param p_index   The model index to display.
     */
    void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;
};

#endif // MAGICDELEGATE_H
