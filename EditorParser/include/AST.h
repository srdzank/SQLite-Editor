#ifndef AST_H
#define AST_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>

class Visitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
    virtual void print(int indent) const = 0;
};

class ColumnNode : public ASTNode {
public:
    std::string name;

    ColumnNode(const std::string& name) : name(name) {}
    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class TableNode : public ASTNode {
public:
    std::string name;

    TableNode(const std::string& name) : name(name) {}
    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class ConditionNode : public ASTNode {
public:
    std::string left, op, right;

    ConditionNode(const std::string& left, const std::string& op, const std::string& right)
        : left(left), op(op), right(right) {}
    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class SelectNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ColumnNode>> columns;
    std::shared_ptr<TableNode> table;
    std::shared_ptr<ConditionNode> whereClause;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class InsertNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;
    std::vector<std::shared_ptr<ColumnNode>> columns;
    std::vector<std::string> values;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class UpdateNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;
    std::vector<std::shared_ptr<ColumnNode>> columns;
    std::vector<std::string> values;
    std::shared_ptr<ConditionNode> whereClause;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class DeleteNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;
    std::shared_ptr<ConditionNode> whereClause;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class CreateNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class DropNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class AlterNode : public ASTNode {
public:
    std::shared_ptr<TableNode> table;
    std::string alteration;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

class JoinNode : public ASTNode {
public:
    std::shared_ptr<TableNode> leftTable;
    std::shared_ptr<TableNode> rightTable;
    std::shared_ptr<ConditionNode> condition;
    std::string joinType;

    void accept(Visitor& v) override;
    void print(int indent) const override;
};

#endif // AST_H
