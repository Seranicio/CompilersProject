%X COMENTARIO
%X COMENTARIO_2
%X ERROR_ESCAPE
%X AUX_ERROR_ESCAPE

%{
#include <stdio.h>
#include <string.h>
#include "estruturas.h"
#include "table_builder.h"
#include "y.tab.h"

void send_line_col(int linha,int coluna,char * string);

int linha=1;
int temp_linha;
int coluna = 1;
int temp_coluna;
int flag=0;
int tree_flag=0;

char * aux_errors_yacc;

extern   Node *Program ;
%}

ID              ([a-z]|[A-Z]|$|_)($|_|[a-z]|[A-Z]|[0-9])*
DECLIT          [0-9]|([1-9]("_"|[0-9])*[0-9])
DECLIT2         [0-9]("_"|[0-9])*[0-9]

REALLIT         (([0-9]|{DECLIT2})"."(([0-9]|{DECLIT2})?((e"+"|e"-"|E"+"|E"-"|e|E)([0-9]|{DECLIT2}))?)?)|("."([0-9]|{DECLIT2})((e"+"|e"-"|E"+"|E"-"|e|E)([0-9]|{DECLIT2}))?)|(([0-9]|{DECLIT2})(e|E)("+"|"-")?([0-9]|{DECLIT2}))

ESCAPE          (\\f)|(\\n)|(\\r)|(\\t)|(\\\\)|(\\\")

STRLIT				  ([^\r\n\"\\]|{ESCAPE})*

BOOL            boolean
BOOLLIT         true|false
CLASS           class
DO              do
DOTLENGTH       "."length
DOUBLE          double
ELSE            else
IF              if
INT             int
PARSEINT        Integer"."parseInt
PRINT           System"."out"."println
PUBLIC          public
RETURN          return
STATIC          static
STRING          String
VOID            void
WHILE           while
OCURV           \(
CCURV           \)
OBRACE          \{
CBRACE          \}
OSQUARE         \[
CSQUARE         \]
AND             &&
OR              \|\|
LT              <
GT              >
EQ              ==
NEQ             !=
LEQ             <=
GEQ             >=
PLUS            \+
MINUS           -
STAR            \*
DIV             \/
MOD             %
NOT             !
ASSIGN          =
SEMI            ;
COMMA           ,
RESERVED        "null"|"System"|"Integer"|"++"|"--"|"abstract"|"continue"|"for"|"new"|"switch"|"assert"|"default"|"if"|"package"|"synchronized"|"boolean"|"do"|"goto"|"private"|"this"|"break"|"double"|"implements"|"protected"|"throw"|"byte"|"else"|"import"|"public"|"throws"|"case"|"enum"|"instanceof"|"transient"|"catch"|"extends"|"int"|"short"|"try"|"char"|"final"|"interface"|"static"|"void"|"class"|"finally"|"long"|"strictfp"|"volatile"|"const"|"float"|"native"|"super"|"while"


IGNORE_SPACE            " "+
IGNORE_NEWLINE          \n
IGNORE_TAB              \t
IGNORE_FORMFEED         \f
IGNORE_CARRIAGE_RETURN  \r

%%

\"{STRLIT}*\"                     {if(flag==1) printf("STRLIT(%s)\n",yytext);coluna += yyleng;if(flag==0) {yylval.string=(char*)strdup(yytext);return STRLIT;}}
\"                                {BEGIN ERROR_ESCAPE;temp_coluna = coluna; temp_linha = linha;coluna += yyleng;}
<ERROR_ESCAPE>\"                  {BEGIN 0;coluna += yyleng;}
<ERROR_ESCAPE>\\\n|\\\r|\\\r\n    {BEGIN 0;printf("Line %d, col %d: invalid escape sequence (\\)\n", linha, coluna);printf("Line %d, col %d: unterminated string literal\n", temp_linha, temp_coluna);coluna=1;linha+=1;}
<ERROR_ESCAPE>{STRLIT}            {coluna += yyleng;}
<ERROR_ESCAPE>\n|\r|\r\n	        {BEGIN 0;printf("Line %d, col %d: unterminated string literal\n", temp_linha, temp_coluna);coluna=1;linha+=1;}
<ERROR_ESCAPE>\\[^fnrt\\\"]       {printf("Line %d, col %d: invalid escape sequence (%s)\n", linha , coluna, yytext); coluna += yyleng;}
<ERROR_ESCAPE><<EOF>>			        {coluna+=yyleng;return 0;}
<ERROR_ESCAPE>.                   {coluna += yyleng;}


"/*"                              {BEGIN COMENTARIO;temp_coluna = coluna; temp_linha = linha ;coluna += yyleng;}
<COMENTARIO>\n|\r|\r\n            {coluna=1;linha+=1;}
<COMENTARIO>"*/"                  {BEGIN 0;coluna += yyleng;}
<COMENTARIO><<EOF>>               {BEGIN 0;printf("Line %d, col %d: unterminated comment\n", temp_linha, temp_coluna);coluna+=yyleng;return 0;}
<COMENTARIO>.                     {coluna += yyleng;}

"//"                              {BEGIN COMENTARIO_2;coluna += yyleng;}
<COMENTARIO_2>\n|\r|\r\n          {BEGIN 0;coluna=1;linha+=1;}
<COMENTARIO_2><<EOF>>			        {coluna+=yyleng;return 0;}
<COMENTARIO_2>.                   {coluna += yyleng;}


{BOOL}                            {if(flag==1) printf("BOOL\n");coluna += yyleng;if(flag==0) {return BOOL;}}
{BOOLLIT}                         {if(flag==1) printf("BOOLLIT(%s)\n",yytext);coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,(char*)strdup(yytext));return BOOLLIT;}}
{CLASS}                           {if(flag==1) printf("CLASS\n");coluna += yyleng;if(flag==0) {return CLASS;}}
{DO}                              {if(flag==1) printf("DO\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return DO;}}
{DOTLENGTH}                       {if(flag==1) printf("DOTLENGTH\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return DOTLENGTH;}}
{DOUBLE}                          {if(flag==1) printf("DOUBLE\n");coluna += yyleng;if(flag==0) {return DOUBLE;}}
{ELSE}                            {if(flag==1) printf("ELSE\n");coluna += yyleng;if(flag==0) {return ELSE;}}
{IF}                              {if(flag==1) printf("IF\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return IF;}}
{INT}                             {if(flag==1) printf("INT\n");coluna += yyleng;if(flag==0) {return INT;}}
{PARSEINT}                        {if(flag==1) printf("PARSEINT\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return PARSEINT;}}
{PRINT}                           {if(flag==1) printf("PRINT\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return PRINT;}}
{PUBLIC}                          {if(flag==1) printf("PUBLIC\n");coluna += yyleng;if(flag==0) {return PUBLIC;}}
{RETURN}                          {if(flag==1) printf("RETURN\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return RETURN;}}
{STATIC}                          {if(flag==1) printf("STATIC\n");coluna += yyleng;if(flag==0) {return STATIC;}}
{STRING}                          {if(flag==1) printf("STRING\n");coluna += yyleng;if(flag==0) {return STRING;}}
{VOID}                            {if(flag==1) printf("VOID\n");coluna += yyleng;if(flag==0) {return VOID;}}
{WHILE}                           {if(flag==1) printf("WHILE\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return WHILE;}}
{OCURV}                           {if(flag==1) printf("OCURV\n");coluna += yyleng;if(flag==0) {return OCURV;}}
{CCURV}                           {if(flag==1) printf("CCURV\n");coluna += yyleng;if(flag==0) {return CCURV;}}
{OBRACE}                          {if(flag==1) printf("OBRACE\n");coluna += yyleng;if(flag==0) {return OBRACE;}}
{CBRACE}                          {if(flag==1) printf("CBRACE\n");coluna += yyleng;if(flag==0) {return CBRACE;}}
{OSQUARE}                         {if(flag==1) printf("OSQUARE\n");coluna += yyleng;if(flag==0) {return OSQUARE;}}
{CSQUARE}                         {if(flag==1) printf("CSQUARE\n");coluna += yyleng;if(flag==0) {return CSQUARE;}}
{AND}                             {if(flag==1) printf("AND\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return AND;}}
{OR}                              {if(flag==1) printf("OR\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return OR;}}
{LT}                              {if(flag==1) printf("LT\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return LT;}}
{GT}                              {if(flag==1) printf("GT\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return GT;}}
{EQ}                              {if(flag==1) printf("EQ\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return EQ;}}
{NEQ}                             {if(flag==1) printf("NEQ\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return NEQ;}}
{LEQ}                             {if(flag==1) printf("LEQ\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return LEQ;}}
{GEQ}                             {if(flag==1) printf("GEQ\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return GEQ;}}
{PLUS}                            {if(flag==1) printf("PLUS\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return PLUS;}}
{MINUS}                           {if(flag==1) printf("MINUS\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return MINUS;}}
{STAR}                            {if(flag==1) printf("STAR\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return STAR;}}
{DIV}                             {if(flag==1) printf("DIV\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return DIV;}}
{MOD}                             {if(flag==1) printf("MOD\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return MOD;}}
{NOT}                             {if(flag==1) printf("NOT\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return NOT;}}
{ASSIGN}                          {if(flag==1) printf("ASSIGN\n");coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,NULL);return ASSIGN;}}
{SEMI}                            {if(flag==1) printf("SEMI\n");coluna += yyleng;if(flag==0) {return SEMI;}}
{RESERVED}                        {if(flag==1) printf("RESERVED(%s)\n",yytext);coluna += yyleng;if(flag==0) {yylval.string=(char*)strdup(yytext);return RESERVED;}}
{COMMA}                           {if(flag==1) printf("COMMA\n");coluna += yyleng;if(flag==0) {return COMMA;}}

{DECLIT}                                  {if(flag==1) printf("DECLIT(%s)\n",yytext);coluna += yyleng;if(flag==0){send_line_col(linha,coluna-yyleng,(char*)strdup(yytext));return DECLIT;}}
{REALLIT}                                 {if(flag==1) printf("REALLIT(%s)\n",yytext);coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,(char*)strdup(yytext));;return REALLIT;}}
{ID}                                      {if(flag==1) printf("ID(%s)\n",yytext);coluna += yyleng;if(flag==0) {send_line_col(linha,coluna-yyleng,(char*)strdup(yytext));return ID;}}

{IGNORE_CARRIAGE_RETURN}{IGNORE_NEWLINE}  {coluna=1;linha+=1;}
{IGNORE_NEWLINE}                          {coluna=1;linha+=1;}
{IGNORE_CARRIAGE_RETURN}                  {coluna=1;linha+=1;}

{IGNORE_SPACE}                            {coluna += yyleng;}
{IGNORE_TAB}                              {coluna += yyleng;}
{IGNORE_FORMFEED}                         {coluna += yyleng;}
<<EOF>>									                  {coluna+=yyleng;return 0;}

.				                                  {printf("Line %d, col %d: illegal character (%s)\n", linha , coluna, yytext); coluna += yyleng;}

%%

int main(int argc, char **argv)
{
  if(argv[1]!=NULL){
    if(strncmp(argv[1], "-l", 2) == 0) {
      flag = 1;
      yylex();
    }
    else if(strncmp(argv[1], "-1", 2) == 0){
      flag = 2;
      yylex();
    }
    else if(strncmp(argv[1], "-t", 2) == 0){
      tree_flag = 1;
      activate_build_flag();
      yyparse();
      if(tree_flag ==1){
        print_Program();
      }
    }
    else if(strncmp(argv[1], "-2", 2) == 0){
      yyparse();
    }
    else if(strncmp(argv[1], "-s", 2) == 0){
      //symbol table.
      tree_flag = 1;
      activate_build_flag();
      yyparse();
      if(tree_flag ==1){
        check_Program(Program);
        show_table();
        printf("\n");
        print_ASTnoted();
      }
    }
    return 0;
  }
  tree_flag = 1;
  activate_build_flag();
  yyparse();
  if(tree_flag ==1){
    check_Program(Program);
  }
  return 0;
}

void cancel_tree(){
  tree_flag=0;
}

int yywrap()
{
return 1;
}

void yyerror (const char *s) {
  printf ("Line %d, col %lu: %s: %s\n", linha, coluna-yyleng, s, yytext);
}

void send_line_col(int linha,int coluna,char * string){
  Line_col *new = (Line_col*) malloc (sizeof(Line_col));
  new->line = linha;
  new->col = coluna;
  new->string = string;

  yylval.linecol =  new;
}