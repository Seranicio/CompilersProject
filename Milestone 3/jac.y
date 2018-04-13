%{
    #define DEBUG 0
    int yylex(void);
    void yyerror (const char *s);

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "estruturas.h"

    int flag_tree_builder = 0;

    Node *Program = NULL;
    Node *auxMethodBody = NULL; //reset done;
    Node *auxMethodParams = NULL; //reset done;
    Node *auxVarDecl = NULL; //reset done;
    Node *auxFormalParams = NULL; //reset done;
    Node *auxProgram = NULL; // not needed?
    Node *auxStatement = NULL;

    //printf("\n\nID: %s | Line: %d | Col: %d\n\n",$1->string,$1->line,$1->col);

%}

%union{
  char* string;
  Line_col* linecol;
  Node* tree_node;
}

%token <string> STRLIT

%token BOOL
%token CLASS
%token DOUBLE
%token ELSE
%token INT
%token PUBLIC
%token STATIC
%token STRING
%token VOID
%token OCURV
%token CCURV
%token OBRACE
%token CBRACE
%token OSQUARE
%token CSQUARE
%token <linecol> AND OR LT GT EQ NEQ LEQ GEQ PLUS MINUS STAR DIV MOD NOT ASSIGN PARSEINT DOTLENGTH DECLIT DO IF PRINT RETURN WHILE BOOLLIT REALLIT
%token SEMI
%token RESERVED
%token COMMA

%token <linecol> ID


%type <string> INT BOOL DOUBLE

%type <tree_node> SubExpr3 SubExpr2 SubExpr SubStatementOpt ParseArgs Assignment Statement SubFormalParams SubMethodBody VarDecl FormalParams SubMethodHeaderBI SubStatementBI2 MethodHeader MethodDecl MethodBody SubFieldDecl FieldDecl Program Subprogram Expr MethodInvocation SubStatement Type SubMethodInvocationOpt ExprAux

%right        ASSIGN
%left         OR
%left         AND
%left         EQ NEQ
%left         GEQ LEQ GT LT
%left         PLUS MINUS
%left         STAR DIV MOD
%right        NOT
%right        OCURV
%left         CCURV
%right        ELSE

%%

Program:  CLASS ID OBRACE Subprogram CBRACE              {if (flag_tree_builder==1){$$ = New_program(add_linecol(Terminals("Id",$2->string),$2),$4);Program=$$;}}

Subprogram: Subprogram FieldDecl                         {if (flag_tree_builder==1) $$ = add_Program($2);}
          | Subprogram MethodDecl                        {if (flag_tree_builder==1) $$ = add_Program($2);}
          | Subprogram SEMI                              {if (flag_tree_builder==1) $$ = add_Program(NULL);}
          | %empty                                       {if (flag_tree_builder==1) $$ = NULL;}

FieldDecl:  PUBLIC STATIC Type ID SubFieldDecl SEMI      {if (flag_tree_builder==1){ $$ = new_Decl("FieldDecl", $3 , add_linecol(Terminals("Id",$4->string),$4) , $5);auxVarDecl=NULL;}}
         |  error SEMI                                   {if (flag_tree_builder==1){ $$ = NULL;cancel_tree();flag_tree_builder=0;}}

SubFieldDecl: SubFieldDecl COMMA ID                      {if (flag_tree_builder==1) $$ = new_Decl2(add_linecol(Terminals("Id",$3->string),$3));}
            | %empty                                     {if (flag_tree_builder==1) $$ = NULL;}

MethodDecl: PUBLIC STATIC MethodHeader MethodBody        {if (flag_tree_builder==1) $$ = new_MethodDecl($3,$4);}

MethodHeader: VOID ID OCURV SubMethodHeaderBI CCURV      {if (flag_tree_builder==1) $$ = new_MethodHeader(Terminals("Void",NULL),add_linecol(Terminals("Id",$2->string),$2),$4);}
            | Type ID OCURV SubMethodHeaderBI CCURV      {if (flag_tree_builder==1) $$ = new_MethodHeader($1,add_linecol(Terminals("Id",$2->string),$2),$4);}

SubMethodHeaderBI: FormalParams                          {if (flag_tree_builder==1){ $$ = new_MethodParams($1);auxMethodParams=NULL;}}
            | %empty                                     {if (flag_tree_builder==1) $$ = NULL;}

MethodBody: OBRACE SubMethodBody CBRACE                  {if (flag_tree_builder==1){ $$ = $2;auxMethodBody=NULL;}}

SubMethodBody: SubMethodBody VarDecl                     {if (flag_tree_builder==1) $$ = new_MethodBody($2);}
             | SubMethodBody Statement                   {if (flag_tree_builder==1) $$ = new_MethodBody($2);}
             | %empty                                    {if (flag_tree_builder==1) $$ = NULL;}

FormalParams: Type ID SubFormalParams                    {if (flag_tree_builder==1){ $$ = new_ParamDecl($1,add_linecol(Terminals("Id",$2->string),$2),$3);auxFormalParams=NULL;}}
            | STRING OSQUARE CSQUARE ID                  {if (flag_tree_builder==1)  $$ = new_ParamDecl(Terminals("StringArray",NULL),add_linecol(Terminals("Id",$4->string),$4),NULL);}

SubFormalParams: SubFormalParams COMMA Type ID           {if (flag_tree_builder==1) $$ = aux_ParamDecl($3, add_linecol(Terminals("Id",$4->string),$4));}
               | %empty                                  {if (flag_tree_builder==1) $$ = NULL;}

VarDecl: Type ID SubFieldDecl SEMI                       {if (flag_tree_builder==1){ $$ = new_Decl("VarDecl", $1 , add_linecol(Terminals("Id",$2->string),$2) , $3);auxVarDecl=NULL;}}

Type: BOOL                                               {if (flag_tree_builder==1) $$ = Terminals("Bool",NULL);}
    | INT                                                {if (flag_tree_builder==1) $$ = Terminals("Int",NULL);}
    | DOUBLE                                             {if (flag_tree_builder==1) $$ = Terminals("Double",NULL);}

Statement: OBRACE SubStatement CBRACE                    {if (flag_tree_builder==1){ $$ = Statement_Block($2);auxStatement=NULL;}}
         | IF OCURV Expr CCURV Statement ELSE Statement  {if (flag_tree_builder==1) $$ = add_linecol(Statement_If($3,$5,$7),$1);}
         | IF OCURV Expr CCURV Statement %prec ELSE      {if (flag_tree_builder==1) $$ = add_linecol(Statement_If($3,$5,NULL),$1);}
         | WHILE OCURV Expr CCURV Statement              {if (flag_tree_builder==1) $$ = add_linecol(Statements_2("While",$3,$5),$1);}
         | DO Statement WHILE OCURV Expr CCURV SEMI      {if (flag_tree_builder==1) $$ = add_linecol(Statements_2("DoWhile",$2,$5),$3);}
         | PRINT OCURV SubStatementOpt CCURV SEMI        {if (flag_tree_builder==1) $$ = add_linecol(Statement_Print($3),$1);}
         | SubStatementBI2 SEMI                          {if (flag_tree_builder==1) $$ = $1;}
         | RETURN SEMI                                   {if (flag_tree_builder==1) $$ = add_linecol(Statement_Return(NULL),$1);}
         | RETURN Expr SEMI                              {if (flag_tree_builder==1) $$ = add_linecol(Statement_Return($2),$1);}
         | error SEMI                                    {if (flag_tree_builder==1){ $$ = NULL;cancel_tree();flag_tree_builder=0;}}

SubStatement: Statement SubStatement                     {if (flag_tree_builder==1) $$ = aux_Statement($1);}
            | %empty                                     {if (flag_tree_builder==1) $$ = NULL;}

SubStatementOpt: Expr                                    {if (flag_tree_builder==1) $$ = $1;}
               | STRLIT                                  {if (flag_tree_builder==1) $$ = Terminals("StrLit",$1);}

SubStatementBI2: Assignment                              {if (flag_tree_builder==1) $$ = $1;}
               | MethodInvocation                        {if (flag_tree_builder==1) $$ = $1;}
               | ParseArgs                               {if (flag_tree_builder==1) $$ = $1;}
               | %empty                                  {if (flag_tree_builder==1) $$ = NULL;}

Assignment: ID ASSIGN Expr                               {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Assign",add_linecol(Terminals("Id",$1->string),$1),$3),$2);}

MethodInvocation: ID OCURV CCURV                                {if (flag_tree_builder==1)  $$ = add_linecol(Operator_Call(add_linecol(Terminals("Id",$1->string),$1), NULL , NULL),$1);}
                | ID OCURV Expr SubMethodInvocationOpt CCURV    {if (flag_tree_builder==1){ $$ = add_linecol(Operator_Call(add_linecol(Terminals("Id",$1->string),$1), $3 , $4),$1);}}
                | ID OCURV error CCURV                          {if (flag_tree_builder==1){ $$ = NULL;cancel_tree();flag_tree_builder=0;}}

SubMethodInvocationOpt: SubMethodInvocationOpt COMMA Expr       {if (flag_tree_builder==1) $$ = addbrother($$,$3);}
                      | %empty                                  {if (flag_tree_builder==1) $$ = NULL;}

ParseArgs: PARSEINT OCURV ID OSQUARE Expr CSQUARE CCURV         {if (flag_tree_builder==1) $$=add_linecol(Operators_2("ParseArgs",add_linecol(Terminals("Id",$3->string),$3),$5),$1);}
         | PARSEINT OCURV error CCURV                           {if (flag_tree_builder==1){ $$ = NULL;cancel_tree();flag_tree_builder=0;}}

Expr: Assignment                            	    {if (flag_tree_builder==1) $$ = $1;}
    | ExprAux                               	    {if (flag_tree_builder==1) $$ = $1;}

ExprAux: MethodInvocation                         {if (flag_tree_builder==1) $$ = $1;}
        | ParseArgs                               {if (flag_tree_builder==1) $$ = $1;}
        | SubExpr                                 {if (flag_tree_builder==1) $$ = $1;}
        | SubExpr2                                {if (flag_tree_builder==1) $$ = $1;}
        | SubExpr3                                {if (flag_tree_builder==1) $$ = $1;}
        | PLUS ExprAux   %prec NOT                {if (flag_tree_builder==1) $$ = add_linecol(Operators_1("Plus",$2),$1);}
        | MINUS ExprAux  %prec NOT                {if (flag_tree_builder==1) $$ = add_linecol(Operators_1("Minus",$2),$1);}
        | NOT ExprAux                             {if (flag_tree_builder==1) $$ = add_linecol(Operators_1("Not",$2),$1);}
        | ID                                      {if (flag_tree_builder==1) $$ = add_linecol(Terminals("Id",$1->string),$1);}
        | ID DOTLENGTH                            {if (flag_tree_builder==1) $$ = add_linecol(Operators_1("Length",add_linecol(Terminals("Id",$1->string),$1)),$2);}
        | OCURV Expr CCURV                        {if (flag_tree_builder==1) $$ = $2;}
        | BOOLLIT                                 {if (flag_tree_builder==1) $$ = add_linecol(Terminals("BoolLit",$1->string),$1);}
        | DECLIT                                  {if (flag_tree_builder==1) $$ = add_linecol(Terminals("DecLit",$1->string),$1);}
        | REALLIT                                 {if (flag_tree_builder==1) $$ = add_linecol(Terminals("RealLit",$1->string),$1);}
        | OCURV error CCURV                       {if (flag_tree_builder==1){ $$ = NULL;cancel_tree();flag_tree_builder=0;}}

SubExpr: ExprAux AND ExprAux                      {if (flag_tree_builder==1) $$=add_linecol(Operators_2("And",$1,$3),$2);}
       | ExprAux OR ExprAux                       {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Or",$1,$3),$2);}

SubExpr2: ExprAux EQ ExprAux                      {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Eq",$1,$3),$2);}
        | ExprAux GEQ ExprAux                     {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Geq",$1,$3),$2);}
        | ExprAux GT ExprAux                      {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Gt",$1,$3),$2);}
        | ExprAux LEQ ExprAux                     {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Leq",$1,$3),$2);}
        | ExprAux LT ExprAux                      {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Lt",$1,$3),$2);}
        | ExprAux NEQ ExprAux                     {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Neq",$1,$3),$2);}

SubExpr3: ExprAux PLUS ExprAux                    {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Add",$1,$3),$2);}
        | ExprAux MINUS ExprAux                   {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Sub",$1,$3),$2);}
        | ExprAux STAR ExprAux                    {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Mul",$1,$3),$2);}
        | ExprAux DIV ExprAux                     {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Div",$1,$3),$2);}
        | ExprAux MOD ExprAux                     {if (flag_tree_builder==1) $$=add_linecol(Operators_2("Mod",$1,$3),$2);}

%%


Node * New_program(Node *Id,Node *sons){
if(DEBUG==1){
  printf("\n20------------------------\n\n");
}
  if(Program == NULL){
    Node * add_Program = (Node *) malloc (sizeof(Node));

    add_Program -> type = (char *) malloc (sizeof("Program"));
    strcpy(add_Program->type,"Program");

    Program = add_Program;
    Program -> son = Id;
    Id -> next = sons;
  }
  return Program;
}

Node * add_Program(Node *add){
  Node * aux;

  if(auxProgram == NULL){
    auxProgram = add;
  }
  else{
    aux = auxProgram;
    while(aux->next != NULL){
      aux= aux->next;
    }
    aux -> next = add;
  }

  return auxProgram;
}

/*Variable Declaration */

Node * new_Decl(char *type_of_decl , Node *id_type, Node *FirstId , Node *Brothers){
if(DEBUG==1){
  printf("\n19------------------------\n\n");
}
  Node *aux;

  Node * decl_node = (Node *) malloc (sizeof(Node));
  decl_node -> type = (char *) malloc (sizeof(type_of_decl));
  strcpy(decl_node->type,type_of_decl);


  decl_node -> son = id_type;

  aux = decl_node -> son;
  aux -> next = FirstId;

  aux = decl_node;

  aux = Brothers;

  while(aux!=NULL){
    aux -> type = (char *) malloc (sizeof(type_of_decl));
    strcpy(aux->type,type_of_decl);

    aux -> son -> type = (char *) malloc (sizeof(id_type->type));
    strcpy(aux -> son -> type,id_type->type);

    aux = aux -> next;
  }

  decl_node -> next = Brothers;

  return decl_node;
}

Node * new_Decl2(Node * id){  /* Auxiliar for >1 variable declaration. tree without <type> */
if(DEBUG==1){
  printf("\n18------------------------\n\n");
}
  Node * aux;

  Node * decl_node = (Node *) malloc (sizeof(Node));

  Node * type_id_node = (Node *) malloc (sizeof(Node)); /*empty */
  type_id_node-> type = NULL;
  type_id_node-> id = NULL;
  type_id_node-> next = NULL;
  type_id_node-> son = NULL;

  decl_node -> son = type_id_node;
  aux = decl_node;
  aux = aux -> son;

  aux -> next = id;

  if(auxVarDecl == NULL){
    auxVarDecl = decl_node;
  }
  else{
    aux = auxVarDecl;
    while(aux -> next != NULL){
      aux = aux -> next;
    }
    aux-> next = decl_node;
  }

  return auxVarDecl;
}

/* Method Declaration */

Node * new_MethodDecl(Node *header , Node * body){
if(DEBUG==1){
  printf("\n17------------------------\n\n");
}
  Node *aux;
  Node * decl_node = (Node *) malloc(sizeof(Node));

  decl_node -> type = (char *) malloc (sizeof("MethodDecl"));
  strcpy(decl_node->type, "MethodDecl");

  decl_node->son = header; /* First son */

  aux = decl_node->son;
  aux->next = body;

  if(body == NULL){ /* nós supérfluos */
    Node * aux_node = (Node *) malloc(sizeof(Node));

    aux_node -> type = (char *) malloc (sizeof("MethodBody"));
    strcpy(aux_node->type, "MethodBody");

    aux->next = aux_node;
  }

  return decl_node;
}

Node * new_MethodHeader(Node *type, Node *id , Node *params){
  if(DEBUG==1){
    printf("\n16------------------------\n\n");
  }

  Node * aux;
  Node * header_node = (Node *)malloc(sizeof(Node));

  header_node -> type = (char *) malloc (sizeof("MethodHeader"));
  strcpy(header_node->type, "MethodHeader");

  aux = header_node;
  aux->son = type;
  aux = aux -> son;

  aux-> next = id;
  aux = aux -> next;

  aux-> next = params;
  if(params == NULL){ /* nós supérfluos */
    Node * aux_node = (Node *)malloc(sizeof(Node));

    aux_node -> type = (char *) malloc (sizeof("MethodParams"));
    strcpy(aux_node->type, "MethodParams");

    aux->next = aux_node;
  }


  return header_node;
}

Node * new_MethodParams(Node *Params){ /* Params nunca e NULL / empty */
if(DEBUG==1){
  printf("\n15------------------------\n\n");
}
  Node * aux;

  if(auxMethodParams == NULL){
    Node * params_node= (Node *)malloc(sizeof(Node));

    params_node -> type = (char *) malloc (sizeof("MethodParams"));
    strcpy(params_node->type, "MethodParams");

    aux = params_node;
    aux->son = Params;

    auxMethodParams = params_node;
  }
  else{
    aux = auxMethodParams;
    while(aux->next!=NULL){
      aux = aux->next;
    }
    aux->next = Params;
  }
  return auxMethodParams;
}

Node * new_ParamDecl(Node *type_or_stringArray , Node *id , Node *brothers){
if(DEBUG==1){
  printf("\n14------------------------\n\n");
}
  Node *aux;
  Node * params_decl_node= (Node *)malloc(sizeof(Node));

  params_decl_node -> type = (char *) malloc (sizeof("ParamDecl"));
  strcpy(params_decl_node->type, "ParamDecl");

  aux = params_decl_node;
  aux -> son = type_or_stringArray;
  aux = aux -> son;

  aux->next = id;

  params_decl_node -> next = brothers;

  return params_decl_node;
}

Node * aux_ParamDecl(Node *type , Node *id){
if(DEBUG==1){
  printf("\n13------------------------\n\n");
}

  Node *aux;
  Node *aux_node = (Node *)malloc(sizeof(Node));
  aux_node -> type = (char *) malloc (sizeof("ParamDecl"));
  strcpy(aux_node->type, "ParamDecl");

  aux_node -> son = type;
  aux = aux_node -> son;
  aux -> next = id;

  if(auxFormalParams == NULL){
    auxFormalParams = aux_node;
  }
  else{
    aux = auxFormalParams;
    while(aux->next!=NULL){
      aux = aux -> next;
    }
    aux->next = aux_node;
  }

  return auxFormalParams;
}

Node * new_MethodBody(Node *vardecl_or_statement){ /* vardecl_or_statement nunca e NULL / empty */
if(DEBUG==1){
  printf("\n12------------------------\n\n");
}

  Node *aux;

  if(auxMethodBody == NULL){
    Node * body_node= (Node *)malloc(sizeof(Node));

    body_node -> type = (char *) malloc (sizeof("MethodBody"));
    strcpy(body_node->type, "MethodBody");

    aux = body_node;
    aux->son = vardecl_or_statement;
    auxMethodBody = body_node;
  }
  else{
    if(vardecl_or_statement == NULL){
      return auxMethodBody;
    }
    aux = auxMethodBody;
    aux = aux ->son;
    while(aux->next!=NULL){
      aux = aux->next;
    }
    aux->next = vardecl_or_statement;
  }

  return auxMethodBody;
}

/* Statements */

Node * Statements_2(char *statement_name , Node *left , Node *right){
  if(DEBUG==1){
    printf("\n11------------------------\n\n");
  }

  Node * aux;
  Node * statement2_node= (Node *)malloc(sizeof(Node));

  statement2_node -> type = (char *) malloc (sizeof(statement_name));
  strcpy(statement2_node->type, statement_name);

  if(left == NULL){
    left= (Node *)malloc(sizeof(Node));

    left -> type = (char *) malloc (sizeof("Block"));
    strcpy(left->type, "Block");
  }
  if(right == NULL){
    right= (Node *)malloc(sizeof(Node));

    right -> type = (char *) malloc (sizeof("Block"));
    strcpy(right->type, "Block");
  }

  aux = statement2_node;

  aux -> son = left;

  aux = aux -> son;

  aux -> next = right;

  return statement2_node;
}

Node * Statement_If(Node * son , Node *brother1 , Node * brother2){
  if(DEBUG==1){
    printf("\n10------------------------\n\n");
  }

  Node *aux;
  Node * statementif_node= (Node *)malloc(sizeof(Node));

  statementif_node -> type = (char *) malloc (sizeof("If"));
  strcpy(statementif_node->type, "If");

  statementif_node->son = son;
  aux = statementif_node->son;

  if(brother1==NULL){
    brother1= (Node *)malloc(sizeof(Node));

    brother1 -> type = (char *) malloc (sizeof("Block"));
    strcpy(brother1->type, "Block");
  }
  if(brother2==NULL){
    brother2= (Node *)malloc(sizeof(Node));

    brother2 -> type = (char *) malloc (sizeof("Block"));
    strcpy(brother2->type, "Block");
  }

  aux -> next = brother1;
  aux = aux -> next;
  aux -> next = brother2;

  return statementif_node;
}

Node * Statement_Print(Node *son){
  if(DEBUG==1){
    printf("\n9------------------------\n\n");
  }

  Node * print_node= (Node *)malloc(sizeof(Node));

  print_node -> type = (char *) malloc (sizeof("Print"));
  strcpy(print_node->type, "Print");

  print_node -> son = son;

  return print_node;
}

Node * Statement_Block (Node *son){
  if(DEBUG==1){
    printf("\n8------------------------\n\n");
  }

  int counter =0;
  Node *aux;

  Node * block_node= (Node *)malloc(sizeof(Node));

  block_node -> type = (char *) malloc (sizeof("Block"));
  strcpy(block_node->type, "Block");

  aux = son;

  while(aux != NULL){
    counter ++;
    aux = aux->next;
  }

  if (counter > 1){
    block_node -> son = son;
    return block_node;
  }


  return son;
}

Node *aux_Statement(Node *add){
  if(DEBUG==1){
    printf("\n7------------------------\n\n");
  }

  if(add == NULL){
    return auxStatement;
  }

  if (auxStatement == NULL){
    auxStatement = add;
  }
  else{
    add -> next = auxStatement;
    auxStatement = add;
  }

  return auxStatement;
}

Node * Statement_Return(Node *return_value){
if(DEBUG==1){
  printf("\n6------------------------\n\n");
}

  Node * return_node= (Node *)malloc(sizeof(Node));

  return_node -> type = (char *) malloc (sizeof("Return"));
  strcpy(return_node->type, "Return");

  return_node -> son = return_value;

  return return_node;
}

/* Operators */

Node * Operators_2(char *operation , Node *left , Node *right){
  if(DEBUG==1){
    printf("\n5------------------------\n\n");
  }

  Node *aux;
  Node * operator2_node = (Node *)malloc(sizeof(Node));

  operator2_node -> type = (char *) malloc (sizeof(operation));
  strcpy(operator2_node->type, operation);

  operator2_node->son = left;
  aux=operator2_node->son;

  if(left==NULL){
    return operator2_node;
  }

  aux->next=right;

  return operator2_node;
}

Node * Operators_1(char *operation, Node * son){
if(DEBUG==1){
  printf("\n4------------------------\n\n");
}

  Node * operator2_node = (Node *)malloc(sizeof(Node));

  operator2_node -> type = (char *) malloc (sizeof(operation));
  strcpy(operator2_node->type, operation);

  operator2_node->son = son;

  return operator2_node;
}

Node * Operator_Call(Node * id , Node *firstson , Node *brothers){
  if(DEBUG==1){
    printf("\n3------------------------\n\n");
  }

  Node * aux;
  Node * call_node = (Node *)malloc(sizeof(Node));

  call_node -> type = (char *) malloc (sizeof("Call"));
  strcpy(call_node->type, "Call");

  call_node->son = id;
  aux = call_node -> son;

  if(firstson==NULL){
    return call_node;
  }

  aux->next = firstson;

  while(aux->next !=NULL){
    aux = aux -> next;
  }
  aux -> next = brothers;

  return call_node;
}

Node * addbrother(Node *son1, Node * son2){
  Node * aux;

  if(son1 == NULL && son2 != NULL){
    son1 = son2;
    return son1;
  }

  if(son1 == NULL && son2 == NULL){
    return NULL;
  }

  aux = son1;

  while(aux->next!=NULL){
    aux = aux ->next;
  }

  aux-> next = son2;

  return son1;
}

/* Terminals */

Node * Terminals(char *type , char *id){
  if(DEBUG==1){
    printf("\n1------------------------\n\n");
  }

  Node * terminal = (Node * ) malloc(sizeof(Node));

  terminal-> type = type;
  terminal-> id = id;
  terminal-> next = NULL;
  terminal-> son = NULL;


  return terminal;
}

/* Printing Tree */

void printing_tree(Node *Program, int pontos){
  int i;
  Node *aux;
  if(Program==NULL){
    return;
  }
  aux = Program;
  while(aux!=NULL){
    //testing_nodes(aux);
    for(i=0;i<pontos;i++){
      printf("..");
    }
    if(aux->type != NULL){
      if(aux->id != NULL){
        printf("%s(%s)\n",aux->type,aux->id);
      }
      else if (aux -> type != NULL){
        printf("%s\n",aux->type);
      }
    }
    if(aux->son != NULL){
      printing_tree(aux->son,pontos+1);
    }
    aux = aux->next;
  }
}

void printing_tree2(Node *Program, int pontos){
  int i;
  Node *aux;
  if(Program==NULL){
    return;
  }
  aux = Program;
  while(aux!=NULL){
    //testing_nodes(aux);
    for(i=0;i<pontos;i++){
      printf("..");
    }
    if(aux->type != NULL){
      if(aux->id != NULL){
        printf("%s(%s)",aux->type,aux->id);
        if(aux->anotation != NULL){
          if(strncmp(aux->anotation,"undef",5) == 0){
            printf(" - %s",aux->anotation);
          }
          else{
            printf(" - %s",maius_minusc(aux->anotation));
          }
        }
        printf("\n");
      }
      else if (aux -> type != NULL){
        printf("%s",aux->type);
        if(aux->anotation != NULL){
          if(strncmp(aux->anotation,"undef",5) == 0){
            printf(" - %s",aux->anotation);
          }
          else{
            printf(" - %s",maius_minusc(aux->anotation));
          }
        }
        printf("\n");
      }
    }
    if(aux->son != NULL){
      printing_tree2(aux->son,pontos+1);
    }
    aux = aux->next;
  }
}

void print_Program(){
  printing_tree(Program,0);
}

void print_ASTnoted(){
  printing_tree2(Program,0);
}

void activate_build_flag(){
  flag_tree_builder=1;
}

void testing_nodes(Node *Program){
  printf("\n1------------------------\n\n");

  if(Program == NULL){
   printf("I AM NULL\n");
   return;
  }

  printf("me:");

  if(Program -> type == NULL){
    printf(" Type: NULL ");
  }
  else{
    printf(" Type: %s",Program -> type);
  }

  if(Program -> id == NULL){
    printf("|  Id: NULL ");
  }
  else{
    printf("|  Id: %s",Program -> id);
  }

  if(Program -> next == NULL){
    printf("|  Next: NULL ");
  }
  else{
    printf("|  Next: %s",Program -> next -> type);
  }

  if(Program -> son == NULL){
    printf("|  Son: NULL \n");
  }
  else{
    printf("|  Son: %s\n",Program -> son -> type);
  }

  printf("\n2------------------------\n");
}

Node* add_linecol(Node *no,Line_col *linecol){
  no->linha_and_col = linecol;
  return no;
}