#ifndef AUTOCOMPLETETEXTEDIT_H
#define AUTOCOMPLETETEXTEDIT_H

#include <QTextEdit>
#include <QCompleter>
#include "SQLHighlighter.h"

class AutoCompleteTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit AutoCompleteTextEdit(QWidget* parent = nullptr);
    void setCompleter(QCompleter* completer);
    QCompleter* completer() const;

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void focusInEvent(QFocusEvent* e) override;

private slots:
    void insertCompletion(const QString& completion);

private:
    QString textUnderCursor() const;

private:
    QCompleter* c;
    SQLHighlighter* highlighter;
};

#endif // AUTOCOMPLETETEXTEDIT_H
