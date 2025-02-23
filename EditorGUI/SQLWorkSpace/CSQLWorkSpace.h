#ifndef CSQLWORKSPACE_H
#define CSQLWORKSPACE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStringListModel>
#include <QCompleter>
#include "../AutoComplete/AutoCompleteTextEdit.h"
#include <QMessageBox>
#include <QLabel>


class CSQLWorkSpace : public QWidget
{
    Q_OBJECT

public:
    explicit CSQLWorkSpace(QWidget* parent = nullptr);

    AutoCompleteTextEdit* sqlInput;
    QPushButton* executeButton;
    QPushButton* clearButton;
    QPushButton* ERDiagramButton;
    QCompleter* completer;
    QStringListModel* completerModel;
    QLabel* errorLabel;
signals:

    void executeSQL(const QString& query);
    void clearSQL();
    void erDiagram();

private slots:
    void onExecuteButtonClicked();
    void onClearButtonClicked();
    void onERDiagramClicked();

private:
    QVBoxLayout* layout;
    void applyCompleterStyles();

};

#endif // CSQLWORKSPACE_H
