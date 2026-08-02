#include "SettingsDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

namespace {
constexpr int kMinFontSize = 8;
constexpr int kMaxFontSize = 24;
}

SettingsDialog::SettingsDialog(int uiFontSize, int editorFontSize, bool confirmDestructive,
                                const QString& defaultDir, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    resize(420, 280);

    auto* layout = new QVBoxLayout(this);

    auto* appearanceLabel = new QLabel("APPEARANCE", this);
    appearanceLabel->setObjectName("sectionLabel");
    layout->addWidget(appearanceLabel);

    auto* uiFontRow = new QHBoxLayout();
    uiFontRow->addWidget(new QLabel("UI font size:", this));
    m_uiFontSizeSpin = new QSpinBox(this);
    m_uiFontSizeSpin->setRange(kMinFontSize, kMaxFontSize);
    m_uiFontSizeSpin->setValue(uiFontSize);
    m_uiFontSizeSpin->setSuffix(" px");
    uiFontRow->addWidget(m_uiFontSizeSpin);
    uiFontRow->addStretch();
    layout->addLayout(uiFontRow);

    auto* editorFontRow = new QHBoxLayout();
    editorFontRow->addWidget(new QLabel("SQL editor font size:", this));
    m_editorFontSizeSpin = new QSpinBox(this);
    m_editorFontSizeSpin->setRange(kMinFontSize, kMaxFontSize);
    m_editorFontSizeSpin->setValue(editorFontSize);
    m_editorFontSizeSpin->setSuffix(" px");
    editorFontRow->addWidget(m_editorFontSizeSpin);
    editorFontRow->addStretch();
    layout->addLayout(editorFontRow);

    layout->addSpacing(10);
    auto* behaviorLabel = new QLabel("BEHAVIOR", this);
    behaviorLabel->setObjectName("sectionLabel");
    layout->addWidget(behaviorLabel);

    m_confirmDestructiveCheck = new QCheckBox("Ask for confirmation before Drop Table / Drop Index", this);
    m_confirmDestructiveCheck->setChecked(confirmDestructive);
    layout->addWidget(m_confirmDestructiveCheck);

    layout->addSpacing(10);
    auto* filesLabel = new QLabel("FILES", this);
    filesLabel->setObjectName("sectionLabel");
    layout->addWidget(filesLabel);

    auto* dirRow = new QHBoxLayout();
    m_defaultDirEdit = new QLineEdit(defaultDir, this);
    m_defaultDirEdit->setPlaceholderText("Last used folder (default)");
    dirRow->addWidget(m_defaultDirEdit, 1);
    auto* browseBtn = new QPushButton("Browse…", this);
    dirRow->addWidget(browseBtn);
    layout->addLayout(dirRow);
    auto* dirHint = new QLabel("Starting folder for Open/New/Export/Import dialogs.", this);
    dirHint->setStyleSheet("color: #888780; font-size: 10px;");
    layout->addWidget(dirHint);

    layout->addStretch(1);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText("Save");
    layout->addWidget(buttons);

    connect(browseBtn, &QPushButton::clicked, this, &SettingsDialog::browseForDir);
    connect(buttons, &QDialogButtonBox::accepted, this, &SettingsDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::reject);
}

void SettingsDialog::browseForDir()
{
    const QString chosen = QFileDialog::getExistingDirectory(this, "Default Folder", m_defaultDirEdit->text());
    if (!chosen.isEmpty()) {
        m_defaultDirEdit->setText(chosen);
    }
}

int SettingsDialog::uiFontSize() const
{
    return m_uiFontSizeSpin->value();
}

int SettingsDialog::editorFontSize() const
{
    return m_editorFontSizeSpin->value();
}

bool SettingsDialog::confirmDestructive() const
{
    return m_confirmDestructiveCheck->isChecked();
}

QString SettingsDialog::defaultDir() const
{
    return m_defaultDirEdit->text().trimmed();
}

void SettingsDialog::accept()
{
    const QString dir = defaultDir();
    if (!dir.isEmpty() && !QFileInfo(dir).isDir()) {
        QMessageBox::warning(this, "Settings", "\"" + dir + "\" is not a valid folder.");
        return;
    }
    QDialog::accept();
}
