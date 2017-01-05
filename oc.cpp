//$Id: oc.cpp,v 1.1 2015/06/04 06:18:20 mchaboll Exp $
// Marcos Chabolla
// mchaboll@ucsc.edu
// ID: 1437530
// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

#include <iostream>
#include <getopt.h>
#include <fstream>
#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxlib.h"
#include "stringset.h"
#include "astree.h"
#include "lyutils.h"
#include "emit.h"

const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;
extern FILE* tokFile;
FILE* outSYM;


int main (int argc, char** argv) {
  
    char * fileName = argv[argc-1]; //input file name is set to string
    char *argument = NULL;
    string ocExtension = ".oc"; //file extension
    string tempStr = (fileName); //C++ string version of fileName
    int test = tempStr.find(ocExtension);
    yy_flex_debug = 0;
    yydebug = 0;

    if(argc<=1)
    {
        cerr<<"Error! Not Enough Arguments. Proper Usage is:"<<endl;
        cerr<<"    oc [-ly] [-@flag...] [-D string] program.oc"<<endl;
        cerr<<"EXITING"<<endl;
        return 0;
    }

    if(test==-1) //Of the file extension is not '.oc'
    {
        cerr<<"Invalid file! File must have extension '.oc'"<<endl;
        cerr<<"EXITING"<<endl;
    }

    set_execname(fileName);
    string stripped;
    string tokName;
    string astName;
    string symName;
    string oilName;
    char *bname;
    string command = CPP + " " + fileName; // /usr/bin/cpp fileName.oc
    int option = 0;
    
     
    
    
    while((option = getopt(argc,argv,"lyD:@:")) != -1) //getopt setup
   {
       switch(option)
      {
            case 'l' :  yy_flex_debug = 1;
            break;

            case 'y' : yydebug = 1;
            break;

            case 'D' : argument=optarg;
            break;
            
            case '@': set_debugflags(optarg);
            break;
      }
   }


  if(argument != NULL)
  {
    command = CPP + " -D" + argument + " " + fileName;       
  }
         
   
   bname = basename(fileName); //Strip off basename of file
    string fileName2(bname); 
    int found = fileName2.find(".");//Find '.' character
    stripped = fileName2.substr(0,found); //Read up to the '.' character
    tokName = fileName2.substr(0,found);
    astName = fileName2.substr(0,found);
    symName = fileName2.substr(0,found);
    oilName = fileName2.substr(0,found);
    stripped = stripped + ".str"; //Append .str extension
    tokName = tokName + ".tok";
    astName = astName + ".ast";
    symName = symName + ".sym";
    oilName = oilName + ".oil";


  
   yyin = popen(command.c_str(),"r");
   
   if (yyin == NULL) 
   {
     syserrprintf (command.c_str());
     exit (get_exitstatus());
   }
   
   opentokFile(tokName); //Open .tok file for writing
   yyparse();
  

 ofstream outFile;
   outFile.open(stripped);
   dump_stringset (outFile); //Dump the CPP output to file
   outFile.close();
    
   closetokFile();
   
   outSYM = fopen(symName.c_str(),"w");
   initialize();
   set_attributes_rec(yyparse_astree);
   funcName(yyparse_astree);
   fclose(outSYM);     
  

   FILE* outAST = fopen(astName.c_str(),"w"); //Open .ast file
   dump_astree(outAST,yyparse_astree); //Dump ast to file
   fclose(outAST);

   FILE* outOIL = fopen(oilName.c_str(),"w");
   createOIL(outOIL, yyparse_astree);
   fclose(outOIL);
    
   yylex_destroy();
    
  
    return 0;
}

