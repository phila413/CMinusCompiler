#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/********************************************************************/
// System Includes

#include <vector>
#include <string>
#include <unordered_map>
// For unique_ptr
#include <memory>

/********************************************************************/
// Local Includes

#include "../Parser/CMinusAst.h"

/********************************************************************/
// Using Declarations

using ScopeTable = std::unordered_map<std::string, DeclarationNode*>;

/********************************************************************/

class SymbolTable 
{
public:

  SymbolTable (ProgramNode* pAstTree)
  : astTree(pAstTree), m_nestLevel(-1)
  {
    m_nestLevel = -1;
    enterScope();
  
    // Add input and output functions
    DeclarationNode* input = new DeclarationNode(ValueType::VOID, std::string("input"), DataType::FUNCTION, 0, 0);
    DeclarationNode* output = new DeclarationNode(ValueType::VOID, std::string("output"), DataType::FUNCTION, 0, 0);
    insert(input);
    insert(output);
  } 
 
  // Adjust the nest level; add a new scope table
  void
  enterScope ()
  {
    ++m_nestLevel;
    ScopeTable entryTable;
    std::unique_ptr<ScopeTable> entry (new ScopeTable(entryTable));
    m_table.push_back(std::move(entry));
  }

  // Adjust the nest level; remove most recent scope table
  void
  exitScope ()
  {
    m_table.pop_back();
    --m_nestLevel;
  }

  // Add a (name, declarationPtr) entry to table
  // If successful set nest level in *declarationPtr
  // Return true if successful, false o/w
  bool 
  insert (DeclarationNode* declarationPtr)
  {
    ScopeTable table = *(std::move(m_table.back()));

    // Declaration is not in the table yet
    if (table.find(declarationPtr->identifier) == table.end())
    { 
      std::pair<std::string, DeclarationNode*> decl (declarationPtr->identifier, declarationPtr);
      table.insert(decl); 
      m_table.back().reset(new ScopeTable (table));

      declarationPtr->nestLevel = m_nestLevel;
      
      return true;
    }
    else
    {
      printf("\nERROR: Multiply-declared variable %s (Line: %d; Column: %d)\n\n", declarationPtr->identifier.c_str(), declarationPtr->row, declarationPtr->col);
      exit(1);
      return false;
    }
  }
  
  // Lookup a name corresponding to a Use node
  // Return corresponding declaration pointer on success,
  //   nullptr o/w
  DeclarationNode* 
  lookup (const std::string& name, int row, int col)
  {
    ScopeTable table = *(std::move(m_table[m_table.size()-1]));
    ScopeTable::iterator lookupNode = table.find(name);

    // Check if name is in any of the tables
    for (int itr = m_table.size()-2; lookupNode == table.end() && itr >= 0; --itr)
    {
      table = *(std::move(m_table[itr]));
      lookupNode = table.find(name);
    }
    
    if (lookupNode == table.end())
    {
      printf("\nERROR: Undeclared variable %s (Line: %d; Column: %d)\n\n", name.c_str(), row, col);
      exit(1);
    }
    else
    {
      return lookupNode->second;
    }
  }

  int
  getNestLevel ()
  { 
    return m_nestLevel;
  }

  ProgramNode*
  getTree ()
  {
    return astTree;
  }
  
private:
  ProgramNode* astTree;

  // Current nest level; 0 is global
  int  m_nestLevel;

  // The symbol table is a vector of scope tables
  std::vector<std::unique_ptr<ScopeTable>> m_table;

};

#endif
