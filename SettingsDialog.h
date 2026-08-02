#pragma once

#include <QDialog>
#include <QString>

class QCheckBox;
class QLineEdit;
class QSpinBox;

// Modal dialog for editing app-wide preferences: the UI font size, the
// monospace font size used by the SQL preview/editor, whether
// destructive schema actions (Drop Table/Index) ask for confirmation,
// and the folder the Open/New/Export/Import file dialogs start in.
// Like the other dialogs in this app, this only collects and validates
// values - MainWindow owns reading/writing QSettings and applying them.
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(int uiFontSize, int editorFontSize, bool confirmDestructive,
                   const QString& defaultDir, QWidget* parent = nullptr);

    int uiFontSize() const;
    int editorFontSize() const;
    bool confirmDestructive() const;
    QString defaultDir() const;

private slots:
    void browseForDir();

private:
    void accept() override;

    QSpinBox* m_uiFontSizeSpin = nullptr;
    QSpinBox* m_editorFontSizeSpin = nullptr;
    QCheckBox* m_confirmDestructiveCheck = nullptr;
    QLineEdit* m_defaultDirEdit = nullptr;
};
