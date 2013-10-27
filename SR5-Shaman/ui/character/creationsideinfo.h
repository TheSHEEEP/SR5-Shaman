#ifndef CREATIONSIDEINFO_H
#define CREATIONSIDEINFO_H

#include <QWidget>

namespace Ui {
class CreationSideInfo;
}

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

private:
    Ui::CreationSideInfo *ui;
};

#endif // CREATIONSIDEINFO_H
