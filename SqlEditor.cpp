#include "SqlEditor.h"

#include <QAbstractItemView>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextCursor>

namespace {

// A practical subset of SQL keywords - enough to be useful for the kind
// of SELECT/JOIN/WHERE queries this app generates and lets users tweak.
const QStringList kSqlKeywords = {
    "SELECT", "FROM", "WHERE", "JOIN", "INNER", "LEFT", "RIGHT", "OUTER",
    "ON", "AND", "OR", "NOT", "IN", "LIKE", "IS", "NULL", "AS", "DISTINCT",
    "GROUP", "BY", "ORDER", "HAVING", "LIMIT", "OFFSET", "ASC", "DESC",
    "INSERT", "INTO", "VALUES", "UPDATE", "SET", "DELETE", "CREATE",
    "TABLE", "COUNT", "SUM", "AVG", "MIN", "MAX", "UNION", "ALL", "EXISTS",
    "BETWEEN", "CASE", "WHEN", "THEN", "ELSE", "END"
};

// Characters that end an identifier - the popup closes once one is typed.
const QString kWordBoundary = "~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-= \t\n";

} // namespace

SqlEditor::SqlEditor(QWidget* parent)
    : QPlainTextEdit(parent)
    , m_keywords(kSqlKeywords)
{
    setTabChangesFocus(false);

    m_completer = new QCompleter(this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setWidget(this);
    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated),
            this, &SqlEditor::insertCompletion);

    updateCompleterModel();
}

void SqlEditor::setSchemaWords(const QStringList& words)
{
    m_schemaWords = words;
    m_schemaWords.removeDuplicates();
    updateCompleterModel();
}

void SqlEditor::updateCompleterModel()
{
    QStringList all = m_keywords + m_schemaWords;
    all.removeDuplicates();
    m_completer->setModel(new QStringListModel(all, m_completer));
}

QString SqlEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void SqlEditor::insertCompletion(const QString& completion)
{
    if (m_completer->widget() != this) {
        return;
    }
    QTextCursor tc = textCursor();
    const int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

void SqlEditor::focusInEvent(QFocusEvent* event)
{
    m_completer->setWidget(this);
    QPlainTextEdit::focusInEvent(event);
}

void SqlEditor::keyPressEvent(QKeyEvent* event)
{
    if (m_completer->popup()->isVisible()) {
        // Let these keys drive the popup instead of the editor.
        switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            event->ignore();
            return;
        default:
            break;
        }
    }

    // Ctrl+Space always forces the popup open, even with an empty prefix.
    const bool isShortcut = event->modifiers().testFlag(Qt::ControlModifier)
                             && event->key() == Qt::Key_Space;
    if (!isShortcut) {
        QPlainTextEdit::keyPressEvent(event);
    }

    const bool ctrlOrShift = event->modifiers().testFlag(Qt::ControlModifier)
                              || event->modifiers().testFlag(Qt::ShiftModifier);
    if (ctrlOrShift && event->text().isEmpty() && !isShortcut) {
        return;
    }

    const bool hasOtherModifier = event->modifiers() != Qt::NoModifier && !ctrlOrShift;
    const QString prefix = textUnderCursor();

    if (!isShortcut
        && (hasOtherModifier || event->text().isEmpty() || prefix.length() < 2
            || (!event->text().isEmpty() && kWordBoundary.contains(event->text().right(1))))) {
        m_completer->popup()->hide();
        return;
    }

    if (prefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(prefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }

    QRect popupRect = cursorRect();
    popupRect.setWidth(m_completer->popup()->sizeHintForColumn(0)
                        + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(popupRect);
}
