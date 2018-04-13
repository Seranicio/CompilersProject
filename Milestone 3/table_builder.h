typedef struct params_tab{ //Params Table.
  char *name;
  char *type;
  int param_flag;
  int special_flag;
  struct params_tab *next;
}Params_tab;

typedef struct method_tab{ //Method Table = Himself + Params Table.
  char *name;
  char *type;
  Params_tab *variables;
  struct method_tab *next;
}Method_tab;

typedef struct method_or_var{ //To se what order method and variable are declared in Class Program.
  Params_tab *global_var;
  Method_tab *methods;
  struct method_or_var * next;
}Method_or_var;

typedef struct symbol_tab{ //Program Table.
  char *name;
  Method_or_var *next;
}Symbol_tab;

Symbol_tab *symbol_table = NULL;

void show_table();
void check_MethodDecl(Method_tab *method,Node *node);
void check_FieldDecl(Node *new);
void check_Program(Node *root);
void method_table_print(Method_tab *table);
void check_Variables(Node *MethodBody);
void check_ParamDecl(Method_tab *new_method , Node *ParamDecl);
char * maius_minusc(char *name);
void helper_print_class(Params_tab *aux_var);
void check_VarDecl(Method_tab *new_method , Node *VarDecl);
int  ambiguous_method_checker(Method_tab *method_table,Method_tab *new_method);
char * check_VariableTable(Method_tab *new_method , Node *atribuition);
char * check_Operators(Method_tab *new_method , Node *Operator);
char * check_Operations_Int(Method_tab *new_method,Node *Operation);
char * check_Operations_Bool(Method_tab *new_method,Node *Operation);
char * check_Expr(Method_tab *new_method,Node *Expr);
void Insert_MethodDecl(Node *new);
char * check_MethodVerifier(Method_tab *new_method , Node *Method);
char * check_VariableTableMethod(Method_tab *new_method , Node *atribuition);
char * DecLit_Verifier(char* declit);
char * RealLit_Verifier(char *reallit);

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

void check_Program(Node *root){
  Node *aux = NULL;
  Method_or_var *aux_method_var;

  if(strncmp(root->type,"Program",7) == 0){
    if(symbol_table==NULL){
      aux = root->son;
      symbol_table = (Symbol_tab *) malloc(sizeof(Symbol_tab));
      symbol_table->name = aux->id;
    }
  }

  //Since java allows to call a method after it's declaration before , we need to put in table all the variables and methods of class then pass the method analyzer.
  while(aux -> next!= NULL){
    if(strncmp(aux->type,"FieldDecl",9) == 0){
      check_FieldDecl(aux);
    }
    if(strncmp(aux->type,"MethodDecl",10) == 0){
      Insert_MethodDecl(aux);
    }
    aux = aux->next;
  }
  //Last element check.
  if(strncmp(aux->type,"FieldDecl",9) == 0){
    check_FieldDecl(aux);
  }
  if(strncmp(aux->type,"MethodDecl",10) == 0){
    Insert_MethodDecl(aux);
  }

  aux = root->son;
  aux = aux -> next;
  aux_method_var = symbol_table->next;

  if(aux == NULL){
    return;
  }


  while(aux -> next!= NULL && aux_method_var != NULL ){
    if( (strncmp(aux->type,"MethodDecl",10) == 0) && ( aux->skip_checker != 3)){
      check_MethodDecl(aux_method_var->methods,aux);
    }
    if(aux->skip_checker != 3){
      aux_method_var = aux_method_var->next;
    }
    aux = aux->next;
  }

  //Last element check.
  if(strncmp(aux->type,"MethodDecl",10) == 0 && aux_method_var != NULL){
    check_MethodDecl(aux_method_var->methods,aux);
  }
}

void check_FieldDecl(Node *new){

  Method_or_var *aux;

  Node *aux_node;

  Params_tab * new_var = (Params_tab * ) malloc (sizeof(Params_tab));

  aux_node = new ->son;
  new_var -> type = aux_node->type;

  aux_node = aux_node ->next;
  new_var -> name = aux_node->id;

  aux = symbol_table->next;

  if(aux == NULL){
    Method_or_var * new_method_or_var = (Method_or_var*) malloc(sizeof(Method_or_var));
    new_method_or_var->global_var = new_var;
    new_method_or_var->methods = NULL;
    symbol_table->next = new_method_or_var;
    return;
  }

  //Checks if Variable already defined.
  while(aux->next!=NULL){
    if(aux->global_var != NULL){
      if(strcmp(aux->global_var ->name, new_var->name) == 0){
        new->skip_checker=3;
        printf("Line %d, col %d: Symbol %s already defined\n",aux_node->linha_and_col->line,aux_node->linha_and_col->col ,new_var->name);
        return;
      }
      else{
        aux = aux->next;
      }
    }
    else{
      aux = aux->next;
    }
  }
  //Last element check.
  if(aux->global_var != NULL){
    if(strcmp(aux->global_var ->name, new_var->name) == 0){
      new->skip_checker=3;
      printf("Line %d, col %d: Symbol %s already defined\n",aux_node->linha_and_col->line,aux_node->linha_and_col->col ,new_var->name);
      return;
    }
  }


  Method_or_var * new_method_or_var = (Method_or_var*) malloc(sizeof(Method_or_var));
  new_method_or_var->global_var = new_var;
  new_method_or_var->methods = NULL;
  aux->next = new_method_or_var;
}

void Insert_MethodDecl(Node *new){

  Method_or_var *aux;
  Node *aux_node;

  Method_tab * new_method = (Method_tab * ) malloc (sizeof(Method_tab));

  aux_node = new->son;

  aux_node = aux_node -> son;
  new_method -> type = aux_node->type;

  aux_node=aux_node->next;
  new_method -> name = aux_node->id;

  aux_node= aux_node->next;

  //Inserir parametros.
  if(strncmp(aux_node->type,"MethodParams",12) == 0){ //Adding Method Parameters to new_method.
    aux_node = aux_node -> son;
    while(aux_node != NULL){
      if(strncmp(aux_node->type,"ParamDecl",9) == 0 ){ //checking Param Declaration.
        check_ParamDecl(new_method,aux_node);
      }
      aux_node = aux_node->next;
    }
  }

  //Inserir Tabela.
  aux = symbol_table->next;

  if(aux == NULL){
    Method_or_var * new_method_or_var = (Method_or_var*) malloc(sizeof(Method_or_var));
    new_method_or_var->global_var = NULL;
    new_method_or_var->methods = new_method;
    symbol_table->next = new_method_or_var;
    return;
  }

  while(aux->next!=NULL){
    if(aux->methods != NULL){
      aux_node = new->son->son->next->next->son;

      if(ambiguous_method_checker(aux->methods,new_method) == 1){
        //getting params.
        new->skip_checker = 3;

        printf("Line %d, col %d: Symbol %s(",new->son->son->next->linha_and_col->line,new->son->son->next->linha_and_col->col,new_method->name);
        while(aux_node != NULL){
          if(aux_node->next == NULL){
            printf("%s",maius_minusc(aux_node->son->type));
          }
          else{
            printf("%s,",maius_minusc(aux_node->son->type));
          }
          aux_node = aux_node -> next;
        }
        printf(") already defined\n");

        return;
      }
      else{
        aux = aux -> next;
      }
    }
    else{
      aux = aux->next;
    }
  }

  if(aux->methods != NULL){
    if(ambiguous_method_checker(aux->methods,new_method) == 1){

      new->skip_checker = 3;

      aux_node = new->son->son->next->next->son;

      printf("Line %d, col %d: Symbol %s(",new->son->son->next->linha_and_col->line,new->son->son->next->linha_and_col->col,new_method->name);
      while(aux_node != NULL){
        if(aux_node->next == NULL){
          printf("%s",maius_minusc(aux_node->son->type));
        }
        else{
          printf("%s,",maius_minusc(aux_node->son->type));
        }
        aux_node = aux_node -> next;
      }
      printf(") already defined\n");
      return;
    }
  }

  Method_or_var * new_method_or_var = (Method_or_var*) malloc(sizeof(Method_or_var));
  new_method_or_var->global_var = NULL;
  new_method_or_var->methods = new_method;
  aux->next = new_method_or_var;
}

void check_MethodDecl(Method_tab *method,Node *node){
  Node *aux_node;

  aux_node = node->son->next; //MethodBody Assignment.
  if(strncmp(aux_node->type,"MethodBody",10) == 0){
    aux_node = aux_node->son;
    while(aux_node!=NULL){
      if(strncmp(aux_node->type,"VarDecl",7) == 0){       //Checking Variable Declaration
        check_VarDecl(method,aux_node);
      }
      //Checking Statements.
      else if(strncmp(aux_node->type,"Assign",6) == 0 || strcmp(aux_node->type,"If") == 0  || strcmp(aux_node->type,"Block") == 0 || strcmp(aux_node->type,"Call") == 0 || strcmp(aux_node->type,"ParseArgs") == 0 || strcmp(aux_node->type,"While") == 0
      || strcmp(aux_node->type,"Return") == 0 || strcmp(aux_node->type,"Print") == 0 || strcmp(aux_node->type,"DoWhile") == 0 ){
        check_Operators(method,aux_node);
      }
      aux_node = aux_node ->next;
    }
  }
  else{
    printf("ERROR: MethodBody not found?");
  }
}

int ambiguous_method_checker(Method_tab *method_table,Method_tab *new_method){ //Returns 1- ambiguous 0- not.
  int table_counter = 0;
  int new_method_counter = 0;
  int i=0;

  Params_tab *aux_table;
  Params_tab *aux_new_method;

  if(strcmp(method_table->name,new_method->name) != 0){
    return 0;
  }


  aux_table = method_table->variables;
  aux_new_method = new_method->variables;

  // counting params of method in table.
  while(aux_table!=NULL){
    if(aux_table->param_flag == 1 ){
      table_counter ++;
      aux_table = aux_table -> next;
    }
    else{
      break;
    }
  }

  //counting params of new method.
  while(aux_new_method != NULL){
    if(aux_new_method->param_flag == 1 ){
      new_method_counter ++;
      aux_new_method = aux_new_method -> next;
    }
    else{
      break;
    }
  }

  aux_table = method_table->variables;
  aux_new_method = new_method->variables;

  //printf("\n\ntable name: %s(%s) | newMethod name: %s(%s)\n\n",aux_table->type,aux_table->name , aux_new_method->type , aux_new_method->name);

  //if params counter are diferent then it's not ambiguous.
  if(table_counter != new_method_counter){
    return 0;
  }
  else{
    for(i=0;i<table_counter;i++){
      if(strcmp(aux_table -> type , aux_new_method->type) == 0){
        aux_table = aux_table -> next;
        aux_new_method = aux_new_method -> next;
      }
      else{
        return 0;
      }
    }
  }
  return 1;
}

void check_ParamDecl(Method_tab *new_method , Node *ParamDecl){
  Node *aux_node;
  Method_tab *aux_method;
  Params_tab *aux_var;

  Params_tab *new_Param = (Params_tab *) malloc(sizeof(Params_tab));

  aux_node=ParamDecl->son;

  new_Param->type = aux_node->type;

  aux_node = aux_node->next;

  new_Param->name = aux_node->id;

  new_Param->param_flag = 1;

  aux_method = new_method;

  if(aux_method->variables == NULL){
    aux_method->variables = new_Param;
    return;
  }

  aux_var = new_method -> variables;

  while(aux_var->next != NULL){
    if(strcmp(aux_var -> name ,ParamDecl->son->next->id) == 0){
      printf("Line %d, col %d: Symbol %s already defined\n",ParamDecl->son->next->linha_and_col->line,ParamDecl->son->next->linha_and_col->col ,aux_var -> name);
      while(aux_var->next != NULL){
        aux_var = aux_var ->next;
      }
      new_Param->special_flag = 2;
      aux_var->next = new_Param;
      return;
    }
    aux_var = aux_var -> next;
  }

  if(strcmp(aux_var -> name ,ParamDecl->son->next->id) == 0){
    new_Param->special_flag = 2;
    aux_var->next = new_Param;
    printf("Line %d, col %d: Symbol %s already defined\n",ParamDecl->son->next->linha_and_col->line,ParamDecl->son->next->linha_and_col->col ,aux_var -> name);
    return;
  }

  aux_var->next = new_Param;
}

char * check_Operators(Method_tab *new_method , Node *Operator){
  Node *aux_node;
  char * result;
  char * result2;

  aux_node = Operator;

  if(strncmp(aux_node->type,"Assign",6) == 0){
    aux_node = aux_node -> son;

    //cheking if atribuition variable exists in table.
    result = check_Expr(new_method,aux_node); //returns type of the atribuition variable.

    //Checking if Assignment is OK.
    aux_node = aux_node->next;

    result2 = check_Expr(new_method,aux_node);

    if(result == NULL && result2 == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types undef, undef\n",Operator->linha_and_col->line,Operator->linha_and_col->col,maius_minusc(Operator->type));
      Operator->anotation = "undef";
      return NULL;
    }
    else if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types undef, %s\n",Operator->linha_and_col->line,Operator->linha_and_col->col,maius_minusc(Operator->type),maius_minusc(result2));
      Operator->anotation = "undef";
      return NULL;
    }
    else if(result2 == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, undef\n",Operator->linha_and_col->line,Operator->linha_and_col->col,maius_minusc(Operator->type),maius_minusc(result));
      Operator->anotation = result;
      return result;
    }

    else if( (strcmp(result,result2) == 0 && strcmp(result,"StringArray") != 0 && strcmp (result2,"StringArray") != 0 ) || (strcmp(result,"Double") == 0 && strcmp(result2,"Int") == 0 ) ){
      Operator->anotation = result;
      return result;
    }
    else{ //incompatible type.
      Operator->anotation = result;
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Operator->linha_and_col->line,Operator->linha_and_col->col,maius_minusc(Operator->type),maius_minusc(result),maius_minusc(result2));
      return result;
    }
  }
  else if(strcmp(aux_node->type,"If") == 0){
    //Verify Condition.
    aux_node = aux_node -> son;
    char * retorno;

    result = check_Expr(new_method,aux_node);

    if(result == NULL){
      retorno = NULL;
      printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(Operator->type));
    }
    else if(strcmp(result,"Bool") != 0){
      retorno = NULL;
      printf("Line %d, col %d: Incompatible type %s in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(result),maius_minusc(Operator->type));
    }
    else{
      retorno = result;
    }

    aux_node= aux_node -> next;
    check_Operators(new_method,aux_node);
    aux_node = aux_node -> next;
    check_Operators(new_method,aux_node);

    return retorno;
  }
  else if(strcmp(aux_node->type,"Block") == 0 ){
    aux_node = aux_node -> son;
    if(aux_node == NULL){
      return NULL;
    }
    else{
      while(aux_node != NULL){
        check_Operators(new_method,aux_node);
        aux_node = aux_node -> next;
      }
    }
  }
  else if(strcmp(aux_node->type,"Print") == 0){
    aux_node = aux_node -> son;
    result = check_Expr(new_method,aux_node);
    if(result == NULL){
      printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(Operator->type));
      return NULL;
    }
    else if(strcmp(result,"StringArray") == 0 || strcmp(result,"Void") == 0){
      printf("Line %d, col %d: Incompatible type %s in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(result),maius_minusc(Operator->type));
      return NULL;
    }else{
      return result;
    }
  }
  else if(strcmp(aux_node->type,"Return") == 0){
    aux_node = aux_node -> son;

    if(aux_node == NULL){
      if(strcmp(new_method->type,"Void") == 0){
        return "Void";
      }
      else{
        printf("Line %d, col %d: Incompatible type void in %s statement\n",Operator->linha_and_col->line,Operator->linha_and_col->col,maius_minusc(Operator->type));
        return NULL;
      }
    }
    else{
      result = check_Expr(new_method,aux_node);
      if(result == NULL){
        printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(Operator->type));
        return NULL;
      }
      else if((strcmp(new_method->type,result) == 0 && strcmp(result,"Void") ) || (strcmp(new_method->type,"Double") == 0 && strcmp(result,"Int") == 0 )){
        return result;
      }else{
        printf("Line %d, col %d: Incompatible type %s in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(result),maius_minusc(Operator->type));
        return NULL;
      }
    }
  }
  else if(strcmp(aux_node->type,"While") == 0){
    aux_node = aux_node -> son;
    result = check_Expr(new_method,aux_node);
    char * retorno;

    if(result == NULL){
      retorno = NULL;
      printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(Operator->type));
    }
    else if(strcmp(result,"Bool") != 0){
      retorno = NULL;
      printf("Line %d, col %d: Incompatible type %s in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(result),maius_minusc(Operator->type));
    }
    else{
      retorno = result;
    }

    aux_node = aux_node -> next;

    check_Operators(new_method,aux_node);

    return retorno;
  }
  else if(strcmp(aux_node->type,"DoWhile") == 0){
    aux_node = aux_node -> son;
    check_Operators(new_method,aux_node);

    aux_node = aux_node -> next;
    result = check_Expr(new_method,aux_node);

    if(result == NULL) {
      printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->next->linha_and_col->line,Operator->son->next->linha_and_col->col,maius_minusc(Operator->type));
      return NULL;
    }
    else if(strcmp(result,"Bool") != 0){
      printf("Line %d, col %d: Incompatible type %s in %s statement\n",Operator->son->next->linha_and_col->line,Operator->son->next->linha_and_col->col,maius_minusc(result),maius_minusc(Operator->type));
      return NULL;
    }
    return "Bool";
  }
  else if(strcmp(aux_node -> type,"ParseArgs") == 0){
    result = check_Expr(new_method,aux_node);

    if(result == NULL){
      printf("Line %d, col %d: Incompatible type undef in %s statement\n",Operator->son->linha_and_col->line,Operator->son->linha_and_col->col,maius_minusc(Operator->type));
      return NULL;
    }

    return result;
  }
  else if(strcmp(aux_node -> type,"Call") == 0){  //TO DO TEST

    result = check_Expr(new_method,aux_node);
    return result;
  }

  return NULL;
}

/* ################################################################# */
/*                    OPERATOR2 (BOOLEAN) VERIFIER                   */
/* ################################################################# */

char * check_Operations_Bool(Method_tab *new_method,Node *Operation){
  Node * firstson;
  Node * secondson;

  char * firstresult;
  char * secondresult;

  firstson = Operation->son;
  secondson = Operation->son->next;

  if(strcmp (firstson->type,"Eq") == 0 || strcmp (firstson->type,"Neq") == 0 || strcmp (firstson->type,"Lt") == 0 || strcmp (firstson->type,"Gt") == 0 || strcmp (firstson->type,"Leq") == 0 || strcmp (firstson->type,"Geq") == 0 || strcmp (firstson->type,"Or") == 0 || strcmp (firstson->type,"And") == 0){
    firstresult = check_Expr(new_method , firstson);
  }
  else{
    firstresult = check_Expr(new_method,firstson);
  }

  if(strcmp (secondson->type,"Eq") == 0 || strcmp (secondson->type,"Neq") == 0 || strcmp (secondson->type,"Lt") == 0 || strcmp (secondson->type,"Gt") == 0 || strcmp (secondson->type,"Leq") == 0 || strcmp (secondson->type,"Geq") == 0 || strcmp (secondson->type,"Or") == 0 || strcmp (secondson->type,"And  ") == 0){
    secondresult=check_Expr(new_method , secondson);
  }
  else{
    secondresult= check_Expr(new_method , secondson);
  }

  if(firstresult == NULL && secondresult == NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types undef, undef\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type));
    Operation->anotation = "Bool";
    return "Bool";
  }

  if(firstresult == NULL && secondresult != NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types undef, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(secondresult));
    Operation->anotation = "Bool";
    return "Bool";
  }

  if(secondresult == NULL && firstresult != NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types %s, undef\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult));
    Operation->anotation = "Bool";
    return "Bool";
  }

  if(firstresult!= NULL && secondresult != NULL){
    if(strcmp (Operation->type,"Eq") == 0  || strcmp (Operation->type,"Neq") == 0 ){
      if(( strcmp(firstresult,"Int") == 0 && strcmp(secondresult,"Double") == 0 ) || ( strcmp(firstresult,"Double") == 0 && strcmp(secondresult,"Int") == 0 )  || ( strcmp(firstresult,"Int") == 0 && strcmp(secondresult,"RealLit") == 0 ) || ( strcmp(firstresult,"RealLit") == 0 && strcmp(secondresult,"Int") == 0 ) || ( strcmp(firstresult,"Double") == 0 && strcmp(secondresult,"RealLit") == 0 ) || ( strcmp(firstresult,"RealLit") == 0 && strcmp(secondresult,"Double") == 0 )){
        Operation->anotation = "Bool";
        return "Bool";
      }
      else if( strcmp(firstresult,secondresult) != 0 || strcmp(firstresult,"Void") == 0 || strcmp(secondresult,"Void") == 0 || strcmp(firstresult,"StringArray") == 0 || strcmp(secondresult,"StringArray") == 0){
        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult),maius_minusc(secondresult));
        Operation->anotation = "Bool";
        return "Bool";
      }
      else{
        Operation->anotation = "Bool";
        return "Bool";
      }
    }
    else if(strcmp (Operation->type,"And") == 0  || strcmp (Operation->type,"Or") == 0){
      if(strcmp(firstresult,"Bool") == 0 && strcmp(secondresult,"Bool") == 0){
        Operation->anotation = "Bool";
        return "Bool";
      }
      else{
        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult),maius_minusc(secondresult));
        Operation -> anotation = "Bool";
        return "Bool";
      }
    }
    else{
      if ((strcmp (firstresult,"Int") != 0  && strcmp(firstresult,"Double") != 0  && strcmp(firstresult,"RealLit") != 0 ) ||  ( strcmp (secondresult,"Int") != 0  && strcmp(secondresult,"Double") != 0  && strcmp (secondresult,"RealLit") != 0 )){
        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult),maius_minusc(secondresult));
        Operation->anotation = "Bool";
        return "Bool";
      }
      else{
        Operation -> anotation = "Bool";
        return "Bool";
      }
    }
  }

  return NULL;
}

/* ################################################################# */
/*                  OPERATOR2 (INT|DOUBLE) VERIFIER                  */
/* ################################################################# */

char * check_Operations_Int(Method_tab *new_method,Node *Operation){
  Node * aux_node;
  char * firstresult;
  char * secondresult;

  aux_node = Operation -> son;

  firstresult = check_Expr(new_method,aux_node);

  aux_node = aux_node -> next;

  secondresult = check_Expr(new_method,aux_node);

  if(firstresult == NULL && secondresult == NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types undef, undef\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type));
    Operation -> anotation = "undef";
    return NULL;
  }

  if(firstresult == NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types undef, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(secondresult));
    Operation -> anotation = "undef";
    return NULL;
  }

  if(secondresult == NULL){
    printf("Line %d, col %d: Operator %s cannot be applied to types %s, undef\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult));
    Operation -> anotation = "undef";
    return NULL;
  }

  if(firstresult != NULL && secondresult != NULL){
    if ( ( strcmp (firstresult,"Int") != 0  && strcmp(firstresult,"Double") != 0  && strcmp(firstresult,"RealLit") != 0 ) ||  ( strcmp (secondresult,"Int") != 0  && strcmp(secondresult,"Double") != 0  && strcmp (secondresult,"RealLit") != 0 )){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Operation->linha_and_col->line,Operation->linha_and_col->col,maius_minusc(Operation->type),maius_minusc(firstresult),maius_minusc(secondresult));
      Operation->anotation = "undef";
      return NULL;
    }
    else if( strcmp (firstresult,"Int") == 0  && strcmp(secondresult,"Int") == 0 ){
      Operation -> anotation = "Int";
      return "Int";
    }
    else{
      Operation->anotation = "Double";
      return "Double";
    }
  }
  return NULL;
}


/* ################################################################# */
/*                            EXPR VERIFIER                          */
/* ################################################################# */

char * check_Expr(Method_tab *new_method,Node *Expr){
  Node * aux_node;
  char * result;
  char * result2;

  aux_node = Expr;


  if(strcmp(aux_node->type,"Id") == 0){
    result = check_VariableTable(new_method,aux_node);
    return result;
  }
  else if(strncmp(aux_node->type,"Assign",6) == 0){
    aux_node = aux_node -> son;

    //cheking if atribuition variable exists in table.
    result = check_Expr(new_method,aux_node); //returns type of the atribuition variable.

    if(result == NULL ){
      result = "undef";
      Expr->anotation = "undef";
    }
    //Checking if Assignment is OK.
    aux_node = aux_node->next;

    result2 = check_Expr(new_method,aux_node);

    if(result2 == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result));
      Expr->anotation = result;
      return result;
    }
    else if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types undef, %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result2));
      return result;
    }
    else{ //Operador e o Assignment forem iguais ou forem Int - Double ou Double-Int
      if (( strcmp(result,result2) == 0 && strcmp(result,"StringArray") != 0 && strcmp (result2,"StringArray") != 0 ) || (strcmp(result,"Double") == 0 && strcmp(result2,"Int") == 0 )){
        Expr->anotation = result;
        return result;
      }
      else{ //incompatible type.
        Expr->anotation = result;
        printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result),maius_minusc(result2));
        return result;
      }
    }
  }

  else if(strcmp(aux_node->type,"DecLit") == 0){
    result = DecLit_Verifier(aux_node->id);
    if(result == NULL){
      aux_node-> anotation = "Int";
      printf("Line %d, col %d: Number %s out of bounds\n",Expr->linha_and_col->line,Expr->linha_and_col->col,aux_node->id);
      return "Int";
    }
    aux_node -> anotation = "Int";
    return "Int";
  }
  else if(strcmp(aux_node->type,"BoolLit") == 0){
    aux_node -> anotation = "Bool";
    return "Bool";
  }
  else if(strcmp(aux_node->type,"RealLit") == 0){
    result = RealLit_Verifier(aux_node->id);
    if(result == NULL){
      aux_node-> anotation = "Double";
      printf("Line %d, col %d: Number %s out of bounds\n",Expr->linha_and_col->line,Expr->linha_and_col->col,aux_node->id);
      return "Double";
    }
    aux_node -> anotation = "Double";
    return "Double";
  }
  else if(strcmp(aux_node->type,"StrLit") == 0){
    aux_node->anotation = "String";
    return "String";
  }
  else if(strcmp(aux_node->type,"Length") == 0){
    result = check_Expr(new_method,aux_node->son);
    if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to type undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type));
      aux_node->anotation = "Int";
      return "Int";
    }
    else if(strcmp(result,"StringArray") == 0 || strcmp(result,"String") == 0){
      aux_node->anotation = "Int";
      return "Int";
    }
    else{
      printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result));
      aux_node->anotation = "Int";
      return "Int";
    }
  }
  else if(strcmp(aux_node->type,"Plus") == 0 || strcmp(aux_node->type,"Minus") == 0){
    result = check_Expr(new_method,aux_node->son);
    if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to type undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type));
      aux_node->anotation = "undef";
      return NULL;
    }
    else{
      if(strcmp(result,"Int") == 0 || strcmp(result,"Double") == 0){
        aux_node->anotation = result;
        return result;
      }
      else{
        printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result));
        aux_node->anotation = "undef";
        return NULL;
      }
    }
  }
  else if(strcmp(aux_node->type,"Not") == 0){
    result = check_Expr(new_method,aux_node->son);
    if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to type undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type));
      aux_node->anotation = "Bool";
      return "Bool";
    }
    else{
      if(strcmp(result,"Bool") == 0 ){
        aux_node->anotation = "Bool";
        return "Bool";
      }
      else{
        printf("Line %d, col %d: Operator %s cannot be applied to type %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result));
        aux_node->anotation = "Bool";
        return "Bool";
      }
    }
  }
  else if(strcmp(aux_node->type,"Call") == 0){

    result = check_MethodVerifier(new_method,aux_node);

    if(result == NULL){
      aux_node->anotation = "undef";
      return NULL;
    }

    return result;

  }
  else if(strcmp(aux_node->type , "ParseArgs") == 0){
    aux_node = aux_node -> son;

    char * aux_name = check_Expr(new_method , aux_node); //checks if variable is in table.
    result = check_Expr(new_method,aux_node->next);

    if(aux_name == NULL && result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types undef, undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type));
      Expr->anotation = "Int";
      return "Int";
    }

    if(aux_name==NULL){ //If aux_name is NULL then the variable was not found in table. Error of type undef.
      printf("Line %d, col %d: Operator %s cannot be applied to types undef, %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(result));
      Expr->anotation = "Int";
      return "Int";
    }
    if(result == NULL){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, undef\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(aux_name));
      Expr->anotation = "Int";
      return "Int";
    }

    if(strcmp(result,"Int") != 0){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(aux_name),maius_minusc(result));
      Expr->anotation = "Int";
      return "Int";
    }

    if(strncmp(aux_name,"StringArray",11) != 0){
      printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n",Expr->linha_and_col->line,Expr->linha_and_col->col,maius_minusc(Expr->type),maius_minusc(aux_name),maius_minusc(result));
      Expr->anotation = "Int";
      return "Int";
    }
    else{
      Expr->anotation = "Int";
      return "Int";
    }
  }
  else if(strncmp(aux_node->type,"Add",3) == 0 || strncmp(aux_node->type,"Sub",3) == 0 || strncmp(aux_node->type,"Mul",3) == 0 || strncmp(aux_node->type,"Div",3) == 0 || strncmp(aux_node->type,"Mod",3) == 0){
    char * result = check_Operations_Int(new_method,aux_node);
    return result;
  }
  //Operation : EQ , NEQ , LT , GT , LEQ , GEQ , OR , AND.
  else if(strcmp (aux_node->type,"Eq") == 0 || strcmp (aux_node->type,"Neq") == 0 || strcmp (aux_node->type,"Lt") == 0 || strcmp (aux_node->type,"Gt") == 0 || strcmp (aux_node->type,"Leq") == 0 || strcmp (aux_node->type,"Geq") == 0 || strcmp (aux_node->type,"Or") == 0 || strcmp (aux_node->type,"And") == 0){
    char *result = check_Operations_Bool(new_method,aux_node);
    return result;
  }


  return "NOTEXPR";
}

/* ################################################################# */
/*                      METHOD EXISTER VERIFIER                      */
/* ################################################################# */

char * check_MethodVerifier(Method_tab *new_method , Node *Method){
  Method_or_var *global_table;
  Method_or_var *retorno;
  Node *aux_node;
  Params_tab *aux_var;
  int counter_params1=0;
  int counter_params2=0;
  int i=0;
  char * result;
  int ambiguos_counter = 0;


  //Counting number of params of Method that was evocated.
  aux_node = Method->son->next;

  while(aux_node != NULL){
    counter_params1++;
    aux_node = aux_node -> next;
  }

  //assigning matrix for number of params.
  char aux_anotation [counter_params1][20];
  char aux_anotation2 [(counter_params1+1)*20];

  aux_node = Method->son->next;
  i=0;


  while(aux_node != NULL){
    result = check_Expr(new_method,aux_node);
    if(result == NULL){
      strcpy(aux_anotation[i],"undef");
    }else{
      strcpy(aux_anotation[i],result);
    }
    i++;
    aux_node = aux_node -> next;
  }

  //reseting aux_node to ID of Method.
  aux_node = Method->son;
  global_table=symbol_table->next;

  while(global_table != NULL){
    if(global_table->methods != NULL){
      if(strcmp(global_table->methods->name,aux_node->id) == 0){ //Method with same name.
        aux_var = global_table->methods->variables;
        counter_params2 = 0;

        //Counter params of the method we testing.
        while(aux_var!= NULL){
          if(aux_var->param_flag == 1) {
            counter_params2++;
          }
          aux_var = aux_var->next;
        }

        //Reseting aux_var to variables.
        aux_var = global_table->methods->variables;

        if(counter_params1 == counter_params2){
          for(i=0;i<counter_params1;i++){
            if(strcmp(aux_var->type, aux_anotation[i]) ==0){
              aux_var = aux_var->next;
            }
            else{
              break;
            }
          }

          if(i==counter_params2 && i==counter_params1){
            aux_var = global_table->methods->variables;
            strcpy(aux_anotation2,"(");
            for(i = 0;i<counter_params1;i++){
              if(i == counter_params1-1){
                strcat(aux_anotation2,maius_minusc(aux_var->type));
              }else{
                strcat(aux_anotation2,maius_minusc(aux_var->type));
                strcat(aux_anotation2,",");
              }
              aux_var = aux_var->next;
            }
            strcat(aux_anotation2,")");
            char * aux_aux_aux = (char*)malloc(sizeof(aux_anotation2));
            memcpy(aux_aux_aux,aux_anotation2,sizeof(aux_anotation2));
            Method->anotation = global_table->methods->type;
            Method->son->anotation = aux_aux_aux;
            return global_table->methods->type;
          }
        }
      }
    }
    global_table = global_table -> next;
  }

  //reseting aux_node to ID of Method.
  aux_node = Method->son;
  global_table=symbol_table->next;

  while(global_table != NULL){
    if(global_table->methods != NULL){
      if(strcmp(global_table->methods->name,aux_node->id) == 0){ //Method with same name.
        aux_var = global_table->methods->variables;
        counter_params2 = 0;

        //Counter params of the method we testing.
        while(aux_var!= NULL){
          if(aux_var->param_flag == 1) {
            counter_params2++;
          }
          aux_var = aux_var->next;
        }

        //Reseting aux_var to variables.
        aux_var = global_table->methods->variables;

        if(counter_params1 == counter_params2){
          for(i=0;i<counter_params1;i++){
            if(strcmp(aux_var->type, aux_anotation[i]) ==0 || ( strcmp(aux_var->type,"Double") == 0 && strcmp(aux_anotation[i],"Int") == 0)){
              aux_var = aux_var->next;
            }
            else{
              break;
            }
          }

          if(i==counter_params2 && i==counter_params1){
            ambiguos_counter++;
            if(ambiguos_counter == 1){
              aux_var = global_table->methods->variables;
              strcpy(aux_anotation2,"(");
              for(i = 0;i<counter_params1;i++){
                if(i == counter_params1-1){
                  strcat(aux_anotation2,maius_minusc(aux_var->type));
                }else{
                  strcat(aux_anotation2,maius_minusc(aux_var->type));
                  strcat(aux_anotation2,",");
                }
                aux_var = aux_var->next;
              }
              strcat(aux_anotation2,")");
              char * aux_aux_aux = (char*)malloc(sizeof(aux_anotation2));
              memcpy(aux_aux_aux,aux_anotation2,sizeof(aux_anotation2));
              Method->anotation = global_table->methods->type;
              Method->son->anotation = aux_aux_aux;
              retorno = global_table;
            }
          }
        }
      }
    }
    global_table = global_table -> next;
  }


  if(ambiguos_counter == 0){
    printf("Line %d, col %d: Cannot find symbol %s(",Method->son->linha_and_col->line,Method->son->linha_and_col->col,maius_minusc(Method->son->id));
    for(i =0 ; i<counter_params1;i++){
      if(i == counter_params1-1){
        printf("%s",maius_minusc(aux_anotation[i]));
      }
      else{
        printf("%s,",maius_minusc(aux_anotation[i]));
      }
    }
    printf(")\n");
    Method->son->anotation = "undef";
    return NULL;
  }
  else if(ambiguos_counter> 1){
    printf("Line %d, col %d: Reference to method %s(",Method->son->linha_and_col->line,Method->son->linha_and_col->col,maius_minusc(Method->son->id));
    for(i =0 ; i<counter_params1;i++){
      if(i == counter_params1-1){
        printf("%s",maius_minusc(aux_anotation[i]));
      }
      else{
        printf("%s,",maius_minusc(aux_anotation[i]));
      }
    }
    printf(") is ambiguous\n");
    Method->son->anotation = "undef";
    Method->anotation = NULL;
    return NULL;
  }
  else{
    return retorno->methods->type;
  }
}

/* ################################################################# */
/*                      VARIABLE EXISTER VERIFIER                    */
/* ################################################################# */


char * check_VariableTable(Method_tab *new_method , Node *atribuition){ //Checks if Variable is in Table , If it is it's then proper type is defined , else undef is defined (AST NOTED).
  Params_tab *aux_table;
  Method_or_var *global_table;

  aux_table = new_method->variables;

  //checking local Variables
  while(aux_table != NULL){
    if(strcmp(aux_table->name ,atribuition->id) == 0){
      atribuition->anotation = aux_table->type;
      return aux_table->type;
    }
    aux_table = aux_table ->next;
  }
  global_table=symbol_table->next;
  //checking global variables
  while(global_table != NULL){
    if(global_table->global_var != NULL){
      if(strcmp(global_table->global_var->name ,atribuition->id) == 0){
        atribuition->anotation=global_table->global_var->type;
        return global_table->global_var->type;
      }
      global_table = global_table -> next;
    }
    else{
      global_table = global_table->next;
    }
  }

  printf("Line %d, col %d: Cannot find symbol %s\n",atribuition->linha_and_col->line,atribuition->linha_and_col->col,atribuition->id);
  atribuition->anotation = "undef";

  return NULL;
}

char * check_VariableTableMethod(Method_tab *new_method , Node *atribuition){ //Checks if Variable is in Table , If it is it's then proper type is defined , else undef is defined (AST NOTED).
  Params_tab *aux_table;
  Method_or_var *global_table;

  aux_table = new_method->variables;

  //checking local Variables
  while(aux_table != NULL){
    if(strcmp(aux_table->name ,atribuition->id) == 0){
      atribuition->anotation = aux_table->type;
      return maius_minusc(aux_table->type);
    }
    aux_table = aux_table ->next;
  }
  global_table=symbol_table->next;
  //checking global variables
  while(global_table != NULL){
    if(global_table->global_var != NULL){
      if(strcmp(global_table->global_var->name ,atribuition->id) == 0){
        atribuition->anotation=global_table->global_var->type;
        return maius_minusc(global_table->global_var->type);
      }
      global_table = global_table -> next;
    }
    else{
      global_table = global_table->next;
    }
  }

  atribuition->anotation = "undef";
  return "undef";
}

/* ################################################################# */
/*                 VARIABLE DECLARATION VERIFIER                     */
/* ################################################################# */

void check_VarDecl(Method_tab *new_method , Node *VarDecl){
  Node *aux_node;
  Method_tab *aux_method;
  Params_tab *aux_var;

  Params_tab *new_Param = (Params_tab *) malloc(sizeof(Params_tab));

  aux_node=VarDecl->son;

  new_Param->type = aux_node->type;

  aux_node = aux_node->next;

  new_Param->name = aux_node->id;

  new_Param->param_flag = 0;

  aux_method = new_method;

  if(aux_method->variables == NULL){
    aux_method->variables = new_Param;
    return;
  }

  aux_var = aux_method -> variables;

  //Check if param is already defined inside method. Does not check global since it's allowed
  while(aux_var->next != NULL){
    if(strcmp(aux_var->name, new_Param->name) == 0){
      printf("Line %d, col %d: Symbol %s already defined\n",aux_node->linha_and_col->line,aux_node->linha_and_col->col ,new_Param->name);
      return;
    }
    else{
      aux_var = aux_var->next;
    }
  }
  //Last element check.
  if(strcmp(aux_var->name, new_Param->name) == 0){
    printf("Line %d, col %d: Symbol %s already defined\n",aux_node->linha_and_col->line,aux_node->linha_and_col->col ,new_Param->name);
    return;
  }

  //if it get's here then it's valid.
  aux_var->next = new_Param;
}

/* ################################################################# */
/*                 TABLE PRINTERS + AUXILIAR FUNCTIONS               */
/* ################################################################# */

void show_table(){
  Params_tab *aux_var;

  if(symbol_table==NULL){
    printf("ERROR: Table is empty! AVL Tree Empty?");
    return;
  }

  printf("===== Class %s Symbol Table =====\n",symbol_table->name);

  Method_or_var *method_or_var = symbol_table->next;

  while(method_or_var != NULL){
    if(method_or_var->global_var != NULL){
      printf("%s\t",method_or_var->global_var->name);
      //printf("[ParamTypes]");
      printf("\t%s",maius_minusc(method_or_var->global_var->type));
      //printf("[\tFlag]");
      printf("\n");
    }
    if(method_or_var->methods != NULL){
      printf("%s\t",method_or_var->methods->name);

      aux_var = method_or_var->methods->variables;
      helper_print_class(aux_var);

      printf("\t%s",maius_minusc(method_or_var->methods->type));

      printf("\n");
    }
    method_or_var = method_or_var -> next;
  }

  method_or_var = symbol_table->next;

  while(method_or_var != NULL){
    if(method_or_var->methods!=NULL){
      printf("\n");
      method_table_print(method_or_var->methods);
    }
    method_or_var = method_or_var ->next;
  }
}

void method_table_print(Method_tab *table){
  Params_tab *aux;

  if(table == NULL){
    printf("ERROR: Method Table is empty!");
    return;
  }

  printf("===== Method %s",table->name);
  helper_print_class(table->variables);
  printf(" Symbol Table =====\n");

  printf("return\t\t%s\n",maius_minusc(table->type));

  aux = table -> variables;
  while(aux!=NULL){
    if(aux->special_flag != 2){
      printf("%s\t",aux->name);
      printf("\t%s",maius_minusc(aux->type));
      if(aux->param_flag==1){
        printf("\tparam");
      }
      printf("\n");
    }
    aux=aux->next;
  }
}

void helper_print_class(Params_tab *aux_var){
  printf("(");
  while(aux_var != NULL){
    if(aux_var->param_flag == 1){
      printf("%s",maius_minusc(aux_var->type));
    }
    aux_var = aux_var->next;
    if(aux_var != NULL && aux_var->param_flag == 1){
      printf(",");
    }
  }
  printf(")");
}

char * maius_minusc(char *name){ //Int -> int || StringArray = String[]
  if(strncmp(name,"StringArray",11) == 0){
    return "String[]";
  }
  else if (strncmp(name,"Int",3) == 0){
    return "int";
  }
  else if(strncmp(name,"Bool",4) == 0){
    return "boolean";
  }
  else if(strncmp(name,"Double",6) == 0){
    return "double";
  }
  else if(strncmp(name,"Void",4) == 0){
    return "void";
  }
  else if(strncmp(name,"undef",5) == 0){
    return "undef";
  }
  else if(strncmp(name,"String",5) == 0){
    return "String";
  }
  else if(strncmp(name,"Return",6) == 0){
    return "return";
  }
  else if(strcmp(name,"If") == 0){
    return "if";
  }
  else if(strcmp(name,"Print") == 0){
    return "System.out.println";
  }
  else if(strcmp(name,"ParseArgs") == 0){
    return "Integer.parseInt";
  }
  else if(strcmp(name,"Assign") == 0){
    return "=";
  }
  else if(strcmp(name,"While") == 0){
    return "while";
  }
  else if(strcmp(name,"DoWhile") == 0){
    return "do";
  }
  else if(strcmp(name,"Add") == 0){
    return "+";
  }
  else if(strcmp(name,"Minus") == 0){
    return "-";
  }
  else if(strcmp(name,"Sub") == 0){
    return "-";
  }
  else if(strcmp(name,"Mul") == 0){
    return "*";
  }
  else if(strcmp(name,"Div") == 0){
    return "/";
  }
  else if(strcmp(name,"Mod") == 0){
    return "%";
  }
  else if(strcmp(name,"Or") == 0){
    return "||";
  }
  else if(strcmp(name,"And") == 0){
    return "&&";
  }
  else if(strcmp(name,"Eq") == 0){
    return "==";
  }
  else if(strcmp(name,"Neq") == 0){
    return "!=";
  }
  else if(strcmp(name,"Lt") == 0){
    return "<";
  }
  else if(strcmp(name,"Gt") == 0){
    return ">";
  }
  else if(strcmp(name,"Leq") == 0){
    return "<=";
  }
  else if(strcmp(name,"Geq") == 0){
    return ">=";
  }
  else if(strcmp(name,"Length") == 0){
    return ".length";
  }
  else if(strcmp(name,"Not") == 0){
    return "!";
  }
  else if(strcmp(name,"Plus") == 0){
    return "+";
  }
  return name;
}

char * DecLit_Verifier(char* declit){
  int tamanho = strlen(declit);
  int i;
  int pos=0;

  char aux [tamanho+1];
  char aux_bounds [10] = "2147483648";

  for(i=0;i<tamanho;i++ ){
    if((declit[i] != '_')){
      aux[pos] = declit[i];
      pos++;
    }
  }
  aux[pos]='\0';

  if(strlen(aux) > 10 || (strlen(aux) > 10 && aux[0] == '-') ){ //"2147483648"
    return NULL;
  }else if(strlen(aux) == 10){
    for(i=0;i<10;i++){
      if(aux[i] > aux_bounds[i] && i<9){
        return NULL;
      }
      else{
        if(aux[i] >= aux_bounds[i] && i==9){
          return NULL;
        }
      }
    }
  }
  else{
    return "something";
  }
  return "something";
}

char * RealLit_Verifier(char *reallit){
  int tamanho = strlen(reallit);
  int i;
  int pos=0;
  int flag=0;


  char aux [tamanho+1];

  //Removing "_"
  for(i=0;i<tamanho;i++ ){
    if((reallit[i] != '_')){
      aux[pos] = reallit[i];
      pos++;
    }
  }
  aux[pos]='\0';

  double d = atof(aux);

  //se o numero de entrada for diferente de 0 e o resultado do atof == 0 -> UNDERFLOW.
  for(i=0;i<strlen(aux);i++){
    if(aux[i] != 'e' && aux[i] != 'E'){
      if(aux[i] != '0' && aux[i] != '.'){
        flag = 1; // ENcontrou numero diferente de 0.0000 ou 0 , etc.
      }
    }
    else{
      break;
    }
  }

  if(d > DBL_MAX || ( d == 0 && flag == 1 )){
    return NULL;
  }
  else{
    return "something";
  }
}
