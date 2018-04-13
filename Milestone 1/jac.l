%X COMENTARIO
%X COMENTARIO_2
%X ERROR_ESCAPE
%X AUX_ERROR_ESCAPE

%{
#include <stdio.h>
#include <string.h>

int linha=1;
int temp_linha;
int coluna = 1;
int temp_coluna;
int flag = 0;
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
RESERVED        "null"|"System"|"Integer"|"++"|"--"|"abstract"|"continue"|"for"|"new"|"switch"|"assert"|"default"|"if"|"package"|"synchronized"|"boolean"|"do"|"goto"|"private"|"this"|"break"|"double"|"implements"|"protected"|"throw"|"byte"|"else"|"import"|"public"|"throws"|"case"|"enum"|"instanceof"|"return"|"transient"|"catch"|"extends"|"int"|"short"|"try"|"char"|"final"|"interface"|"static"|"void"|"class"|"finally"|"long"|"strictfp"|"volatile"|"const"|"float"|"native"|"super"|"while"


IGNORE_SPACE            " "+
IGNORE_NEWLINE          \n
IGNORE_TAB              \t
IGNORE_FORMFEED         \f
IGNORE_CARRIAGE_RETURN  \r

%%

\"{STRLIT}*\"                      {if(flag==1) printf("STRLIT(%s)\n",yytext);coluna += yyleng;}
\"                                {BEGIN ERROR_ESCAPE;temp_coluna = coluna; temp_linha = linha;coluna += yyleng;}
<ERROR_ESCAPE>\"                  {BEGIN 0;coluna += yyleng;}
<ERROR_ESCAPE>\\\n|\\\r|\\\r\n    {BEGIN 0;printf("Line %d, col %d: invalid escape sequence (\\)\n", linha, coluna);printf("Line %d, col %d: unterminated string literal\n", temp_linha, temp_coluna);coluna=1;linha+=1;}
<ERROR_ESCAPE>{STRLIT}            {coluna += yyleng;}
<ERROR_ESCAPE>\n|\r|\r\n	        {BEGIN 0;printf("Line %d, col %d: unterminated string literal\n", temp_linha, temp_coluna); coluna=1;linha+=1;}
<ERROR_ESCAPE>\\[^fnrt\\\"]       {printf("Line %d, col %d: invalid escape sequence (%s)\n", linha , coluna, yytext); coluna += yyleng;}
<ERROR_ESCAPE>.                   {coluna += yyleng;}


"/*"                              {BEGIN COMENTARIO;temp_coluna = coluna; temp_linha = linha ;coluna += yyleng;}
<COMENTARIO>\n|\r|\r\n            {coluna=1;linha+=1;}
<COMENTARIO>"*/"                  {BEGIN 0;coluna += yyleng;}
<COMENTARIO><<EOF>>               {BEGIN 0;printf("Line %d, col %d: unterminated comment\n", temp_linha, temp_coluna); coluna+=yyleng; return 0;}
<COMENTARIO>.                     {coluna += yyleng;}

"//"                              {BEGIN COMENTARIO_2;coluna += yyleng;}
<COMENTARIO_2>\n|\r|\r\n          {BEGIN 0;coluna=1;linha+=1;}
<COMENTARIO_2>.                   {coluna += yyleng;}


{BOOL}                            {if(flag==1) printf("BOOL\n");coluna += yyleng;}
{BOOLLIT}                         {if(flag==1) printf("BOOLLIT(%s)\n",yytext);coluna += yyleng;}
{CLASS}                           {if(flag==1) printf("CLASS\n");coluna += yyleng;}
{DO}                              {if(flag==1) printf("DO\n");coluna += yyleng;}
{DOTLENGTH}                       {if(flag==1) printf("DOTLENGTH\n");coluna += yyleng;}
{DOUBLE}                          {if(flag==1) printf("DOUBLE\n");coluna += yyleng;}
{ELSE}                            {if(flag==1) printf("ELSE\n");coluna += yyleng;}
{IF}                              {if(flag==1) printf("IF\n");coluna += yyleng;}
{INT}                             {if(flag==1) printf("INT\n");coluna += yyleng;}
{PARSEINT}                        {if(flag==1) printf("PARSEINT\n");coluna += yyleng;}
{PRINT}                           {if(flag==1) printf("PRINT\n");coluna += yyleng;}
{PUBLIC}                          {if(flag==1) printf("PUBLIC\n");coluna += yyleng;}
{RETURN}                          {if(flag==1) printf("RETURN\n");coluna += yyleng;}
{STATIC}                          {if(flag==1) printf("STATIC\n");coluna += yyleng;}
{STRING}                          {if(flag==1) printf("STRING\n");coluna += yyleng;}
{VOID}                            {if(flag==1) printf("VOID\n");coluna += yyleng;}
{WHILE}                           {if(flag==1) printf("WHILE\n");coluna += yyleng;}
{OCURV}                           {if(flag==1) printf("OCURV\n");coluna += yyleng;}
{CCURV}                           {if(flag==1) printf("CCURV\n");coluna += yyleng;}
{OBRACE}                          {if(flag==1) printf("OBRACE\n");coluna += yyleng;}
{CBRACE}                          {if(flag==1) printf("CBRACE\n");coluna += yyleng;}
{OSQUARE}                         {if(flag==1) printf("OSQUARE\n");coluna += yyleng;}
{CSQUARE}                         {if(flag==1) printf("CSQUARE\n");coluna += yyleng;}
{AND}                             {if(flag==1) printf("AND\n");coluna += yyleng;}
{OR}                              {if(flag==1) printf("OR\n");coluna += yyleng;}
{LT}                              {if(flag==1) printf("LT\n");coluna += yyleng;}
{GT}                              {if(flag==1) printf("GT\n");coluna += yyleng;}
{EQ}                              {if(flag==1) printf("EQ\n");coluna += yyleng;}
{NEQ}                             {if(flag==1) printf("NEQ\n");coluna += yyleng;}
{LEQ}                             {if(flag==1) printf("LEQ\n");coluna += yyleng;}
{GEQ}                             {if(flag==1) printf("GEQ\n");coluna += yyleng;}
{PLUS}                            {if(flag==1) printf("PLUS\n");coluna += yyleng;}
{MINUS}                           {if(flag==1) printf("MINUS\n");coluna += yyleng;}
{STAR}                            {if(flag==1) printf("STAR\n");coluna += yyleng;}
{DIV}                             {if(flag==1) printf("DIV\n");coluna += yyleng;}
{MOD}                             {if(flag==1) printf("MOD\n");coluna += yyleng;}
{NOT}                             {if(flag==1) printf("NOT\n");coluna += yyleng;}
{ASSIGN}                          {if(flag==1) printf("ASSIGN\n");coluna += yyleng;}
{SEMI}                            {if(flag==1) printf("SEMI\n");coluna += yyleng;}
{COMMA}                           {if(flag==1) printf("COMMA\n");coluna += yyleng;}
{RESERVED}                        {if(flag==1) printf("RESERVED(%s)\n",yytext);coluna += yyleng;}

{DECLIT}                                  {if(flag==1) printf("DECLIT(%s)\n",yytext);coluna += yyleng;}
{REALLIT}                                 {if(flag==1) printf("REALLIT(%s)\n",yytext);coluna += yyleng;}
{ID}                                      {if(flag==1) printf("ID(%s)\n",yytext);coluna += yyleng;}

{IGNORE_CARRIAGE_RETURN}{IGNORE_NEWLINE}  {coluna=1;linha+=1;}
{IGNORE_NEWLINE}                          {coluna=1;linha+=1;}
{IGNORE_CARRIAGE_RETURN}                  {coluna=1;linha+=1;}

{IGNORE_SPACE}                            {coluna += yyleng;}
{IGNORE_TAB}                              {coluna += yyleng;}
{IGNORE_FORMFEED}                         {coluna += yyleng;}

.				                                  {printf("Line %d, col %d: illegal character (%s)\n", linha , coluna, yytext); coluna += yyleng;}

%%

int main(int argc, char **argv)
{
  if(argv[1]!=NULL){
    if(strncmp(argv[1], "-l", 2) == 0) {
      flag = 1;
    }
    else if(strncmp(argv[1], "-1", 2) == 0){
      flag = 1;
    }
  }

  yylex();

  return 0;
}

int yywrap()
{
return 1;
}
