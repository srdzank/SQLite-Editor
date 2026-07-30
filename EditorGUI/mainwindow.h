#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStringListModel>
#include <QGraphicsView>
#include <QLabel>
#include <sqlite3.h>

#include "tableviewworkspace/ctableviewworkspace.h"
#include "dbnavigator/cdbnavigator.h"
#include "SQLParserAPI.h"
#include "SQLWorkSpace/CSQLWorkSpace.h"
#include "ERDiagram/CERDiagram.h"
#include "TableEditorDialog/CTableEditorDialog.h"
#include "TableDiagram/CTableDiagram.h"
#include "SettingsDialog/CSettingsDialog.h"

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
    QString m_table;
    QVBoxLayout* layout;
    QSplitter* splitter;
    CSQLWorkSpace* customWidget;
    CTableViewWorkspace* tableViewWidget;
    CDBNavigator* navigator;
    QCompleter* completer;
    QStringListModel* completerModel;
    QString lastOpenedFileName;
    ERDiagram* erDiagram;
    TableDiagram* tableDiagram;
    QGraphicsView* view;
    QLabel* errorLabel;

    void CreateMenu();
    void createToolBar();
    void updateLastOpenedFileName(const QString& fileName);
    void updateCompleterModel();

private slots:
    void onActionExit();
    void onActionOpen();
    void onCustomButtonClicked();
    void onTableCreateDialogClicked();
    void onTableEditDialogClicked();
    void procClickedTableItem(const QString& table);
    void onSQLTextChanged();
    void onExecuteSQL(const QString& query);
    void onClearSQL();
    void erDiagramProc();
    void onNewDatabase();
    void onGetSQLQuery();
    void onSettingsClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void executeSQLCommand(sqlite3* db, const QString& eSql);
    void printResults(const std::vector<std::vector<std::string>>& results, const std::vector<std::string>& columnNames);
};

#endif // MAINWINDOW_H