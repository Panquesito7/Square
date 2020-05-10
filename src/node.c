#include "square.h"
#include "node.h"
#include "parse.tab.h"
#include <stdlib.h>
#include <string.h>

#define SQU_ERROR_RUNTIME 0
#define SQU_ERROR_RETURN 1

static char*
strdup0(const char *s)
{
  size_t len = strlen(s);
  char *p;

  p = (char*)malloc(len+1);
  if (p) {
    strcpy(p, s);
  }
  return p;
}

static char*
strndup0(const char *s, size_t n)
{
  size_t i;
  const char *p = s;
  char *new;

  for (i=0; i<n && *p; i++,p++)
    ;
  new = (char*)malloc(i+1);
  if (new) {
    memcpy(new, s, i);
    new[i] = '\0';
  }
  return new;
}

node*
node_value_new(node* v)
{
  /* TODO */
  return NULL;
}

squ_array*
squ_array_new()
{  
  squ_array* arr = malloc(sizeof(squ_array));
  arr->len = 0;
  arr->max = 0;
  arr->data = NULL;
  return arr;
}

void
squ_array_add(squ_array* arr, void* data) 
{
  if (arr->len == arr->max) {
    arr->max = arr->len + 10;
    arr->data = realloc(arr->data, sizeof(void*) * arr->max);
  }

  arr->data[arr->len] = data;
  arr->len++;
}

node*
node_array_new()
{

  node* np = malloc(sizeof(node));
  if(!(np = malloc(sizeof(node))))
  {
    printf("could not malloc for node_array area\n");
  }
  np->type = NODE_VALUE;
  np->value.t = SQU_VALUE_ARRAY;
  np->value.v.p = squ_array_new();
  return np;
}

node*
node_array_of(node* np)
{
  if (np == NULL)
    np = node_array_new();
  return np;
}

void
node_array_add(node* arr, node* np)
{
  node_array* arr0 = arr->value.v.p;
  squ_array_add(arr0, np);
}

void
node_array_free(node* np)
{
  int i;
  node_array* arr = np->value.v.p;
  for (i = 0; i < arr->len; i++)
    node_free(arr->data[i]);
  free(arr);
  free(np);
}

node*
node_pair_new(node* key, node* value)
{
  node_pair* npair = malloc(sizeof(node_pair));
  npair->key = key;
  npair->value = value;

  node* np = malloc(sizeof(node));
  np->type = NODE_PAIR;
  np->value.t = SQU_VALUE_USER;
  np->value.v.p = npair;
  return np;
}

node*
node_map_new()
{
  node_array* arr = malloc(sizeof(node_array));
  /* TODO: error check */
  arr->len = 0;
  arr->max = 0;
  arr->data = NULL;

  node* np = malloc(sizeof(node));
  np->type = NODE_VALUE;
  np->value.t = SQU_VALUE_MAP;
  np->value.v.p = arr;
  return np;
}

node*
node_map_of(node* np)
{
  if (np == NULL)
    np = node_map_new();
  return np;
}

void
node_map_free(node* np)
{
  int i;
  node_array* arr = np->value.v.p;
  for (i = 0; i < arr->len; i++) {
    node* pair = arr->data[i];
    node_pair* npair = pair->value.v.p;
    node_free(npair->key);
    node_free(npair->value);
    free(npair);
    free(pair);
  }
  free(arr);
  free(np);
}

node*
node_let_new(node* lhs, node* rhs)
{
  node_let* nlet = malloc(sizeof(node_let));
  nlet->lhs = lhs;
  nlet->rhs = rhs;
  nlet->type = NODE_LET;
  return (node*)nlet;
}

node*
node_op_new(char* op, node* lhs, node* rhs)
{
  node_op* nop = malloc(sizeof(node_op));
  nop->lhs = lhs;
  nop->op = node_ident_of(op);
  nop->rhs = rhs;
  nop->type = NODE_OP;
  return (node*)nop;
}

node*
node_block_new(node* args, node* stmt_seq)
{
  node_block* block = malloc(sizeof(node_block));
  block->args = args;
  block->stmt_seq = stmt_seq;
  block->type = NODE_BLOCK;
  return (node*)block;
}

node*
node_call_new(node* cond, node* ident, node* args, node* blk)
{
  node_call* ncall = malloc(sizeof(node_call));
  ncall->cond = cond;
  ncall->ident = ident;
  ncall->args = args;
  ncall->blk = blk;
  ncall->type = NODE_CALL;
  return (node*)ncall;
}

node*
node_double_new(squ_double d)
{
  node* np = malloc(sizeof(node));
  np->type = NODE_VALUE;
  np->value.t = SQU_VALUE_DOUBLE;
  np->value.v.d = d;
  return np;
}

node*
node_int_new(squ_int i)
{
  node* np = malloc(sizeof(node));
  np -> type = NODE_VALUE;
  np -> value.t = SQU_VALUE_INT;
  np -> value.v.i = i;
  return np;
}

node*
node_string_new(squ_string s)
{
  node* np = malloc(sizeof(node));
  np->type = NODE_VALUE;
  np->value.t = SQU_VALUE_STRING;
  np->value.v.s = strdup0(s);
  return np;
}

node*
node_string_len_new(squ_string s, size_t l)
{
  node* np = malloc(sizeof(node));
  np->type = NODE_VALUE;
  np->value.t = SQU_VALUE_STRING;
  np->value.v.s = strndup0(s, l);
  return np;
}

node*
node_ident_new(squ_id id)
{
  node* np = malloc(sizeof(node));
  np->type = NODE_IDENT;
  np->value.t = SQU_VALUE_FIXNUM;
  np->value.v.id = id;
  return np;
}

squ_id
node_ident_of(char* s)
{
  /* TODO: get id of the identifier which named as s */
  return (squ_id) strdup0(s);
}

node*
node_null()
{
  static node nd = { NODE_VALUE, { SQU_VALUE_NULL, {0} } };
  return &nd;
}

node*
node_true()
{
  static node nd = { NODE_VALUE, { SQU_VALUE_BOOL, {1} } };
  return &nd;
}

node*
node_false()
{
  static node nd = { NODE_VALUE, { SQU_VALUE_BOOL, {0} } };
  return &nd;
}

node*
node_if_new(node* cond, node* stmt_seq, node* opt_else)
{
  node_if* nif = malloc(sizeof(node_if));
  nif->cond = cond;
  nif->stmt_seq = stmt_seq;
  nif->opt_else = opt_else;
  nif->type = NODE_IF;
  return (node*)nif;
}

node*
node_return_new(node* value)
{
  node* np = malloc(sizeof(node));
  np->type = NODE_RETURN;
  np->value.t = SQU_VALUE_USER;
  np->value.v.p = value;
  return np;
}

node*
node_import_new(squ_id name)
{
  node_import* nimp = malloc(sizeof(nimp));
  nimp -> name = name;
  nimp -> type = NODE_IMPORT;
  return (node*)nimp;
}

node*
node_break_new(node* value)
{
  static node nd = { NODE_BREAK };
  return &nd;
}