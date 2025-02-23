#ifndef VISITOR_H
#define VISITOR_H

class ColumnNode;
class TableNode;
class ConditionNode;
class SelectNode;
class InsertNode;
class UpdateNode;
class DeleteNode;
class CreateNode;
class DropNode;
class AlterNode;
class JoinNode;

class Visitor {
public:
    virtual void visit(ColumnNode& node) = 0;
    virtual void visit(TableNode& node) = 0;
    virtual void visit(ConditionNode& node) = 0;
    virtual void visit(SelectNode& node) = 0;
    virtual void visit(InsertNode& node) = 0;
    virtual void visit(UpdateNode& node) = 0;
    virtual void visit(DeleteNode& node) = 0;
    virtual void visit(CreateNode& node) = 0;
    virtual void visit(DropNode& node) = 0;
    virtual void visit(AlterNode& node) = 0;
    virtual void visit(JoinNode& node) = 0;
};

#endif // VISITOR_H
