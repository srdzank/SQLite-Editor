#ifndef CONCRETEVISITOR_H
#define CONCRETEVISITOR_H

#include "Visitor.h"
#include <iostream>
#include "../sqlite3/include/sqlite3.h"
#include <vector>
#include <string>

class ConcreteVisitor : public Visitor {
public:
    explicit ConcreteVisitor(sqlite3* db) : db(db) {}

    void visit(SelectNode& node) override {
        std::cout << "Visiting Select Node" << std::endl;
        std::string sql = "SELECT ";
        for (size_t i = 0; i < node.columns.size(); ++i) {
            if (i > 0) sql += ", ";
            ColumnNode* column = dynamic_cast<ColumnNode*>(node.columns[i].get());
            sql += column->name;
        }
        sql += " FROM ";
        TableNode* table = dynamic_cast<TableNode*>(node.table.get());
        sql += table->name;
        if (node.whereClause) {
            ConditionNode* condition = dynamic_cast<ConditionNode*>(node.whereClause.get());
            sql += " WHERE " + condition->left + " " + condition->op + " " + condition->right;
        }
        sql += ";";

        executeSQL(sql);
    }

    void visit(ColumnNode& node) override {
        std::cout << "Visiting Column Node: " << node.name << std::endl;
    }

    void visit(TableNode& node) override {
        std::cout << "Visiting Table Node: " << node.name << std::endl;
    }

    void visit(ConditionNode& node) override {
        std::cout << "Visiting Condition Node: " << node.left << " " << node.op << " " << node.right << std::endl;
    }

    const std::vector<std::vector<std::string>>& getResults() const {
        return results;
    }

private:
    sqlite3* db;
    std::vector<std::vector<std::string>> results;

    void executeSQL(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), callback, this, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    static int callback(void* data, int argc, char** argv, char** azColName) {
        ConcreteVisitor* self = static_cast<ConcreteVisitor*>(data);
        std::vector<std::string> row;
        for (int i = 0; i < argc; i++) {
            row.push_back(argv[i] ? argv[i] : "NULL");
        }
        self->results.push_back(row);
        return 0;
    }
};

#endif // CONCRETEVISITOR_H
