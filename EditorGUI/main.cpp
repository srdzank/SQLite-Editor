#include "mainwindow.h"
#include <QApplication>

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
#if defined(Q_OS_WIN)
    // Овозможување PerMonitorV2 DPI Awareness за Windows
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif

    QApplication a(argc, argv);

    a.setStyleSheet("QToolTip {"
        "background-color: #ffffe0;"  // light yellow background color
        "color: #000000;"             // black text color
        "border: 1px solid #000000;"  // black border
        "}");

    MainWindow w;
    w.showMaximized();
    return a.exec();
}