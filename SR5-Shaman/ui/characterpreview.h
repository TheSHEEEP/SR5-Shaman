#ifndef CHARACTERPREVIEW_H
#define CHARACTERPREVIEW_H

#include <QWidget>

namespace Ui {
class CharacterPreview;
}

class CharacterPreview : public QWidget
{
    Q_OBJECT

public:
    explicit CharacterPreview(QWidget *parent = 0);
    ~CharacterPreview();

private:
    Ui::CharacterPreview *ui;
};

#endif // CHARACTERPREVIEW_H
