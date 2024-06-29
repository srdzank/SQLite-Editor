#ifndef VISITOR_H
#define VISITOR_H

class SelectNode;
class ColumnNode;
class TableNode;
class ConditionNode;

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(SelectNode& node) = 0;
    virtual void visit(ColumnNode& node) = 0;
    virtual void visit(TableNode& node) = 0;
    virtual void visit(ConditionNode& node) = 0;
};

#endif // VISITOR_H
