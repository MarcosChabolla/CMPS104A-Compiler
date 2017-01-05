//$Id: typecheck.h,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#ifndef __TYPECHECK_H__
#define __TYPECHECK_H__

#include <string>
#include <iostream>
#include <unordered_map>
#include <bitset>
#include <vector>

using namespace std;

//Via ASG4 Spec
enum {  ATTR_void, ATTR_bool, ATTR_char, ATTR_int, ATTR_null,
        ATTR_string, ATTR_struct, ATTR_array, ATTR_function, ATTR_proto,
        ATTR_variable, ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval,
        ATTR_const, ATTR_vreg, ATTR_vaddr, ATTR_bitset_size
};


struct symbol;
using attr_bitset = bitset<ATTR_bitset_size>;
using symbol_table = unordered_map<string*, symbol*>; //via asg4 spec
using symbol_entry = pair<string*,symbol*>;
using symbol_stack = vector<symbol_table*>; // 

#include "astree.h"
#include "auxlib.h"

struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   string *typeId;
   size_t filenr, linenr, offset, blockNum;
   vector<symbol*>* parameters;
   string vreg;
};

//Initialize global table
void initialize(); 

//For function names
void funcName(astree* root);

//For variable names
void varName(astree* root);

//Setting Attributes
void set_attributes_rec(astree *root);

//Return attribute string
const char *attrString(attr_bitset attributes);

//Enter A Block
symbol_table *enterBlock();

//Exit A Block
void exitBlock();

//Dump Symbol To .sym file
void dumpSymbol (symbol *inSymbol, string *inName);

//Create/Initialize a new symbol
symbol *newSymbol (astree *sym_node);

//Insert Symbol To Table
bool insertSymbol(symbol_table *inTable, 
symbol *inSymbol, string *inName);

void newType (astree *struct_nodeIN);

void newVar(astree *var_node);

void newBlock(astree *block_node);

void newParam(astree *param, symbol_table *fn_table,
      symbol *fn_symbol);

void newFunc(astree *fn_node); 

symbol* st_lookup(symbol_table* st,  astree* node);

void st_insert(symbol_table* st,  astree* node);



#endif

