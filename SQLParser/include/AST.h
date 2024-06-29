#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>

class Visitor;

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(Visitor& v) = 0;
    virtual void print(int indent = 0) const = 0;
};

class SelectNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> columns;
    std::shared_ptr<ASTNode> table;
    std::shared_ptr<ASTNode> whereClause;

    void accept(Visitor& v) override;
    void print(int indent = 0) const override;
};

class ColumnNode : public ASTNode {
public:
    std::string name;

    explicit ColumnNode(const std::string& name) : name(name) {}

    void accept(Visitor& v) override;
    void print(int indent = 0) const override;
};

class TableNode : public ASTNode {
public:
    std::string name;

    explicit TableNode(const std::string& name) : name(name) {}

    void accept(Visitor& v) override;
    void print(int indent = 0) const override;
};

class ConditionNode : public ASTNode {
public:
    std::string left;
    std::string op;
    std::string right;

    ConditionNode(const std::string& left, const std::string& op, const std::string& right)
        : left(left), op(op), right(right) {}

    void accept(Visitor& v) override;
    void print(int indent = 0) const override;
};

#endif // AST_H
