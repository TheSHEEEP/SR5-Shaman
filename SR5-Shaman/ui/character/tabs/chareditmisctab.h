#ifndef CHAREDITMISCTAB_H
#define CHAREDITMISCTAB_H

#include <QWidget>

namespace Ui {
class CharEditMiscTab;
}

/**
 * @brief This tabs hold character information like name, nick, portrait and metatype.
 * @author  TheSHEEEP
 */
class CharEditMiscTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditMiscTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditMiscTab();

    /**
     * @brief Initializes this character tab.
     */
    void initialize();

private slots:
    /**
     * @brief Stores the new metatype and updates priority selection.
     */
    void on_cbMetatype_currentIndexChanged(int index);

    /**
     * @brief Stores the new priority for the metatype.
     */
    void on_cbPriority_currentIndexChanged(int index);

private:
    Ui::CharEditMiscTab *ui;
};

#endif // CHAREDITMISCTAB_H
