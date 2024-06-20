#include "logger.h"
#include <QFileInfo>

void logMessage(const QString &message, const char *file, int line)
{
    QString fileName = QFileInfo(file).fileName();
    qDebug() << QString("%1 (%2:%3)").arg(message).arg(fileName).arg(line);
}
