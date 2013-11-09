#ifndef CREATIONSIDEINFO_H
#define CREATIONSIDEINFO_H

#include <QWidget>

#include "rules/rules.h"

namespace Ui {
class CreationSideInfo;
}

class QLabel;

/**
 * @brief Side info widget that displays useful information during the guided
 *          character creation.
 * @author  TheSHEEEP
 */
class CreationSideInfo : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CreationSideInfo(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CreationSideInfo();

    /**
     * @brief Initializes this side info widget.
     */
    void initialize();

private slots:
    /**
     * @brief Updates the side info once per second.
     */
    void update();

private:
    Ui::CreationSideInfo* ui;

    QTimer* _timer;

    /**
     * @brief Updates the passed label with the current selection of the passed priority index.
     * @param p_label       The label to update.
     * @param p_prioIndex   The priority index (0-4 for A-E).
     * @param p_prio        The priority for the label.
     */
    void updatePriorityLabelText(QLabel* p_label, int p_prioIndex, Priority p_prio);
};

#endif // CREATIONSIDEINFO_H
