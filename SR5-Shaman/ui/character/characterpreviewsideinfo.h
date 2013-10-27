#ifndef CHARACTERPREVIEW_H
#define CHARACTERPREVIEW_H

#include <QWidget>

namespace Ui {
class CharacterPreviewSideInfo;
}

/**
 * @brief Side info bar to show character preview during character editing.
 * @author  TheSHEEEP
 */
class CharacterPreviewSideInfo : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharacterPreviewSideInfo(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharacterPreviewSideInfo();

    /**
     * @brief Initializes the character preview.
     */
    void initialize();

private:
    Ui::CharacterPreviewSideInfo *ui;
};

#endif // CHARACTERPREVIEW_H
