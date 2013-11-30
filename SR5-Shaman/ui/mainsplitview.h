#ifndef MAINSPLITVIEW_H
#define MAINSPLITVIEW_H

#include <QWidget>

namespace Ui {
class MainSplitView;
}
class CharacterPreviewSideInfo;
class CreationSideInfo;
class CharEditMiscTab;
class CharEditAttributeTab;
class CharEditMagicTab;

/**
 * @brief This class manages the main split view between the character preview and
 *          the actual character editor.
 * @author  TheSHEEEP
 */
class MainSplitView : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit MainSplitView(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~MainSplitView();

    /**
     * @brief Initializes the main split view.
     */
    void initialize();

    /**
     * @brief Switches the position of the character preview from left to right.
     */
    void switchPreviewOrder();

    /**
     * @brief Initializes the character creation according to the current app state.
     */
    void initializeCharacterCreation();

private slots:
    /**
     * @brief Will proceed to the next step in the guided creation.
     */
    void handleGuidedNext();

    /**
     * @brief Will disable all tabs that come after the current one.
     */
    void handleDisableNext();

private:
    Ui::MainSplitView *ui;

    bool _previewLeft;

    QWidget*                    _currentSideInfo;
    CharacterPreviewSideInfo*   _charPreviewSideInfo;
    CreationSideInfo*           _creationSideInfo;

    // All the editing/creation tabs
    CharEditMiscTab*        _tabCharEditMisc;
    CharEditAttributeTab*   _tabCharEditAttribute;
    CharEditMagicTab*       _tabCharEditMagic;

private:
    /**
     * @brief Applies the correct order of the split view.
     */
    void applyOrder();
};

#endif // MAINSPLITVIEW_H
