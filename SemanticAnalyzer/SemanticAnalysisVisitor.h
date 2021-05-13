#include <iostream>
#include "../Parser/CMinusAst.h"
#include "SymbolTable.h"

class IVisitor;
class SymbolTable;

class SemanticAnalysisVisitor : public IVisitor
{ 
public:
	SemanticAnalysisVisitor(SymbolTable* symTab)
	: table (symTab), foundMain(false)
	{ }

  virtual void
  visit (ProgramNode* node)
  {
  	for (DeclarationNode* decNode : node->declarations)
      decNode->accept (this);

    if (!foundMain)
    {
      printf("\nERROR: \"main\" function was never declared\n\n");
      exit(1);
    }
  }

  virtual void
  visit (DeclarationNode* node)
  {
  }

  virtual void
  visit (VariableDeclarationNode* node)
  {
    if (node->valueType == ValueType::VOID)
    {
      printf("\nERROR: Declared variable \"%s\" as void (Line: %d; Column %d)\n\n", node->identifier.c_str(), node->row, node->col);
      exit(1);
    }
  }

  virtual void
  visit (FunctionDeclarationNode* node)
  {
    bool foundReturn = false;
    for (StatementNode* statement : node->functionBody->statements)
    {
      ReturnStatementNode* newStatement = dynamic_cast<ReturnStatementNode*>(statement);

      // newStatement is a return statement
      if (newStatement != nullptr)
      {
        if (node->valueType == ValueType::VOID && newStatement->expression != nullptr)
        {
          printf("\nERROR: Returning a value from a void function (Line: %d, Column: %d)\n\n", newStatement->expression->row, newStatement->expression->col);
          exit(1);
        }
        else if (node->valueType == ValueType::INT)
        {
          // Not returning an int value
          if (newStatement->expression->valueType != node->valueType)
          {
            printf("\nERROR: Returning a void value from a non-void function (Line: %d, Column: %d)\n\n", newStatement->expression->row, newStatement->expression->col);
            exit(1);
          }
          // Returning an int value
          else
          {
            foundReturn = true;
          }
        }
      }
    }
    if (node->valueType == ValueType::INT && !foundReturn)
    {
      printf("\nERROR: Not returning a value from a non-void function (Line: %d, Column: %d)\n\n", node->row, node->col);
      exit(1);
    }
    if (foundMain)
    {
      printf ("\nERROR: \"main\" function was not declared last\n\n");
      exit(1);
    }
    if (node->identifier == "main")
    {
      foundMain = true;
    }
    
  	for (ParameterNode* parameter : node->parameters)
      parameter->accept (this);

    node->functionBody->accept(this);
  }

  virtual void
  visit (ArrayDeclarationNode* node)
  {
    if (node->valueType == ValueType::VOID)
    {
      printf("\nERROR: Declared array variable \"%s\" as void (Line: %d; Column %d)\n\n", node->identifier.c_str(), node->row, node->col);
      exit(1);
    }
  }

  virtual void
  visit (ParameterNode* node)
  {
    if (node->valueType == ValueType::VOID)
    {
      printf("\nERROR: Declared parameter \"%s\" as void (Line: %d; Column %d)\n\n", node->identifier.c_str(), node->row, node->col);
      exit(1);
    }
  }

  virtual void
  visit (StatementNode* node)
  {
  }

  virtual void
  visit (CompoundStatementNode* node)
  {
  	for (VariableDeclarationNode* varDec : node->localDeclarations)
  	  varDec->accept (this);

  	for (StatementNode* statement : node->statements)
  	  statement->accept (this);
  }

  virtual void
  visit (IfStatementNode* node)
  {
  	node->conditionalExpression->accept (this);

  	node->thenStatement->accept (this);

  	if (node->elseStatement != nullptr)
  		node->elseStatement->accept (this);
  }

  virtual void
  visit (WhileStatementNode* node)
  {
  	node->conditionalExpression->accept (this);

  	node->body->accept (this);
  }

  virtual void
  visit (ForStatementNode* node)
  {
  }

  virtual void
  visit (ReturnStatementNode* node)
  {
  	if (node->expression != nullptr)
      node->expression->accept (this);
  }

  virtual void
  visit (ExpressionStatementNode* node)
  {
  	if (node->expression != nullptr)
      node->expression->accept (this);
  }

  virtual void
  visit (ExpressionNode* node)
  {
  }

  virtual void
  visit (AssignmentExpressionNode* node)
  {
    DeclarationNode* useNode = node->variable->usingDecNode;

    // Declaration is an array, but the use is not subscripting
    if (useNode->dataType == DataType::ARRAY && node->variable->dataType != DataType::ARRAY)
    {
      printf("\nERROR: Assigning a value to \"%s\" with no subscript (Line: %d; Column: %d)\n", useNode->identifier.c_str(), node->variable->row, node->variable->col);
      printf("       - Variable declared back on (Line %d; Column: %d)\n\n", useNode->row, useNode->col);
      exit(1);
    }
    // Assigning to a function name
    else if (useNode->dataType == DataType::FUNCTION)
    {
      printf("\nERROR: Assigning a value to the function \"%s\" (Line: %d; Column: %d)\n", useNode->identifier.c_str(), node->variable->row, node->variable->col);
      printf("       - Variable declared back on (Line %d; Column: %d)\n\n", useNode->row, useNode->col);
      exit(1);
    }
    
  	if (node->variable != nullptr)
      node->variable->accept (this);
    
    // Adds right side of assignment
    if (node->expression != nullptr)
      node->expression->accept (this); 
  }

  virtual void
  visit (VariableExpressionNode* node)
  {
    
  }

  virtual void
  visit (SubscriptExpressionNode* node)
  {
  	if (node->index != nullptr)
    {
      if (node->usingDecNode->dataType != DataType::ARRAY)
      {
        printf("\nERROR: Subscripting \"%s\", which is not an array (Line: %d; Column: %d)\n\n", node->identifier.c_str(), node->row, node->col);
        exit(1);
      }
      node->index->accept (this);
    }
  }

  virtual void
  visit (CallExpressionNode* node)
  {
    if (node->identifier == "output" || node->identifier == "input")
    {
      if (node->arguments.size() != 1)
      {
        printf("\nERROR: More than one parameter for input/output (Line: %d; Column: %d)\n\n", node->row, node->col);
        exit(1);
      }
    }
    else
    {
      if (node->usingDecNode->dataType != DataType::FUNCTION)
      {
        printf ("\nERROR: \"%s\" is not a function (Line: %d; Column: %d)\n\n", node->identifier.c_str(), node->row, node->col);
        exit(1);
      }
      FunctionDeclarationNode* DecNode = dynamic_cast<FunctionDeclarationNode*>(node->usingDecNode);
      
      if (node->arguments.size() != DecNode->parameters.size())
      {
        printf("\nERROR: Number of parameters does not match the number of arguments from the declaration (Line: %d; Column: %d)\n\n", node->row, node->col);
        exit(1);
      }
      
      for (size_t n = 0; n < node->arguments.size(); ++n)
        if (node->arguments[n]->valueType != DecNode->parameters[n]->valueType)
        {
          printf("\nERROR: Parameter\'s type does not match the argument type (Line: %d; Column: %d)\n\n", node->arguments[n]->row, node->arguments[n]->col);
          exit(1);
        }
    }
    
    
  	if (!node->arguments.empty ())
      for (ExpressionNode* arg : node->arguments)
      {
          arg->accept (this);
      }
  }

  virtual void
  visit (AdditiveExpressionNode* node)
  {
    if (node->left->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->left->row, node->left->col);
      exit(1);
    }
    else if (node->right->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->right->row, node->right->col);
      exit(1);
    }

  	if (node->left != nullptr)
      node->left->accept (this);
    if (node->right != nullptr)
      node->right->accept (this);
  }

  virtual void
  visit (MultiplicativeExpressionNode* node)
  {
    if (node->left->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->left->row, node->left->col);
      exit(1);
    }
    else if (node->right->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->right->row, node->right->col);
      exit(1);
    }
    
  	if (node->left != nullptr)
      node->left->accept (this);
    if (node->right != nullptr)
      node->right->accept (this);
  }

  virtual void
  visit (RelationalExpressionNode* node)
  {
    if (node->left->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->left->row, node->left->col);
      exit(1);
    }
    else if (node->right->valueType != ValueType::INT)
    {
      printf("\nERROR: An operator is being applied to a non-integer type (Line: %d; Column: %d)\n", node->right->row, node->right->col);
      exit(1);
    }
    
  	if (node->left != nullptr)
      node->left->accept (this);
    if (node->right != nullptr)
      node->right->accept (this);
  }

  virtual void
  visit (UnaryExpressionNode* node)
  {
  }

  virtual void
  visit (IntegerLiteralExpressionNode* node)
  {
  }

  SymbolTable* table;
  bool foundMain;
};
