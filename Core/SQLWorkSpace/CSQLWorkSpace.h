#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

class CSQLWorkSpace : public QWidget
{
    Q_OBJECT

public:
    CSQLWorkSpace(QWidget* parent = nullptr);
    ~CSQLWorkSpace();

private slots:
    void onExecuteClicked();
    void onClearClicked();

private:
    QTextEdit* sqlTextEdit;
    QPushButton* executeButton;
    QPushButton* clearButton;
};
