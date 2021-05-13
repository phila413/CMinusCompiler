#include <iostream>
#include "../Parser/CMinusAst.h"
#include "SymbolTable.h"

class IVisitor;
class SymbolTable;

class SymbolTableVisitor : public IVisitor
{ 
public:
	SymbolTableVisitor(SymbolTable* symTab)
	: table (symTab)
	{ }

  virtual void
  visit (ProgramNode* node)
  {
  	for (DeclarationNode* decNode : node->declarations)
      decNode->accept (this);
  }

  virtual void
  visit (DeclarationNode* node)
  {
  }

  virtual void
  visit (VariableDeclarationNode* node)
  {
	table->insert(node);
  }

  virtual void
  visit (FunctionDeclarationNode* node)
  {
  	table->insert(node);

  	table->enterScope();
  	for (ParameterNode* parameter : node->parameters)
      parameter->accept (this);

    node->functionBody->accept(this);
    table->exitScope();
  }

  virtual void
  visit (ArrayDeclarationNode* node)
  {
  	table->insert(node);
  }

  virtual void
  visit (ParameterNode* node)
  {
  	table->insert(node);
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

  	table->enterScope ();
  	node->thenStatement->accept (this);
  	table->exitScope ();

  	if (node->elseStatement != nullptr)
  	{
  		table->enterScope ();
  		node->elseStatement->accept (this);
  		table->exitScope ();
  	}
  }

  virtual void
  visit (WhileStatementNode* node)
  {
  	node->conditionalExpression->accept (this);

  	table->enterScope ();
  	node->body->accept (this);
  	table->exitScope ();
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
  	if (node->variable != nullptr)
      node->variable->accept (this);
    
    // Adds right side of assignment
    if (node->expression != nullptr)
      node->expression->accept (this);
  }

  virtual void
  visit (VariableExpressionNode* node)
  {
  	DeclarationNode* DecNode = table->lookup(node->identifier, node->row, node->col);
  	node->usingDecNode = DecNode;
    // Fix valueType (parser can't accurately know this
    // until it knows what the declaration variable is) 
    node->valueType = DecNode->valueType;
  }

  virtual void
  visit (SubscriptExpressionNode* node)
  {
  	DeclarationNode* DecNode = table->lookup(node->identifier, node->row, node->col);
    node->usingDecNode = DecNode;
    node->valueType = DecNode->valueType;
  	if (node->index != nullptr)
      node->index->accept (this);
  }

  virtual void
  visit (CallExpressionNode* node)
  {
  	DeclarationNode* DecNode = table->lookup(node->identifier, node->row, node->col);
  	node->usingDecNode = DecNode;
    node->valueType = DecNode->valueType;
  	if (!node->arguments.empty ())
      for (ExpressionNode* arg : node->arguments)
      { 
        arg->accept (this);
      }
  }

  virtual void
  visit (AdditiveExpressionNode* node)
  {
  	if (node->left != nullptr)
      node->left->accept (this);
    if (node->right != nullptr)
      node->right->accept (this);
  }

  virtual void
  visit (MultiplicativeExpressionNode* node)
  {
  	if (node->left != nullptr)
      node->left->accept (this);
    if (node->right != nullptr)
      node->right->accept (this);
  }

  virtual void
  visit (RelationalExpressionNode* node)
  {
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

};
