#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>
#include <QFontComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

class CSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSettingsDialog(const QFont& currentFont, QWidget* parent = nullptr);
    QFont getSelectedFont() const;

private:
    QFontComboBox* fontComboBox;
    QSpinBox* fontSizeSpinBox;
    QPushButton* btnOk;
    QPushButton* btnCancel;
};

#endif // CSETTINGSDIALOG_H