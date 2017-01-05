//$Id: emit.cpp,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#include "emit.h"
#include <cstdlib>
#include <cstring>

//Create sym table for emitting
symbol_table* symtable = new symbol_table;

size_t pointerCount = 0; //Track Pointers
size_t charCount = 0; //Track chars
size_t stringCount = 0; //Track strings
size_t intCount = 0; //Track integers
size_t indpointerCount = 0; //


void indent(FILE* file)
{
   fprintf(file, "        ");
}

void createOIL(FILE* file, astree* root)
{
   for(auto child : root->children)
   {
      if(child->symbol == TOK_STRUCT)
      {
         fprintf(file, mangleStruct(child).c_str());
      }
   }
   
   //To distingush 
   fprintf(file, "void __ocmain (void) {\n");
 
   for(auto node : stringcon_list)
   {
      indent(file);
      emitStringCon(file, node);
   }
  
   for(auto child : root->children)
   {
      if(child->symbol == TOK_VARDECL)
      {
         fprintf(file, "        ");
         child->vreg = mangleIdent(child->children[0]);
         fprintf(file, child->vreg.c_str());
      }
      else if (child->symbol != TOK_FUNCTION 
      && child->symbol != TOK_PROTOTYPE)
      {
        
         emitREC(file, root);
      }
   }
   fprintf(file, "}\n");

   for(auto child : root->children)
   {
      if(child->symbol == TOK_FUNCTION)
      {
         emitFunc(file, child);
      }
   }
}

void emitWhile(FILE* file, astree* node)
{
   fprintf(file, "while_%zu_%zu_%zu:;\n",
      node->filenr, node->linenr, node->offset);

   switch(node->children[0]->symbol)
   {
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emitExpr(file, node->children[0]);
   }

   indent(file);

   //For goto w/break
   fprintf(file,
      "if (!%s) goto break_%zu_%zu_%zu\n", 
      node->children[0]->vreg.c_str(), 
      node->children[1]->filenr,
      node->children[1]->linenr,
      node->children[1]->offset);

   emitNode(file, node->children[1]);

   indent(file);

   //For standard while
   fprintf(file,
      "goto while_%zu_%zu_%zu\n", 
      node->filenr, 
      node->linenr, 
      node->offset);

   //For break
   fprintf(file,
      "break_%zu_%zu_%zu):;\n",
      node->children[1]->filenr,
      node->children[1]->linenr,
      node->children[1]->offset);
}

void emitIF(FILE* file, astree* node)
{
   switch(node->children[0]->symbol)
   {
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emitExpr(file, node->children[0]);
   }
      indent(file);

      //For gotoIF
      fprintf(file,
         "if (!%s) goto fi_%zu_%zu_%zu;\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

      for(auto child : node->children[0]->children)
      {
         indent(file);
         emitNode(file, child);
      }

      fprintf(file,
         "fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

}

string newVreg(char type)
{
   string builder;
   switch(type)
{    
      //For pointers
      case 'p':
         builder += type;
         builder += to_string(++pointerCount);
         break;
      //For characters
      case 'c':
         builder += type;
         builder += to_string(++charCount);
         break;
      //For integers
      case 'i':
         builder += type; 
         builder += to_string(++intCount);
         break;
      //For indPointer
      case 'a':
         builder += type; 
         builder += to_string(++indpointerCount);
         break;
      //For strings
      case 's':
         builder += type;
         builder += to_string(++stringCount);
         break;
      default:
      errprintf("Invalid register type: %c \n", type);
   }
   return builder;
}

void emitIFELSE(FILE* file, astree* node)
{
   switch(node->children[0]->symbol)
   {
      case TOK_IDENT:
      case TOK_CHARCON:
      case TOK_INTCON:
         break;
      default:
         indent(file);
         emitExpr(file, node->children[0]);
   }
      indent(file);

      //For goto Else
      fprintf(file,
         "if (!%s) goto else_%zu_%zu_%zu;\n", 
         node->children[0]->vreg.c_str(), 
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

      for(auto child : node->children[0]->children)
      {
         indent(file);
         emitNode(file, child);
      }

      fprintf(file,
         "goto fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

      fprintf(file,
         "else_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

      for(auto child : node->children[1]->children)
      {
         indent(file);
         emitNode(file, child);
      }

      fprintf(file,
         "fi_%zu_%zu_%zu:;\n",
         node->children[0]->filenr,
         node->children[0]->linenr,
         node->children[0]->offset);

}


void emitBinop(FILE* file, astree* node)
{

   symbol* sym = st_lookup(symtable, node);
   string vreg;

   if(sym != nullptr)
   {
      vreg = sym->vreg;

      fprintf(file, "%s %s %s", 
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->children[1]->vreg.c_str()
            );
   }
   else
   {
      
      fprintf(file, "%s %s %s",
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->children[1]->vreg.c_str()
            );
   }
}


void emitIdent(FILE* file, astree* node)
{
   //printf("Emit ident: %s\n", node->lexinfo->c_str());
   string ident = mangleIdent(node);
   fprintf(file, ident.c_str());
   node->vreg = ident;
}

void emitExpr(FILE* file, astree* node)
{
   if(node->attributes[ATTR_vreg])
   {
      emitBinop(file, node);
   }
   else
   {
      switch(node->symbol)
      {
         case TOK_IDENT:
            emitIdent(file, node);
            break;
         case TOK_INTCON:
         case TOK_CHARCON:
            emitVreg(file, node);
            break;
      }
   }

   fprintf(file, "\n");
}

void emitREC(FILE* file, astree* node)
{
   for(auto child : node->children)
   {
      emitREC(file, child);
   }

   emitNode(file, node);

}

void emitRet(FILE* file, astree* node)
{
   if(node->children.empty())
   {
      indent(file);
      fprintf(file, "return;\n");
   }

   else
   {
      fprintf(file, "return %s;\n", node->children[0]->vreg.c_str());
   }
}

void emitAssign(FILE* file, astree* node)
{
   symbol* sym = st_lookup(symtable, node);
   
   char* type_buf = nullptr;
   indent(file);

   if(node->attributes[ATTR_bool] || node->attributes[ATTR_char])
   {
      node->vreg = newVreg('c');
      type_buf = strdup("char");
   }

   else if(node->attributes[ATTR_int])
   {
      node->vreg = newVreg('i');
      type_buf = strdup("int");
   }

   else if(node->attributes[ATTR_struct])
   {
      node->vreg = newVreg('p');
      type_buf = (char*) node->children[0]->lexinfo->c_str();
   }

   if(sym == nullptr)
   {
      fprintf(file, "%s %s %s %s;\n",
            type_buf, node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(), node->vreg.c_str());
   }
   else
   {
      fprintf(file, "%s %s %s;\n",
            node->children[0]->vreg.c_str(),
            node->lexinfo->c_str(),
            node->vreg.c_str());
   }
}

void emitVreg(FILE* file, astree* node)
{
   
   char* type_buf;
   if(node->attributes[ATTR_bool] || node->attributes[ATTR_char])
   {
      node->vreg = newVreg('c');
      type_buf = strdup("char");
   }

   else if(node->attributes[ATTR_int])
   {
      node->vreg = newVreg('i');
      type_buf = strdup("int");
   }
   else if(node->attributes[ATTR_struct])
   {
      node->vreg = newVreg('p');
      type_buf = (char*) node->children[0]->lexinfo->c_str();
   }

   else
   {
      node->vreg = newVreg('a');
      type_buf = strdup("unknown");
   }

   fprintf(file, "%s %s", 
      type_buf,
      node->vreg.c_str());
   st_insert(symtable, node);

}

void emitCall(FILE* file, astree* node)
{
   
   if(!node->attributes[ATTR_void]){
      emitVreg(file, node);
   }
      fprintf(file, "%s ( ", 
            node->children.back()->lexinfo->c_str());

      for(auto child = node->children.begin(); 
            child != node->children.end()-1; child++)
      {
            fprintf(file, "%s", (*child)->vreg.c_str());

            if(child != node->children.end()-2)
            {
               fprintf(file, ", ");
            }

            else
               fprintf(file, " );\n");
      }

}

void emitNode(FILE* file, astree* node)
{
   switch(node->symbol)
   {
      case TOK_IFELSE:
         emitIFELSE(file, node);
         break;
      case TOK_WHILE:
         emitWhile(file, node);
         break;
      case TOK_RETURN:
         emitRet(file, node); 
      case TOK_IF:
         emitIF(file, node);
         break;
      case TOK_VARDECL:
         emitAssign(file, node);
         break;
      
         break; 
      case TOK_CALL:
         emitCall(file, node);
         break;

     
      case TOK_EQ:
      case TOK_NE:
      case TOK_LE:
      case TOK_LT:
      case TOK_GT:
      case TOK_NEG:
      case TOK_POS:
      case TOK_GE:
      case TOK_INTCON:
      case TOK_CHARCON:
      case TOK_IDENT:
      case '*':
      case '-':
      case '+':
      case '%':
         emitExpr(file, node);
         break; 
   }
}

void emitStringCon(FILE* file, astree* node)
{
   string vreg = newVreg('s');
   node->vreg = vreg;

   fprintf(file, "char* %s = %s;\n", 
      vreg.c_str(), node->lexinfo->c_str());

   st_insert(symtable, node);

}

string mangleStruct(astree* node)
{
   
   string builder;
   builder += "struct s_" + *node->children[0]->lexinfo + "{" + "\n";

   for(auto field = node->children.cbegin()+1; 
         field != node->children.cend(); field++)
   {
      builder += "        ";
      builder += *(*field)->lexinfo;
      builder += " _f";
      builder += *(*field)->children[0]->lexinfo;
      builder += "_";
      builder += *(*field)->children[0]->lexinfo;
      builder += ";\n";
   }
   builder += "};\n";
   return builder;
}



string mangleParam(astree* node)
{
   string builder = *node->lexinfo + " ";
   astree* child = node->children[0];
   builder += "_";
   builder += to_string(child->blockNum);
   builder +="_";
   builder += *child->lexinfo;
   return builder;
}
void emitFunc(FILE* file, astree* node)
{
  
   string builder = "__" + *node->children[0]->lexinfo + " ";
   builder += *node->children[0]->children[0]->lexinfo + " (\n";
   for(auto param : node->children[1]->children)
   {
      builder += "        " + mangleParam(param) + ",\n";
   }

   builder.pop_back();
   builder.pop_back();
   builder += ")\n{\n";
   fprintf(file, builder.c_str());
   for(auto func_block : node->children[2]->children){
      emitREC(file, func_block);
   }
   fprintf(file, "}\n");
  
}

string mangleIdent(astree* node)
{
   string builder = *node->lexinfo + " ";
   astree* child = nullptr;

   if (!node->children.empty())
      child = node->children[0];
   else 
      child = node;

   if (node->blockNum == 0)
   {
      builder += "__";
      builder += *child->lexinfo;
   }

   else
   {
      builder += "_";
      builder += child->blockNum;
      builder +="_";
      builder += *child->lexinfo;
   }

   builder += ";\n";
   return builder;
}



