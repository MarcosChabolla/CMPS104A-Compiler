//$Id: emit.h,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
#include <string>
#include <iostream>
#include "astree.h"
#include "typecheck.h"
#include "lyutils.h"
#include "auxlib.h"

//Base function for creating .oil file
void createOIL(FILE* file, astree* root);

//Emit node to file, based on type
void emitNode(FILE* file, astree* node);

//Emit expressions to file
void emitExpr(FILE* file, astree* node);

//Emit virtual registers to file
void emitVreg(FILE* file, astree* node);

//Emit while statements to file
void emitWhile(FILE* file, astree* node);

//Emit if statements to file
void emitIF(FILE* file, astree* node);

//Emit if else statements
void emitIFELSE(FILE* file, astree* node);

//Emit binary op to file
void emitBinop(FILE* file, astree* node);

//Emit identifiers to file
void emitIdent(FILE* file, astree* node);

//Recursively emit
void emitREC(FILE* file, astree* node);

//Emit return statements to file
void emitRet(FILE* file, astree* node);

//Emit assignment statement to file
void emitAssign(FILE* file, astree* node);

void emitCall(FILE* file, astree* node);

//Emit string constant to file
void emitStringCon(FILE* file, astree* node);

//Emit function to file
void emitFunc(FILE* file, astree* node);

//Mangle parameters to avoid global/scope clashes
string mangleParam(astree* node);

//Mangle structs to avoid global/scope clashes
string mangleStruct(astree* node);

//Mangle identifiers to avoid global/scope clashes
string mangleIdent(astree* node);

//New virtual register
string newVreg(char type);


