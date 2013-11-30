#ifndef COMBOPRIORITYDELEGATE_H
#define COMBOPRIORITYDELEGATE_H

#include <QObject>

class QComboBox;

/**
 * @brief Event filter to catch the even of a QComboBox to change the texts and their colors,
 *          depending on the priority status of the character.
 * @note    Make sure to connect the COmboBox to the handlePrioritySelection slot!
 * @author  TheSHEEEP
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

public slots:
    /**
     * @brief Will hide the possible suffix when a item was selected.
     */
    void handlePrioritySelection(int p_index);
};

#endif // COMBOPRIORITYDELEGATE_H
