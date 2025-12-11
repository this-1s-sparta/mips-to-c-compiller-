#ifndef AST_HH
#define AST_HH

#include <iostream>
#include <string>
#include <vector>

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual int evaluate() const = 0;
    virtual void print(int indent = 0) const = 0;
};

// ----------- Leaf Nodes -----------
class IntNode : public ASTNode {
public:
    int value;
    explicit IntNode(int v);
    int evaluate() const override;
    void print(int indent = 0) const override;
};

class IdNode : public ASTNode {
public:
    std::string name;
    explicit IdNode(const std::string& n);
    int evaluate() const override;
    void print(int indent = 0) const override;
};

// ----------- Binary Nodes -----------
#define DEFINE_BINARY_NODE(NAME) \
class NAME : public ASTNode { \
public: \
    ASTNode *left, *right; \
    NAME(ASTNode* l, ASTNode* r); \
    ~NAME(); \
    int evaluate() const override; \
    void print(int indent = 0) const override; \
};

DEFINE_BINARY_NODE(AddNode)
DEFINE_BINARY_NODE(SubNode)
DEFINE_BINARY_NODE(MulNode)
DEFINE_BINARY_NODE(DivNode)
DEFINE_BINARY_NODE(ModNode)
DEFINE_BINARY_NODE(EqNode)
DEFINE_BINARY_NODE(NeNode)
DEFINE_BINARY_NODE(GtNode)
DEFINE_BINARY_NODE(GeNode)
DEFINE_BINARY_NODE(LtNode)
DEFINE_BINARY_NODE(LeNode)
DEFINE_BINARY_NODE(AndNode)
DEFINE_BINARY_NODE(OrNode)
DEFINE_BINARY_NODE(AssignNode)
DEFINE_BINARY_NODE(IfElseNode)
DEFINE_BINARY_NODE(WhileNode)
DEFINE_BINARY_NODE(CallNode)
DEFINE_BINARY_NODE(FunctionNode)
DEFINE_BINARY_NODE(FunctionListNode)
DEFINE_BINARY_NODE(ParamsBodyNode)
DEFINE_BINARY_NODE(DeclNode)
DEFINE_BINARY_NODE(DeclAssignNode)
DEFINE_BINARY_NODE(VarsNode)
DEFINE_BINARY_NODE(VarsAssignNode)
DEFINE_BINARY_NODE(InitTailNode)
DEFINE_BINARY_NODE(DeclsNode)
DEFINE_BINARY_NODE(DeclListNode)
DEFINE_BINARY_NODE(StmtsNode)
DEFINE_BINARY_NODE(BranchesNode)
DEFINE_BINARY_NODE(BodyNode)
DEFINE_BINARY_NODE(ProgramNode)


// ----------- Unary Nodes -----------
#define DEFINE_UNARY_NODE(NAME) \
class NAME : public ASTNode { \
public: \
    ASTNode* child; \
    explicit NAME(ASTNode* c); \
    ~NAME(); \
    int evaluate() const override; \
    void print(int indent = 0) const override; \
};

DEFINE_UNARY_NODE(NotNode)
DEFINE_UNARY_NODE(UMinusNode)
DEFINE_UNARY_NODE(UPlusNode)
DEFINE_UNARY_NODE(ReturnNode)
DEFINE_UNARY_NODE(CondNode)
DEFINE_UNARY_NODE(BlockNode)
DEFINE_UNARY_NODE(ConstNode)


// ----------- Special Nodes -----------
class BreakNode : public ASTNode {
public:
    int evaluate() const override;
    void print(int indent = 0) const override;
};

// ----------- Factory Functions -----------
ASTNode* MkNode(const std::string& op, ASTNode* left = nullptr, ASTNode* right = nullptr);
ASTNode* MkLeaf(const std::string& type, const std::string& val);
ASTNode* MkLeaf(const std::string& type, int val);
ASTNode* VectorToNode(const std::vector<ASTNode*>* vec);
void print_ast(ASTNode* root);
int evaluateAST(ASTNode* root);


#endif // AST_HH

