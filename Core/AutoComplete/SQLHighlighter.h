#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class SQLHighlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    SQLHighlighter(QTextDocument* parent)
        : QSyntaxHighlighter(parent) {
        keywordFormat.setForeground(Qt::blue); // Set the color for SQL keywords
        keywordFormat.setFontWeight(QFont::Bold);

        // Regex pattern for SQL keywords
        keywordPatterns = QStringList()
            << "\\bSELECT\\b" << "\\bFROM\\b" << "\\bWHERE\\b" << "\\bINSERT\\b"
            << "\\bUPDATE\\b" << "\\bDELETE\\b" << "\\bCREATE\\b" << "\\bDROP\\b"
            << "\\bALTER\\b" << "\\bTABLE\\b" << "\\bINDEX\\b" << "\\bVIEW\\b"
            << "\\bTRIGGER\\b" << "\\bIF\\b" << "\\bEXISTS\\b" << "\\bNOT\\b"
            << "\\bNULL\\b" << "\\bDEFAULT\\b" << "\\bPRIMARY\\b" << "\\bKEY\\b"
            << "\\bFOREIGN\\b" << "\\bREFERENCES\\b" << "\\bCHECK\\b" << "\\bUNIQUE\\b"
            << "\\bAND\\b" << "\\bOR\\b" << "\\bIN\\b" << "\\bLIKE\\b" << "\\bLIMIT\\b"
            << "\\bORDER\\b" << "\\bBY\\b" << "\\bGROUP\\b" << "\\bHAVING\\b" << "\\bJOIN\\b"
            << "\\bINNER\\b" << "\\bLEFT\\b" << "\\bRIGHT\\b" << "\\bFULL\\b" << "\\bOUTER\\b"
            << "\\bON\\b" << "\\bAS\\b" << "\\bDISTINCT\\b" << "\\bIS\\b" << "\\bBETWEEN\\b"
            << "\\bVALUES\\b" << "\\bINTO\\b" << "\\bSET\\b";
    }

protected:
    void highlightBlock(const QString& text) override {
        foreach(const QString & pattern, keywordPatterns) {
            QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
            QRegularExpressionMatchIterator it = regex.globalMatch(text);
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);

                // Convert SQL keywords to uppercase
                QTextCursor cursor(document());
                cursor.setPosition(currentBlock().position() + match.capturedStart());
                cursor.setPosition(currentBlock().position() + match.capturedStart() + match.capturedLength(), QTextCursor::KeepAnchor);
                cursor.insertText(match.captured().toUpper());
            }
        }
    }

private:
    QTextCharFormat keywordFormat;
    QStringList keywordPatterns;
};

#endif // SQLHIGHLIGHTER_H
