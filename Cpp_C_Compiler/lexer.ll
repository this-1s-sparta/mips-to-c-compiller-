%{
#include "parser.tab.hh"
#include <iostream>

#define N 8

int lexical_column = 1;
int id_or_keyword(char *s);

%}

%option noyywrap
%option nounput
%option yylineno

%%


"//"[^\n]*  		         { 
                              lexical_column = 1;
                           }

[0-9]+                     {  
                              lexical_column += yyleng;
    				               yylval.yint = atoi(yytext);
                              //TODO: check overflow?
    				               return(CONST_INT);
				               }
"+"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(PLUS);
                           }
"-"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(MINUS);
                           }
"*"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(MUL);
                           }
"/"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(DIV);
                           }
"%"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(MOD);
                           }


">"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(GT);
                           }
">="                       {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(GE);
                           }
"<"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(LT);
                           }
"<="                       {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(LE);
                           }
"=="                       {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(EQ);
                           }
"!="                       {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(NE);
                           }

"="                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(ASSIGN);
                           }

"!"                       {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(NOT);
                           }

"("                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(LPAREN);
                           }
")"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(RPAREN);
                           }

"{"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(LBRACE);
                           }
"}"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(RBRACE);
                           }
","                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(COMMA);
                           }
";"                        {
                              //printf("%s",yytext);
                              lexical_column += yyleng;
                              return(SEMICOLON);
                           }

"&&"                       {
                              //printf("%s", yytext);
                              lexical_column += yyleng;
                              return(AND);
                           }
"||"                       {
                              //printf("%s", yytext);
                              lexical_column += yyleng;
                              return(OR);
                           }

"true" 			            {
    				               //printf("%s", yytext);
                              lexical_column += yyleng;
    				               yylval.yint = 1;
    				               return(TRUE);
			                  }

"false" 		               {
    				               //printf("%s", yytext);
                              lexical_column += yyleng;
    				               yylval.yint = 0;
    				               return(FALSE);
			                  }

[a-zA-Z_][a-zA-Z0-9_]*     {  
                              //printf("%s\n",yytext);
                              lexical_column += yyleng;

                              int i;
                              i=id_or_keyword(yytext);
                              if(i==ID) 
                                 yylval.ystr = strdup(yytext); // here should be added to lexicografical table and reffed by ID
                              else 
                                 yylval.yint=i;
                              return(i);
                           }
\n                         {
                              lexical_column = 1;
                           }
[ \t\r\f\v]+               {
                              lexical_column += yyleng;
                           }
.                          {
                              fprintf(stderr, "Error : Unknown lexer %s l.%dc.%d\n",yytext, yylineno, lexical_column);
                              exit(-1);
                           } 

   

%%

// Specify const because why not
// remove True False
static const char *kid[N]={
   "if", "else",
   "int",
   "print","read",
   "while", "break", "return"};
static const int kval[N]={
   IF, ELSE,
   INT,
   PRINT,READ,
   WHILE, BREAK, RETURN};


int id_or_keyword(char *s)
{  
   size_t i;
   for(i = 0; (i < N) && (strcmp(s, kid[i]) != 0); i++);
   if(i < N)
      return kval[i];
   else
      return ID;
}