#ifndef CURRENTSETTINGSCONTAINER_H
#define CURRENTSETTINGSCONTAINER_H

#include <QWidget>

namespace Ui {
class CurrentSettingsContainer;
}

/**
 * @brief This class is onyl responsible for holding the actual settings layout.
 * @author TheSHEEEP
 */
class CurrentSettingsContainer : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CurrentSettingsContainer(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~CurrentSettingsContainer();

    /**
     * @brief Adds the passed widget.
     */
    void addWidget(QWidget* p_widget);

    /**
     * @brief Removes the passed widget.
     */
    void removeWidget(QWidget* p_widget);

private:
    Ui::CurrentSettingsContainer *ui;
};

#endif // CURRENTSETTINGSCONTAINER_H
