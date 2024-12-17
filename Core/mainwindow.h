#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStringListModel>
#include <sqlite3.h>
#include "tableviewworkspace/ctableviewworkspace.h"
#include "dbnavigator/cdbnavigator.h"
#include "SQLParserAPI.h"  // Include your SQL parser API header
#include "SQLWorkSpace/CSQLWorkSpace.h"  // Include the custom CSQLWorkSpace header
#include <QLabel>
#include "ERDiagram/CERDiagram.h"
#include "TableEditorDialog/CTableEditorDialog.h"


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    sqlite3* m_db;
    QVBoxLayout* layout;
    QSplitter* splitter;
    CSQLWorkSpace* customWidget;  // Use the custom CSQLWorkSpace
    CTableViewWorkspace* tableViewWidget;
    CDBNavigator* navigator;
    QCompleter* completer;
    QStringListModel* completerModel;
    QString lastOpenedFileName;
    ERDiagram* erDiagram;

    void CreateMenu();
    void createToolBar();
    void updateLastOpenedFileName(const QString& fileName);
    void updateCompleterModel();
    void parseSQLAndSuggest();  // New method to parse SQL and suggest words
private:
    QLabel* errorLabel;

private slots:
    void onActionExit();
    void onActionOpen();
    void onCustomButtonClicked();
    void procClickedTableItem(const QString& table);
    void onSQLTextChanged();  // New slot for SQL text changes
    void onExecuteSQL(const QString& query);  // Slot for executing SQL
    void onClearSQL();  // Slot for clearing SQL input
    void erDiagramProc();
protected:
    void resizeEvent(QResizeEvent* event) override;
    void executeSQLCommand(sqlite3* db, const QString& eSql);  // Add this line
    void printResults(const std::vector<std::vector<std::string>>& results, const std::vector<std::string>& columnNames);  // Add this line
};

#endif // MAINWINDOW_H
