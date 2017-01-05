//$Id: typecheck.cpp,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#include <string>
#include <unordered_map>
#include <vector>

#include "typecheck.h"
#include "astree.h"
#include "lyutils.h"


symbol_stack identifiers; //Hold idents
symbol_table types;  //Hold types
symbol_stack fields; //Hold fields


size_t blockCount = 0; //Track blocks
size_t nextBlock = 1; //Next block
vector<size_t> blockStack;
extern FILE *outSYM; //.sym file

//Initialize global table
void initialize() 
{
   symbol_table *global = new symbol_table();
   blockStack.push_back(blockCount);
   identifiers.push_back(global);
}

//Enter A  Block
symbol_table *enterBlock() 
{
   blockCount = nextBlock;
   nextBlock++;
   blockStack.push_back(blockCount);
   symbol_table *newBlock = new symbol_table();
   return newBlock;
}

void exitBlock() 
{
   blockStack.pop_back();
}

void dumpSymbol (symbol *inSymbol, string *inName)
{
   if (inSymbol == NULL || inName == NULL) 
           return;   
   size_t depth = inSymbol->blockNum;
   for (size_t i = 0; i < depth; i++) 
           fprintf(outSYM, "\t");
   fprintf(outSYM, "%s (%zu.%zu.%zu) {%zu} %s\n",
           inName->c_str(),
           inSymbol->filenr,
           inSymbol->linenr,
           inSymbol->offset, 
           inSymbol->blockNum,
           attrString(inSymbol->attributes));
}


symbol *newSymbol (astree *sym_node) 
{
   if (sym_node == NULL) return NULL;
   symbol *new_sym = new symbol();
   new_sym->fields = NULL;
   new_sym->filenr = sym_node->filenr;
   new_sym->linenr = sym_node->linenr;
   new_sym->offset = sym_node->offset;
   new_sym->blockNum = blockStack.back();
   new_sym->attributes = sym_node->attributes;
   new_sym->parameters = NULL;
   new_sym->typeId = NULL;
   return new_sym;
}


bool insertSymbol(symbol_table *inTable, 
symbol *inSymbol, string *inName)
{
   if (inTable == NULL || inSymbol == NULL || inName == NULL) 
           return false;
   if (inTable->find(inName) != inTable->end()) 
           return false;
   inTable->insert(make_pair(inName, inSymbol));
   dumpSymbol(inSymbol, inName);
   return true;
}


void newType (astree *struct_nodeIN) 
{
   if (struct_nodeIN == NULL || struct_nodeIN->children.empty()) 
           return;
   astree *struct_name = struct_nodeIN->children[0];
   if (struct_name == NULL) 
           return;
   symbol *type_sym = newSymbol(struct_name);
   insertSymbol(&types, type_sym, (string*)struct_name->lexinfo);
   symbol_table *field_table = new symbol_table();

   bool c = true;
   for (auto &i : struct_nodeIN->children) 
   {
      if (i->symbol == TOK_TYPEID && c) 
      {
         c = false;
         continue;
      }

      if (i->children.empty()) 
           return;
      astree *field_name = i->children[0];
      if (field_name == NULL) 
           return;
      symbol *field = newSymbol(i);
      if (field == NULL) 
           return;
      field->attributes.set(ATTR_field);
      insertSymbol(field_table, field, (string*)field_name->lexinfo);
   }

   fields.push_back(field_table);
   type_sym->fields = field_table;
   struct_nodeIN->visited = true;
   struct_nodeIN->structEntry = new symbol_entry(
           (string*)struct_name->lexinfo, type_sym);
}


void newVar(astree *var_node) 
{

   if (var_node == NULL || var_node->children.empty()) 
           return;
   astree *var_type = var_node->children[0];
   if (var_type == NULL) 
           return;
   astree *var_name = var_type->children[0];
   if (var_name == NULL) 
           return;
   astree *var_val = var_node->children[1];
   if (var_val == NULL) 
           return;
   symbol *var_sym = newSymbol(var_node);
   if (var_sym == NULL) 
           return;
   var_sym->attributes.set(ATTR_variable);
   insertSymbol(identifiers[blockStack.back()],
                 var_sym, (string*)var_name->lexinfo);
   var_node->visited = true;
}


void newBlock(astree *block_node) 
{
   for (auto &i : block_node->children) 
   {
      switch(i->symbol) {
      case (TOK_VARDECL):
         newVar(i);
         break;

      case (TOK_BLOCK): {
         symbol_table *iblock = enterBlock();
         identifiers.push_back(iblock);
         newBlock(i);
         exitBlock();
         break;
      }
      default:
         break;
      }
   }
   block_node->visited = true;
}


void newParam(astree *param, symbol_table *fn_table,
      symbol *fn_symbol) 
{
   if (param == NULL) 
         return;
   if (param->children.empty()) 
         return;
   astree *param_name = param->children[0];
   if (param_name == NULL) 
         return;
   symbol *param_sym = newSymbol(param_name);
   if (param_sym == NULL) 
         return;
   param_sym->attributes.set(ATTR_variable);
   param_sym->attributes.set(ATTR_param);

   fn_symbol->parameters->push_back(param_sym);
   insertSymbol(fn_table, param_sym, (string*)param_name->lexinfo);
   param->visited = true;
}

void newFunc(astree *fn_node) 
{

   if (fn_node == NULL || fn_node->children.empty()) 
         return;
   astree *type_node = fn_node->children[0];
   if (type_node == NULL || type_node->children.empty()) 
         return;
   astree *name_node = type_node->children[0];
   if (name_node == NULL) 
         return;
   astree *params_node = fn_node->children[1];
   if (params_node == NULL) 
         return;
   astree *fnblock_node = fn_node->children[2];
   if (fnblock_node == NULL) 
         return;
   symbol *fn_symbol = newSymbol(fn_node);
   if (fn_symbol == NULL) 
         return;

   fn_symbol->attributes.set(ATTR_function);
   fn_symbol->parameters = new vector<symbol*>();

   dumpSymbol(fn_symbol, (string *)name_node->lexinfo);
   symbol_table *fn_table = enterBlock();
   identifiers.push_back(fn_table);

   for (auto &param_type : params_node->children) 
   {
      newParam(param_type, fn_table, fn_symbol);
   }


   newBlock(fnblock_node);
   exitBlock();
   insertSymbol(identifiers[blockStack.back()],
                 fn_symbol, (string*)name_node->lexinfo);
   fn_node->visited = true;
}




void funcName(astree *root) 
{
   if (root == NULL) 
      return;
   if (root->children.empty()) {
      return;
   } else {
      for (auto &child : root->children) {
         if (child->visited) continue;
         switch(child->symbol) {
            case TOK_STRUCT: newType(child); break;
            case TOK_FUNCTION: newFunc(child); break;
            case TOK_VARDECL: newVar(child); break;
            case TOK_BLOCK: {
               symbol_table *iblock = enterBlock();
               identifiers.push_back(iblock);
               newBlock(child);
               exitBlock();
               break;
            }
         }
         funcName(child);
      }
   }
}


void set_attributes(astree* n) 
{
   if (n == NULL) return;
   DEBUGF('z', "%s\n", get_yytname(n->symbol));
   switch (n->symbol) {
      case TOK_INTCON:                    
         n->attributes.set(ATTR_const);   
      case TOK_INT:
         n->attributes.set(ATTR_int);      
          break;
      case TOK_CHARCON:
         n->attributes.set(ATTR_const);
      case TOK_CHAR:
         n->attributes.set(ATTR_char);     
          break;
      case TOK_STRINGCON:
         n->attributes.set(ATTR_const);
      case TOK_STRING:
         n->attributes.set(ATTR_string);   
          break;
      case TOK_VOID:
         n->attributes.set(ATTR_void);     
          break;
      case TOK_BOOL:
         n->attributes.set(ATTR_bool);     
          break;
      case TOK_STRUCT:
         n->attributes.set(ATTR_struct);   
          break;
      case TOK_NULL:
         n->attributes.set(ATTR_const);    
          break;
      case TOK_FIELD:
         n->attributes.set(ATTR_field);    
          break;
      case TOK_FUNCTION:
         n->attributes.set(ATTR_function); 
          break;
      case TOK_ARRAY:
         n->attributes.set(ATTR_array);    
          break;
      case TOK_VARDECL:
         n->attributes.set(ATTR_variable); 
          break;
      case TOK_PROTOTYPE:
         n->attributes.set(ATTR_proto);    
          break;
      default: break;
   }
}

void set_attributes_rec (astree *root) {
   if (root == NULL) 
       return;
   if (root->children.empty()) {
       return;
   } else {
      for (auto &child : root->children) {
         set_attributes(child);
         set_attributes_rec(child);
      }
   }
}


const char *attrString(attr_bitset attributes) {
   string attrs = "";
   if (attributes.test(ATTR_void))     
        attrs += "void ";
   if (attributes.test(ATTR_bool))     
        attrs += "bool ";
   if (attributes.test(ATTR_char))     
        attrs += "char ";
   if (attributes.test(ATTR_int))      
        attrs += "int ";
   if (attributes.test(ATTR_null))     
        attrs += "null ";
   if (attributes.test(ATTR_string))   
        attrs += "string ";
   if (attributes.test(ATTR_struct))   
        attrs += "struct ";
   if (attributes.test(ATTR_array))    
        attrs += "array ";
   if (attributes.test(ATTR_function)) 
        attrs += "function ";
   if (attributes.test(ATTR_proto))    
        attrs += "prototype ";
   if (attributes.test(ATTR_variable)) 
        attrs += "variable ";
   if (attributes.test(ATTR_field))    
        attrs += "field ";
   if (attributes.test(ATTR_typeid))   
        attrs += "typeid ";
   if (attributes.test(ATTR_param))    
        attrs += "param ";
   if (attributes.test(ATTR_lval))     
        attrs += "lval ";
   if (attributes.test(ATTR_const))    
        attrs += "const ";
   if (attributes.test(ATTR_vreg))     
        attrs += "vreg ";
   if (attributes.test(ATTR_vaddr))    
        attrs += "vaddr ";
   return attrs.c_str();
}

symbol* st_lookup(symbol_table* st,  astree* node){
   string* lexinfo = const_cast<string*>(node->lexinfo);
   if(!st->count(lexinfo))
      return nullptr;
   symbol_entry ent = *st->find(const_cast<string*>(node->lexinfo));
   node->deflinenr = ent.second->linenr;
   node->deffilenr = ent.second->filenr;
   node->defoffset = ent.second->offset;
   return ent.second;
}

void st_insert(symbol_table* st,  astree* node){
   symbol* sym = newSymbol(node);
   symbol_entry ent = symbol_entry(
      const_cast<string*>(node->lexinfo), sym);
   if(st->count(ent.first)){
      st->erase(ent.first);
   }
   st->insert(ent);
}
