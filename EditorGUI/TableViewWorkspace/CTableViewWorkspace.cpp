#include "CTableViewWorkspace.h"
#include <QStringList>
#include <QTableWidgetItem>

CTableViewWorkspace::CTableViewWorkspace(QWidget* parent)
    : QWidget(parent)
{
    resultTableWidget = new QTableWidget(this);

    // Apply styles to the QTableWidget
    resultTableWidget->setStyleSheet(
        "QTableWidget {"
        "    border: 1px solid #ccc;"
        "    background-color: #f9f9f9;"
        "    gridline-color: #ddd;"
        //"    font-size: 14px;"  // increase font size for table content
        "}"
        "QHeaderView::section {"
        "    background-color: #d3d3d3;"  // light gray background for headers
        "    color: #333;"  // dark text for headers
        "    padding: 4px;"
        "    border: 1px solid #ccc;"
        //"    font-size: 14px;"  // increase font size for headers
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #555555;"  // dark gray background for selected items
        "    color: #fff;"  // white text for selected items
        "}"

    );

    layout = new QVBoxLayout(this);
    layout->addWidget(resultTableWidget);
    setLayout(layout);
}

void CTableViewWorkspace::printResults(const std::vector<std::vector<std::string>>& results, const std::vector<std::string>& columnNames)
{
    resultTableWidget->clear();
    resultTableWidget->setRowCount(results.size());

    if (!results.empty()) {
        resultTableWidget->setColumnCount(results[0].size());

        // Set the column names
        QStringList headers;
        for (const auto& columnName : columnNames) {
            headers << QString::fromStdString(columnName);
        }
        resultTableWidget->setHorizontalHeaderLabels(headers);
    }

    for (size_t row = 0; row < results.size(); ++row) {
        for (size_t col = 0; col < results[row].size(); ++col) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(results[row][col]));

            // Apply styles to each item
            item->setBackground(QColor("#f0f0f0"));
            item->setForeground(QColor("#333"));
            resultTableWidget->setItem(row, col, item);
        }
    }
}
