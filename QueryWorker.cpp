#include "QueryWorker.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QUuid>

namespace {
// Register the cross-thread signal payload type once, the first time a
// worker is constructed. Needed because QVector<QStringList> isn't one
// of Qt's built-in meta types, and queued connections (which is what a
// cross-thread signal/slot always uses) require every argument type to
// be registered with the meta-object system.
void registerResultRowsMetaType()
{
    static const int registered = qRegisterMetaType<ResultRows>("ResultRows");
    Q_UNUSED(registered);
}
} // namespace

QueryWorker::QueryWorker(QString dbPath, QObject* parent)
    : QObject(parent)
    , m_dbPath(std::move(dbPath))
    // Each QSqlDatabase connection is identified by a global name; a
    // per-worker UUID keeps this from colliding with the GUI thread's
    // own connection (or with any other worker's, if one is ever
    // created while an old one is still shutting down).
    , m_connectionName("query_worker_" + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
    registerResultRowsMetaType();
}

QueryWorker::~QueryWorker()
{
    if (m_connected) {
        // The QSqlDatabase handle must be released before removeDatabase()
        // is called, or Qt will warn about it still being in use - the
        // extra scope makes sure db goes out of scope first.
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            db.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
}

bool QueryWorker::ensureConnected()
{
    if (m_connected) {
        return true;
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        emit failed(db.lastError().text());
        return false;
    }

    m_connected = true;
    return true;
}

void QueryWorker::execute(const QString& sql)
{
    if (!ensureConnected()) {
        return;
    }

    QSqlQuery query(QSqlDatabase::database(m_connectionName));
    if (!query.exec(sql)) {
        emit failed(query.lastError().text());
        return;
    }

    // Copy everything out of QSqlQuery into plain strings before
    // crossing back to the GUI thread - the query object itself is
    // bound to this thread's connection and must not be touched from
    // MainWindow.
    const QSqlRecord record = query.record();
    const int columnCount = record.count();

    QStringList headers;
    headers.reserve(columnCount);
    for (int c = 0; c < columnCount; ++c) {
        headers << record.fieldName(c);
    }

    ResultRows rows;
    while (query.next()) {
        QStringList row;
        row.reserve(columnCount);
        for (int c = 0; c < columnCount; ++c) {
            row << query.value(c).toString();
        }
        rows.push_back(row);
    }

    emit succeeded(headers, rows);
}
