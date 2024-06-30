#ifndef CTABLEVIEWWORKSPACE_H
#define CTABLEVIEWWORKSPACE_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <vector>
#include <string>

class CTableViewWorkspace : public QWidget
{
    Q_OBJECT

public:
    explicit CTableViewWorkspace(QWidget* parent = nullptr);
    void printResults(const std::vector<std::vector<std::string>>& results, const std::vector<std::string>& columnNames);

private:
    QTableWidget* resultTableWidget;
    QVBoxLayout* layout;
};

#endif // CTABLEVIEWWORKSPACE_H
