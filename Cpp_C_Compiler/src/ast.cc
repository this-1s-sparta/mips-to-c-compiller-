#include "ast.hh"
#include <iomanip>
#include <fstream>
#include <string>

IntNode::IntNode(int v) : value(v) {}
int IntNode::evaluate() const { return value; }
void IntNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Int(" << value << ")\n";
}

IdNode::IdNode(const std::string& n) : name(n) {}
int IdNode::evaluate() const { return 0; }
void IdNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Id(" << name << ")\n";
}

// Binary Nodes
#define DEFINE_BINARY_NODE_IMPL(NAME, SYMBOL) \
NAME::NAME(ASTNode* l, ASTNode* r) : left(l), right(r) {} \
NAME::~NAME() { delete left; delete right; } \
int NAME::evaluate() const { return 0; } \
void NAME::print(int indent) const { \
    std::cout << std::string(indent, ' ') << #NAME << "\n"; \
    if (left) left->print(indent + 2); \
    if (right) right->print(indent + 2); \
}

DEFINE_BINARY_NODE_IMPL(AddNode, "+")
DEFINE_BINARY_NODE_IMPL(SubNode, "-")
DEFINE_BINARY_NODE_IMPL(MulNode, "*")
DEFINE_BINARY_NODE_IMPL(DivNode, "/")
DEFINE_BINARY_NODE_IMPL(ModNode, "%")
DEFINE_BINARY_NODE_IMPL(EqNode, "==")
DEFINE_BINARY_NODE_IMPL(NeNode, "!=")
DEFINE_BINARY_NODE_IMPL(GtNode, ">")
DEFINE_BINARY_NODE_IMPL(GeNode, ">=")
DEFINE_BINARY_NODE_IMPL(LtNode, "<")
DEFINE_BINARY_NODE_IMPL(LeNode, "<=")
DEFINE_BINARY_NODE_IMPL(AssignNode, "assign")
DEFINE_BINARY_NODE_IMPL(IfElseNode, "if")
DEFINE_BINARY_NODE_IMPL(WhileNode, "while")
DEFINE_BINARY_NODE_IMPL(CallNode, "call")
DEFINE_BINARY_NODE_IMPL(FunctionNode, "function")
DEFINE_BINARY_NODE_IMPL(FunctionListNode, "functions")
DEFINE_BINARY_NODE_IMPL(ParamsBodyNode, "params_body")
DEFINE_BINARY_NODE_IMPL(DeclNode, "decl")
DEFINE_BINARY_NODE_IMPL(DeclAssignNode, "decl_assign")
DEFINE_BINARY_NODE_IMPL(VarsNode, "vars")
DEFINE_BINARY_NODE_IMPL(VarsAssignNode, "vars_assign")
DEFINE_BINARY_NODE_IMPL(InitTailNode, "init_tail")
DEFINE_BINARY_NODE_IMPL(DeclsNode, "decls")
DEFINE_BINARY_NODE_IMPL(DeclListNode, "decl_list")
DEFINE_BINARY_NODE_IMPL(StmtsNode, "stmts")
DEFINE_BINARY_NODE_IMPL(BranchesNode, "branches")
DEFINE_BINARY_NODE_IMPL(BodyNode, "body")
DEFINE_BINARY_NODE_IMPL(ProgramNode,"program")
//for future development 
DEFINE_BINARY_NODE_IMPL(AndNode, "&&")
DEFINE_BINARY_NODE_IMPL(OrNode, "||")

// Unary Nodes
#define DEFINE_UNARY_NODE_IMPL(NAME) \
NAME::NAME(ASTNode* c) : child(c) {} \
NAME::~NAME() { delete child; } \
int NAME::evaluate() const { return 0; } \
void NAME::print(int indent) const { \
    std::cout << std::string(indent, ' ') << #NAME << "\n"; \
    if (child) child->print(indent + 2); \
}

DEFINE_UNARY_NODE_IMPL(ReturnNode)
DEFINE_UNARY_NODE_IMPL(CondNode)
DEFINE_UNARY_NODE_IMPL(BlockNode)
DEFINE_UNARY_NODE_IMPL(ConstNode)
//for future development 
DEFINE_UNARY_NODE_IMPL(NotNode)
DEFINE_UNARY_NODE_IMPL(UMinusNode)
DEFINE_UNARY_NODE_IMPL(UPlusNode)

// break
int BreakNode::evaluate() const { return 0; }
void BreakNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "Break\n";
}

// Utility Functions
void print_ast(ASTNode* root) {
    if (root) root->print();
}

int evaluateAST(ASTNode* root) {
    return root ? root->evaluate() : 0;
}

// based on operation names
ASTNode* MkNode(const std::string& op, ASTNode* left, ASTNode* right) {
    if (op == "+") return new AddNode(left, right);
    else if (op == "-") return new SubNode(left, right);
    else if (op == "*") return new MulNode(left, right);
    else if (op == "/") return new DivNode(left, right);
    else if (op == "%") return new ModNode(left, right);
    else if (op == "==") return new EqNode(left, right);
    else if (op == "!=") return new NeNode(left, right);
    else if (op == ">") return new GtNode(left, right);
    else if (op == ">=") return new GeNode(left, right);
    else if (op == "<") return new LtNode(left, right);
    else if (op == "<=") return new LeNode(left, right);
    else if (op == "assign") return new AssignNode(left, right);
    else if (op == "if" || op == "cond" || op == "ifelse") return new IfElseNode(left, right);
    else if (op == "while") return new WhileNode(left, right);
    else if (op == "call") return new CallNode(left, right);
    else if (op == "function") return new FunctionNode(left, right);
    else if (op == "functions" || op == "function_list") return new FunctionListNode(left, right);
    else if (op == "params_body") return new ParamsBodyNode(left, right);
    else if (op == "decl") return new DeclNode(left, right);
    else if (op == "decl_assign") return new DeclAssignNode(left, right);
    else if (op == "vars") return new VarsNode(left, right);
    else if (op == "vars_assign") return new VarsAssignNode(left, right);
    else if (op == "init_tail") return new InitTailNode(left, right);
    else if (op == "decls") return new DeclsNode(left, right);
    else if (op == "decl_list") return new DeclListNode(left, right);
    else if (op == "stmts") return new StmtsNode(left, right);
    else if (op == "branches") return new BranchesNode(left, right);
    else if (op == "body") return new BodyNode(left, right);
    else if (op == "return") return new ReturnNode(left);
    else if (op == "const") return new ConstNode(left);
    else if (op == "program") return new ProgramNode(left, right);
    //for future development
    else if (op == "&&") return new AndNode(left, right);
    else if (op == "||") return new OrNode(left, right);
    else {
          throw std::runtime_error("MkNode: Unknown op " + op); //for testing 
    }
}

//creates a leaf (int)
ASTNode* MkLeaf(const std::string& type, int val) {
    if (type == "int" || type == "const") return new IntNode(val);
    else throw std::runtime_error("MkLeaf: Unknown int type " + type);
}

//creates a leaf (string) 
ASTNode* MkLeaf(const std::string& type, const std::string& val) {
    if (type == "id") return new IdNode(val);
    else throw std::runtime_error("MkLeaf: Unknown string type " + type);
}

//convert from vector to ast
ASTNode* VectorToNode(const std::vector<ASTNode*>* vec) {
    if (!vec || vec->empty()) return nullptr;
    if (vec->size() == 1) return (*vec)[0];
    ASTNode* root = (*vec)[0];
    for (size_t i = 1; i < vec->size(); ++i) {
        root = new StmtsNode(root, (*vec)[i]);
    }
    return root;
}

