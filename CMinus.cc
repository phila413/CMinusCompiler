/*
 Filename   : CMinus.cc
 Author     : Philip Androwick
 Description: Lexical Analyzer, Parser, and Semantic Analyzer for the c- language.
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "Lexer/Lexer.h"
#include "Parser/Parser.h"
#include "SemanticAnalyzer/SymbolTable.h"
#include "SemanticAnalyzer/SymbolTableVisitor.h"
#include "SemanticAnalyzer/SemanticAnalysisVisitor.h"
#include <stdio.h>
#include <deque>

//**

extern FILE* stdin;

FILE*
getInput (int argc, char* argv[]);

//**

int
main (int argc, char* argv[])
{
  ++argv;
  --argc;

  // Run Lexical analyzer
  // IF USING STDIN: finish program by using the $ sign
  Lexer lex (getInput (argc, argv));
  std::deque<Token> tokens;
  Token token;
  do
  {
    token = lex.getToken ();
    tokens.push_back(token);
  } while (token.type != END_OF_FILE);
  
  // Run Parser
  Parser par(tokens);
  ProgramNode* astTree = par.program();
  
  // Create Symbol Table and Check for
  // Undeclared/Multiply declared variables
  // (Phase 1 of Sementic Analysis)
  SymbolTable table(astTree);
  SymbolTableVisitor visitor(&table);
  astTree->accept (&visitor);
  table.exitScope();

  // Run Phase 2 of Semantic Analysis
  SemanticAnalysisVisitor semanticVisitor(&table);
  astTree->accept (&semanticVisitor);

  // Print results in .ast file  
  std::ofstream myfile;
  std::string fileName;
  if (argc > 0)
  {
    std::string fullFileName = argv[0];
    size_t lastindex = fullFileName.find_last_of("."); 
    fileName = fullFileName.substr(0, lastindex); 
    fileName += ".ast";
    myfile.open (fileName.c_str());
  }
  else
  {
    fileName = "Default.ast";
    myfile.open (fileName);
  }
  myfile << par.getAST(astTree);
  myfile.close();

  printf("\nValid!\n");
  printf("Writing AST to \"%s\"\n\n", fileName.c_str());

  return EXIT_SUCCESS;
}

FILE*
getInput (int argc, char* argv[])
{
  if (argc > 0) 
  {
    return fopen (argv[0], "r");
  }
  else
  {
    return stdin;
  }
}