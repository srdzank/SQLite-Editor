#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include "TimeLineWidget/customwidget.h"
#include <QResizeEvent>

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

    void CreateMenu();
    void createToolBar();

    Ui::MainWindow *ui;
    CustomWidget *customWidget;
    QVBoxLayout *layout;


};
#endif // MAINWINDOW_H
