#include "CSettingsDialog.h"

CSettingsDialog::CSettingsDialog(const QFont& currentFont, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Application Settings"));
    resize(350, 180);

    QFormLayout* formLayout = new QFormLayout();

    // Font Family Selector
    fontComboBox = new QFontComboBox(this);
    fontComboBox->setCurrentFont(currentFont);
    formLayout->addRow(tr("Font Family:"), fontComboBox);

    // Font Size Selector
    fontSizeSpinBox = new QSpinBox(this);
    fontSizeSpinBox->setRange(8, 36);
    fontSizeSpinBox->setValue(currentFont.pointSize() > 0 ? currentFont.pointSize() : 10);
    formLayout->addRow(tr("Font Size:"), fontSizeSpinBox);

    // Buttons
    btnOk = new QPushButton(tr("OK"), this);
    btnCancel = new QPushButton(tr("Cancel"), this);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnOk);
    btnLayout->addWidget(btnCancel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

QFont CSettingsDialog::getSelectedFont() const
{
    QFont font = fontComboBox->currentFont();
    font.setPointSize(fontSizeSpinBox->value());
    return font;
}