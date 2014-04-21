#include "customdescriptorpopup.h"
#include "ui_customdescriptorpopup.h"

//---------------------------------------------------------------------------------
CustomDescriptorPopup::CustomDescriptorPopup(QWidget* p_parent, const QString& p_target,
                                             bool p_showSelection, bool p_showCustom, bool p_showLevelSelection)
    : QDialog(p_parent)
    , ui(new Ui::CustomDescriptorPopup)
{
    ui->setupUi(this);

    // This dialog is always modal
    setModal(true);

    // Show/hide selection and custom frame
    ui->frameChoice->setVisible(p_showSelection);
    ui->frameCustom->setVisible(p_showCustom);
    ui->frameLevelSelect->setVisible(p_showLevelSelection);

    // Set target title
    ui->lblSelectCustomValue->setText("<html><head/><body><p><span style=\" font-size:12pt; font-weight:600;\">"
                                      + p_target + "</span></p></body></html>");
    setWindowTitle(p_target);
}

//---------------------------------------------------------------------------------
CustomDescriptorPopup::~CustomDescriptorPopup()
{
    delete ui;
}

//---------------------------------------------------------------------------------
void
CustomDescriptorPopup::setChoices(const QStringList& p_choices, const QStringList &p_choiceValues)
{
    ui->cbSelection->addItems(p_choices);
    _choiceValues = p_choiceValues;
    if (_choiceValues.size() > 0)
    {
        ui->frameChoice->setVisible(true);
        ui->frameCustom->setVisible(false);
    }
}

//---------------------------------------------------------------------------------
void
CustomDescriptorPopup::setLevels(const std::vector<float>& p_levels)
{
    _levels = p_levels;

    // Setup slider and value
    ui->sliderLevel->setMaximum(0);
    ui->sliderLevel->setMaximum(_levels.size() - 1);
    ui->sliderLevel->setTickInterval(1);
    ui->sliderLevel->setValue(0);
    ui->lblLevelValue->setText(QString("%1").arg(_levels[0]));
}

//---------------------------------------------------------------------------------
int
CustomDescriptorPopup::getLevel() const
{
    return ui->sliderLevel->value();
}

//---------------------------------------------------------------------------------
QString
CustomDescriptorPopup::getCustomization()
{
    // Custom text has precedence
    if (ui->leCustom->text().size() > 0)
    {
        return ui->leCustom->text();
    }

    return _choiceValues[ui->cbSelection->currentIndex()];
}

//---------------------------------------------------------------------------------
QString
CustomDescriptorPopup::getCustomizationString()
{
    // Custom text has precedence
    if (ui->leCustom->text().size() > 0)
    {
        return ui->leCustom->text();
    }

    return ui->cbSelection->currentText();
}

//---------------------------------------------------------------------------------
void
CustomDescriptorPopup::on_btnOk_clicked()
{
    // If we need a custom value, one has to be entered/selected
    if ((ui->frameCustom->isVisible() && ui->leCustom->text().length() < 1) ||
        (ui->frameChoice->isVisible() && ui->cbSelection->currentText() == " "))
    {
        return;
    }

    accept();
}

//---------------------------------------------------------------------------------
void
CustomDescriptorPopup::on_btnCancel_clicked()
{
    reject();
}

//---------------------------------------------------------------------------------
void
CustomDescriptorPopup::on_sliderLevel_valueChanged(int value)
{
    ui->lblLevelValue->setText(QString("%1").arg(_levels[value]));
}
