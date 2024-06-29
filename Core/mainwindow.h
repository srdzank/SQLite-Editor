#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include "TimeLineWidget/customwidget.h"
#include <QResizeEvent>
#include <QLabel>
#include <QTimer>
#include "Video/QVideo.h"
#include <QTableWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event) override;
private slots:
    void onActionExit();
    void onCustomButtonClicked();

private:
    void printResults(const std::vector<std::vector<std::string>>& results);
    void CreateMenu();
    void createToolBar();

    Ui::MainWindow *ui;
    CustomWidget *customWidget;
    QVBoxLayout *layout;
    CVideo* c;
    QTableWidget* resultTableWidget;


};
#endif // MAINWINDOW_H
