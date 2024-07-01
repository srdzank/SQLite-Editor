#include "AST.h"
#include "Visitor.h"

void ColumnNode::accept(Visitor& v) {
    v.visit(*this);
}

void TableNode::accept(Visitor& v) {
    v.visit(*this);
}

void ConditionNode::accept(Visitor& v) {
    v.visit(*this);
}

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

void InsertNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
    for (auto& column : columns) {
        column->accept(v);
    }
}

void UpdateNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
    for (auto& column : columns) {
        column->accept(v);
    }
    if (whereClause) {
        whereClause->accept(v);
    }
}

void DeleteNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
    if (whereClause) {
        whereClause->accept(v);
    }
}

void CreateNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
}

void DropNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
}

void AlterNode::accept(Visitor& v) {
    v.visit(*this);
    table->accept(v);
}

void JoinNode::accept(Visitor& v) {
    v.visit(*this);
    leftTable->accept(v);
    rightTable->accept(v);
    if (condition) {
        condition->accept(v);
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

void InsertNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "INSERT INTO" << std::endl;
    table->print(indent + 1);
    for (auto& column : columns) {
        column->print(indent + 1);
    }
    for (const auto& value : values) {
        for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
        std::cout << "Value: " << value << std::endl;
    }
}

void UpdateNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "UPDATE" << std::endl;
    table->print(indent + 1);
    for (auto& column : columns) {
        column->print(indent + 1);
    }
    for (const auto& value : values) {
        for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
        std::cout << "Value: " << value << std::endl;
    }
    if (whereClause) {
        whereClause->print(indent + 1);
    }
}

void DeleteNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "DELETE FROM" << std::endl;
    table->print(indent + 1);
    if (whereClause) {
        whereClause->print(indent + 1);
    }
}

void CreateNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "CREATE TABLE" << std::endl;
    table->print(indent + 1);
}

void DropNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "DROP TABLE" << std::endl;
    table->print(indent + 1);
}

void AlterNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << "ALTER TABLE" << std::endl;
    table->print(indent + 1);
    for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
    std::cout << "Alteration: " << alteration << std::endl;
}

void JoinNode::print(int indent) const {
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << joinType << " JOIN" << std::endl;
    leftTable->print(indent + 1);
    rightTable->print(indent + 1);
    if (condition) {
        condition->print(indent + 1);
    }
}
