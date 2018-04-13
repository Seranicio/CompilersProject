typedef struct line_col{
  int line;
  int col;
  char * string;
}Line_col;

typedef struct node{
  char * id;
  char *type;
  char * anotation;

  Line_col * linha_and_col;

  int undef;

  int skip_checker; // 3 -> skips analizyng method.

  struct node * father; // pai ( ajuda no cleanup ).
  struct node * son;   //filhos diretos.
  struct node * next;  //mesmo nivel (irmaos).
}Node;

Node * New_program(Node *Id,Node *sons);
Node * add_Program(Node *add);

/* Variable Declaration */

Node * new_Decl(char * type_of_decl,Node *id_type, Node *FirstId , Node *Brothers);
Node * new_Decl2(Node * id);

/* Method Declaration */

Node * new_MethodDecl(Node *header , Node * body);
Node * new_MethodHeader(Node *type, Node *id , Node *params );
Node * new_MethodParams(Node *Params);
Node * new_ParamDecl(Node *type_or_stringArray , Node *id, Node *brothers);
Node * aux_ParamDecl(Node *type , Node *id);
Node * new_MethodBody(Node *vardecl_or_statement);

/* Statements */

Node * Statements_2(char *statement_name , Node *left , Node *right);
Node * Statement_If(Node * son , Node *brother1 , Node * brother2);
Node * Statement_Print(Node *son);
Node * Statement_Block ();
Node * aux_Statement(Node *add);
Node * Statement_Return(Node *return1);

/* Operators */

Node * Operators_2(char *operation , Node *left ,Node *right);
Node * Operators_1(char *operation, Node * son);
Node * Operator_Call(Node * id , Node *firstson , Node *brothers);
Node * auxOperator_Call(Node *add);
Node * addbrother(Node *son1,Node *son2);
/* Terminals */

Node * Terminals(char *type , char *id);

/* Tree Printing */

void printing_tree(Node *Program, int pontos);
void testing_nodes(Node *Program);
void print_Program();
void cancel_tree();
void activate_build_flag();
void print_ASTnoted();
void printing_tree2(Node *Program, int pontos);
extern char * maius_minusc(char *name);

/* Adding Line and Colune to Nodes */
Node* add_linecol(Node *no,Line_col *linecol);