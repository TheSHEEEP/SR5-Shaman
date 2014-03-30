#ifndef QUALITYDELEGATE_H
#define QUALITYDELEGATE_H

#include <QItemDelegate>

/**
 * @brief This delegate can be configured to display a magic definition in different variations.
 *          Default is to display the translation only.
 */
class QualityDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    QualityDelegate();

    /**
     * @brief Destructor.
     */
    ~QualityDelegate();

    /**
     * @brief Defines how the quality definition will be painted.
     * @param p_painter The painter to use.
     * @param p_option  The option with the default display parameters.
     * @param p_index   The model index to display.
     */
    void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

    /**
     * @param p_show    If this is true, the delegate will show the level of a quality.
     */
    void setShowQualityLevel(bool p_show);

private:
    bool    _showQualityLevel;
};

//---------------------------------------------------------------------------------
inline
void
QualityDelegate::setShowQualityLevel(bool p_show)
{
    _showQualityLevel = p_show;
}

#endif // QUALITYDELEGATE_H
