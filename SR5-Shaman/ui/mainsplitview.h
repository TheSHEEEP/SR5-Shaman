#ifndef MAINSPLITVIEW_H
#define MAINSPLITVIEW_H

#include <QWidget>

namespace Ui {
class MainSplitView;
}

class MainSplitView : public QWidget
{
    Q_OBJECT

public:
    explicit MainSplitView(QWidget *parent = 0);
    ~MainSplitView();

private:
    Ui::MainSplitView *ui;
};

#endif // MAINSPLITVIEW_H
