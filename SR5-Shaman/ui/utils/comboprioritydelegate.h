#ifndef COMBOPRIORITYDELEGATE_H
#define COMBOPRIORITYDELEGATE_H

#include <QItemDelegate>

class QComboBox;

/**
 * @brief Event filter to catch the show even of a QComboBox's ListView and change the texts.
 */
class PriorityEventFilter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    PriorityEventFilter();

    /**
     * @brief Destructor.
     */
    ~PriorityEventFilter();

    /**
     * @brief Filters out the show event to adjust the displayed texts.
     */
    bool eventFilter(QObject* p_obj, QEvent* p_event);
};

/**
 * @brief This delegate is to be used on priority selection combo boxes.
 *          They must have their data entries as integers ranging from -1 to 4.
 *
 *          The delegate will mark priorities that are already taken and tell by what.
 */
class ComboPriorityDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief Constructor.
     */
    ComboPriorityDelegate(QComboBox* parent);

    /**
    * @brief Destructor.
    */
    virtual ~ComboPriorityDelegate();

    /**
     * @brief Will color the specific item depending on a possible other priority in the same priority index.
     * @param p_painter The painter that is used for the actual painting.
     * @param p_option  The options.
     * @param p_index   The index of the model.
     */
    virtual void paint(QPainter* p_painter, const QStyleOptionViewItem& p_option, const QModelIndex& p_index) const;

signals:

private slots:
    /**
     * @brief Will correct the displayed texts on the ComboBox if required.
     */
    void handleComboBoxOpened(int p_index);

private:
    PriorityEventFilter* _filter;
};

#endif // COMBOPRIORITYDELEGATE_H
