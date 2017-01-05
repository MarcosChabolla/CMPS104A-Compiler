//$Id: astree.h,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <bitset>

using namespace std;
struct astree;
#include "typecheck.h"
#include "auxlib.h"
extern vector<astree*> stringcon_list;

struct astree {
   int symbol;                
   size_t filenr;              
   size_t linenr;             
   size_t offset;               
   const string* lexinfo;
   attr_bitset attributes;
   size_t blockNum;
   symbol_entry* structEntry;
   vector<astree*> children; 
   bool visited = false;
   string vreg;
   size_t deffilenr, deflinenr, defoffset;
};

astree* new_astree (int symbol, int filenr, int linenr, int offset,
                    const char* lexinfo);
astree* change_symbol (astree* root, int symbol);
astree* adopt1 (astree* root, astree* child);
astree* adopt2 (astree* root, astree* left, astree* right);
astree* adopt3 (astree* root, astree* left, astree* mid, astree* right);
astree* adopt2sym(astree* root, astree*left, astree* right, int symbol);
astree* adopt1sym (astree* root, astree* child, int symbol);
void dump_astree (FILE* outfile, astree* root);
void yyprint (FILE* outfile, unsigned short toknum, astree* yyvaluep);
void free_ast (astree* tree);
void free_ast2 (astree* tree1, astree* tree2);
void free_ast3 (astree* tree1, astree* tree2, astree* tree3);

void assign_attrs(astree* ast);

#endif
