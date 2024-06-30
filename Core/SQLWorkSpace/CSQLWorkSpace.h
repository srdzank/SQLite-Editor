#ifndef CSQLWORKSPACE_H
#define CSQLWORKSPACE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStringListModel>
#include <QCompleter>
#include "../AutoComplete/AutoCompleteTextEdit.h"
#include <QMessageBox>

class CSQLWorkSpace : public QWidget
{
    Q_OBJECT

public:
    explicit CSQLWorkSpace(QWidget* parent = nullptr);

    AutoCompleteTextEdit* sqlInput;
    QPushButton* executeButton;
    QPushButton* clearButton;
    QCompleter* completer;
    QStringListModel* completerModel;

signals:
    void executeSQL(const QString& query);
    void clearSQL();

private slots:
    void onExecuteButtonClicked();
    void onClearButtonClicked();

private:
    QVBoxLayout* layout;
    void applyCompleterStyles();
};

#endif // CSQLWORKSPACE_H