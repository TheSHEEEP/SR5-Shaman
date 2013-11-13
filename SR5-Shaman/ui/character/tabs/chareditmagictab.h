#ifndef CHAREDITMAGICTAB_H
#define CHAREDITMAGICTAB_H

#include <QWidget>

namespace Ui {
class CharEditMagicTab;
}

/**
 * @brief This tab holds all information about the character's magic type, selection of magical / resonance skills
 *          and the selection of spells / complex forms.
 */
class CharEditMagicTab : public QWidget
{
    Q_OBJECT

public:
    explicit CharEditMagicTab(QWidget *parent = 0);
    ~CharEditMagicTab();

private:
    Ui::CharEditMagicTab *ui;
};

#endif // CHAREDITMAGICTAB_H
