#ifndef CHAREDITATTRIBUTETAB_H
#define CHAREDITATTRIBUTETAB_H

#include <QWidget>
#include <QMap>

namespace Ui {
class CharEditAttributeTab;
}

class QSpinBox;
class PriorityEventFilter;

/**
 * @brief This tab holds all information about the character's attributes.
 * @author  TheSHEEEP
 */
class CharEditAttributeTab : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit CharEditAttributeTab(QWidget *parent = 0);

    /**
     * @brief Destructor.
     */
    ~CharEditAttributeTab();

    /**
     * @brief Initializes this character tab.
     */
    void initialize();

    /**
     * @brief Will show/hide the elements specific to the guided creation.
     *          Will also re-calculate all the displayed values.
     */
    virtual void showEvent(QShowEvent* /*unused*/);

signals:
    void guidedNextStep();
    void disableNext();

private slots:
    /**
     * @brief Called whenever any attribute spin button changed.
     *          Will update the according text and the character values.
     */
    void spinBoxChanged(int p_newValue);

    /**
     * @brief Will enable the attribute layout and update all values.
     */
    void on_cbPriority_currentIndexChanged(int p_index);

    /**
     * @brief Emits the guidedNextStep signal.
     */
    void on_btnGuidedContinue_clicked();

private:
    Ui::CharEditAttributeTab*   ui;
    QMap<QSpinBox*, QString>    _attributeSpinBoxAssignment;
    PriorityEventFilter*        _filter;

    /**
     * @brief Updates the text on the passed spinbox to reflect the current character values.
     * @param p_spinBox     The spinbox to update.
     * @param p_attribute   The attribute.
     */
    void updateSpinBoxText(QSpinBox* p_spinBox, const QString& p_attribute);

    /**
     * @brief Updates all displayed values.
     */
    void updateValues();

    /**
     * @brief Updates all displayed derived values that depend on the passed attribute.
     * @param p_all if this is true, all value will be updated independent of the passed attribute;
     */
    void updateDerivedValues(const QString& p_attribute, bool p_all);

    /**
     * @brief During guided creation, checks if all conditions are met to continue with the creation.
     */
    void checkContinue();
};

#endif // CHAREDITATTRIBUTETAB_H
