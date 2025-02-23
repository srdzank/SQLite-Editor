#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QToolTip {"
        "background-color: #ffffe0;"  // light yellow background color
        "color: #000000;"  // black text color
        "border: 1px solid #000000;"  // black border
        "}");
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
