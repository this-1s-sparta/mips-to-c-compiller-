%require "3.8.2"

%code requires {
  #include <vector>
  #include "ast.hh"
  #include "generator.hh"
}

%{
#include <string>
#include <iostream>
#include <variant>
#include <cstdio>
#include <unordered_map>
#include "map.hh"
#include <fstream>
extern std::ofstream out;


extern std::unordered_map<std::string, int> symbolTable;

int yylex();
int yyparse();
extern FILE *yyin;
void yyerror(const char *s);

extern int yylineno;
extern int lexical_column;

int nextsymbol;
int current_region = 0;

%}

%union {
    int yint;
    char *ystr;
    ASTNode *yast;
    std::vector<ASTNode*>* yvec;
    const char *op;
}

%token <op> PLUS MINUS MUL DIV MOD ASSIGN
%token <op> EQ NE GT GE LT LE
%token <op> AND OR NOT

%token SEMICOLON COMMA
%token LPAREN RPAREN LBRACE RBRACE

%token IF ELSE
%token PRINT READ
%token WHILE BREAK
%token RETURN
%token INT

%token <yint> TRUE FALSE CONST_INT
%token <ystr> ID

%type <yast> program function_list function body decls decl decl_list vars stmts stmt block assignment location expression add_expression term unary factor type
%type <op> relop addop mulop
%type <yvec> params formals actuals arg_list

%left COMMA
%right ASSIGN
%left OR
%left AND
%left EQ NE
%left GT GE LT LE
%left PLUS MINUS
%left MUL DIV MOD
%right NOT UPLUS UMINUS
%left LPAREN

%%

program
    : function_list
    {
        $$ = MkNode("program", $1);
        print_ast($$);
        //
        generate_code($$);
        out << "RET     HLT" << "\n";
        out << "        END MAIN" << "\n";
        out.close();
        
    }
    ;

function_list
    : function_list function { $$ = MkNode("function_list", $1, $2); }
    | function { $$ = MkNode("function_list", $1); }
    ;

function
    : type ID LPAREN
        {
            // Δηλώνεις τη συνάρτηση στο global region
            Symbol sym(0, 1, nextsymbol++, std::string($2));
            if (!add_symbol(0, sym)) yyerror("Function already declared");

            // Δημιουργία νέου region πριν τα params
            current_region++;
        }
      params RPAREN body
    {
        ASTNode* params_node = VectorToNode($5); // Το $5 = params
        ASTNode* func_body = MkNode("params_body", params_node, $7);
        $$ = MkNode("function", MkLeaf("id", $2), func_body);
    }
    ;



params
    : formals type ID
    {
        Symbol sym(0, 2, nextsymbol++, std::string($3));
        if (!add_symbol(current_region, sym)) yyerror("Parameter already declared");

        if (!$1)
            $1 = new std::vector<ASTNode*>();

        $1->push_back(MkLeaf("id", $3));
        $$ = $1;
    }
    | {$$ = new std::vector<ASTNode*>();}
    ;


formals
    : formals type ID COMMA
    {
        Symbol sym(0, 2, nextsymbol++, std::string($3));
        if (!add_symbol(current_region, sym)) yyerror("Parameter already declared");

        if (!$1)
            $1 = new std::vector<ASTNode*>();

        $1->push_back(MkLeaf("id", $3));
        $$ = $1;
    }
    | {$$ = new std::vector<ASTNode*>();}
    ;


type
    : INT { $$ = nullptr; }
    ;

body
    : LBRACE decls stmts RBRACE
    {
        $$ = MkNode("body", $2, $3);
    }
    ;

decls
    : decl_list decl
    {
        if ($1 && $2)
            $$ = MkNode("decls", $1, $2);
        else if ($1)
            $$ = $1;
        else
            $$ = $2;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

decl_list
    : decl_list decl
    {
        if ($1 && $2)
            $$ = MkNode("decl_list", $1, $2);
        else if ($1)
            $$ = $1;
        else
            $$ = $2;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

decl
    : type ID vars SEMICOLON
    {
        Symbol sym(0, 0, nextsymbol++, $2);
        if (!add_symbol(current_region, sym)) {
            fprintf(stderr, "Error: redeclaration of variable '%s'\n", $2);
            exit(1);
        }
        $$ = MkNode("decl", MkLeaf("id", $2), $3);
    }
    | type ID ASSIGN expression vars SEMICOLON
    {
        Symbol sym(0, 0, nextsymbol++, $2);
        if (!add_symbol(current_region, sym)) {
            fprintf(stderr, "Error: redeclaration of variable '%s'\n", $2);
            exit(1);
        }
        ASTNode* id_node = MkLeaf("id", $2);
	ASTNode* right = MkNode("init_tail", $4, $5);
	$$ = MkNode("decl_assign", id_node, right);

    }
    ;

vars
    : COMMA ID vars
    {
        Symbol sym(0, 0, nextsymbol++, std::string($2));
        if (!add_symbol(current_region, sym)) yyerror("Variable already declared");
        $$ = MkNode("vars", MkLeaf("id", $2), $3);
    }
    | COMMA ID ASSIGN expression vars
    {
        Symbol sym(0, 0, nextsymbol++, std::string($2));
        if (!add_symbol(current_region, sym)) yyerror("Variable already declared");
        ASTNode* id_node = MkLeaf("id", $2);
	ASTNode* assign_tail = MkNode("init_tail", $4, $5);
	$$ = MkNode("vars_assign", id_node, assign_tail);
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

stmts
    : stmts stmt
    {
        if ($1 && $2)
            $$ = MkNode("stmts", $1, $2);
        else if ($1)
            $$ = $1;
        else
            $$ = $2;
    }
    | /* empty */
    {
        $$ = nullptr;
    }
    ;

stmt
    : assignment SEMICOLON { $$ = $1; }
    | RETURN expression SEMICOLON { $$ = MkNode("return", $2); }
    | IF LPAREN expression RPAREN stmt ELSE stmt { 
    	ASTNode* cond = MkNode("cond", $3);
    	ASTNode* branches = MkNode("branches", $5, $7);
    	$$ = MkNode("ifelse", cond, branches);
    }
    | WHILE LPAREN expression RPAREN stmt { $$ = MkNode("while", $3, $5); }
    | BREAK SEMICOLON { $$ = MkNode("break"); }
    | block { $$ = $1; }
    | SEMICOLON { $$ = nullptr; }
    ;

block
    : LBRACE stmts RBRACE { $$ = $2; }
    ;

assignment
    : location ASSIGN expression { $$ = MkNode("assign", $1, $3); }
    ;

location
    : ID { $$ = MkLeaf("id", $1); }
    ;

expression
    : add_expression relop add_expression { $$ = MkNode($2, $1, $3); }
    | add_expression { $$ = $1; }
    ;

relop
    : LE { $$ = "<="; }
    | LT { $$ = "<"; }
    | GT { $$ = ">"; }
    | GE { $$ = ">="; }
    | EQ { $$ = "=="; }
    | NE { $$ = "!="; }
    ;

add_expression
    : add_expression addop term { $$ = MkNode($2, $1, $3); }
    | term { $$ = $1; }
    ;

addop
    : PLUS { $$ = "+"; }
    | MINUS { $$ = "-"; }
    ;

term
    : term mulop unary { $$ = MkNode($2, $1, $3); }
    | unary { $$ = $1; }
    ;

mulop
    : MUL { $$ = "*"; }
    | DIV { $$ = "/"; }
    ;

unary
    : MINUS factor %prec UMINUS { $$ = MkNode("uminus", $2); }
    | PLUS factor %prec UPLUS { $$ = MkNode("uplus", $2); }
    | factor { $$ = $1; }
    ;

factor
    : LPAREN expression RPAREN { $$ = $2; }
    | location { $$ = $1; }
    | CONST_INT { $$ = MkLeaf("const", $1); }
    | TRUE { $$ = MkLeaf("bool", 1); }
    | FALSE { $$ = MkLeaf("bool", 0); }
    | ID LPAREN actuals RPAREN {
    ASTNode* args_node = VectorToNode($3);      // Μετατροπή vector σε ASTNode*
    $$ = MkNode("call", MkLeaf("id", $1), args_node);
}


actuals
    : arg_list { $$ = $1; }
    | { $$ = new std::vector<ASTNode*>(); }
    ;

arg_list
    : expression
    {
        $$ = new std::vector<ASTNode*>();
        $$->push_back($1);
    }
    | expression COMMA arg_list
    {
        $$ = $3;
        $$->insert($$->begin(), $1);
    }
    ;

%%

int main() {
#ifdef DEBUG
    extern int yydebug;
    yydebug = 1;
#endif
    std::remove("out.mix");
    yyparse();

    for (const auto& [region_id, _] : regions_map) {
        print_region(region_id);
    }
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s l.%d c.%d\n", s, yylineno, lexical_column);
    exit(-1);
}

