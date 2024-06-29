#include "AST.h"
#include "Visitor.h"

void SelectNode::accept(Visitor& v) {
    v.visit(*this);
    for (auto& column : columns) {
        column->accept(v);
    }
    table->accept(v);
    if (whereClause) {
        whereClause->accept(v);
    }
}

void ColumnNode::accept(Visitor& v) {
    v.visit(*this);
}

void TableNode::accept(Visitor& v) {
    v.visit(*this);
}

void ConditionNode::accept(Visitor& v) {
    v.visit(*this);
}

void SelectNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "SELECT" << std::endl;
    for (auto& column : columns) {
        column->print(indent + 1);
    }
    table->print(indent + 1);
    if (whereClause) {
        whereClause->print(indent + 1);
    }
}

void ColumnNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Column: " << name << std::endl;
}

void TableNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Table: " << name << std::endl;
}

void ConditionNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "Condition: " << left << " " << op << " " << right << std::endl;
}
