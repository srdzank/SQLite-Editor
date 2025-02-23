#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDebug>

#ifdef QT_DEBUG
void logMessage(const QString &message, const char *file, int line);

#define LOG(message) logMessage(message, __FILE__, __LINE__)
#else
#define LOG(message) // No-op in release builds
#endif

#endif // LOGGER_H
