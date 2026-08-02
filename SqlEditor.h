#pragma once

#include <QCompleter>
#include <QPlainTextEdit>
#include <QStringList>

// A QPlainTextEdit for raw SQL input that adds inline autocomplete: SQL
// keywords are always suggested, plus table/column names once the schema
// is known (see setSchemaWords). Popup opens automatically while typing
// an identifier, or on demand with Ctrl+Space.
class SqlEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit SqlEditor(QWidget* parent = nullptr);

    // Replaces the schema-derived part of the completion list (table and
    // column names). Built-in SQL keywords are kept regardless.
    void setSchemaWords(const QStringList& words);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;

private slots:
    void insertCompletion(const QString& completion);

private:
    QString textUnderCursor() const;
    void updateCompleterModel();

    QCompleter* m_completer = nullptr;
    QStringList m_keywords;
    QStringList m_schemaWords;
};
