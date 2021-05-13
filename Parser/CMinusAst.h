#ifndef C_MINUS_AST_H
#define C_MINUS_AST_H

/********************************************************************/
// System Includes

#include <string>
#include <vector>
#include <map>

/********************************************************************/
// Using Declarations

using std::string;
using std::vector;

/********************************************************************/
// Forward Class Declarations

// Visitor
class IVisitor;

// Abstract Node
struct Node;

// Root Node
struct ProgramNode;

// Declaration Nodes
struct DeclarationNode;
struct FunctionDeclarationNode;
struct ParameterNode;
struct VariableDeclarationNode;
struct ArrayDeclarationNode;

// Statement Nodes
struct StatementNode;
struct CompoundStatementNode;
struct IfStatementNode;
struct WhileStatementNode;
struct ForStatementNode;
struct ReturnStatementNode;
struct ExpressionStatementNode;

// Expression Nodes
struct ExpressionNode;
struct AssignmentExpressionNode;
struct VariableExpressionNode;
struct SubscriptExpressionNode;
struct CallExpressionNode;
struct AdditiveExpressionNode;
struct MultiplicativeExpressionNode;
struct RelationalExpressionNode;
struct UnaryExpressionNode;
struct IntegerLiteralExpressionNode;

/********************************************************************/
//  Enum Classes

enum class AdditiveOperatorType
{
  PLUS, MINUS
};

enum class MultiplicativeOperatorType
{
  TIMES, DIVIDE
};

enum class RelationalOperatorType
{
  LT, LTE, GT, GTE, EQ, NEQ
};

enum class UnaryOperatorType
{
  INCREMENT, DECREMENT
};

enum class ValueType
{
  VOID, INT, ARRAY
};

enum class DataType
{
  VARIABLE, FUNCTION, ARRAY, PARAMETER
};

/********************************************************************/
// Abstract Classes

class IVisitor
{
public:
  virtual void visit (ProgramNode* node) = 0;

  virtual void visit (DeclarationNode* node) = 0;
  virtual void visit (FunctionDeclarationNode* node) = 0;
  virtual void visit (VariableDeclarationNode* node) = 0;
  virtual void visit (ArrayDeclarationNode* node) = 0;
  virtual void visit (ParameterNode* node) = 0;

  virtual void visit (StatementNode* node) = 0;
  virtual void visit (CompoundStatementNode* node) = 0;
  virtual void visit (IfStatementNode* node) = 0;
  virtual void visit (WhileStatementNode* node) = 0;
  virtual void visit (ForStatementNode* node) = 0;
  virtual void visit (ReturnStatementNode* node) = 0;
  virtual void visit (ExpressionStatementNode* node) = 0;

  virtual void visit (ExpressionNode* node) = 0;
  virtual void visit (AssignmentExpressionNode* node) = 0;
  virtual void visit (VariableExpressionNode* node) = 0;
  virtual void visit (SubscriptExpressionNode* node) = 0;
  virtual void visit (CallExpressionNode* node) = 0;
  virtual void visit (AdditiveExpressionNode* node) = 0;
  virtual void visit (MultiplicativeExpressionNode* node) = 0;
  virtual void visit (RelationalExpressionNode* node) = 0;
  virtual void visit (UnaryExpressionNode* node) = 0;
  virtual void visit (IntegerLiteralExpressionNode* node) = 0;
};

struct Node
{
  virtual ~Node ()
  { }

  virtual void
  accept (IVisitor* visitor) = 0;
};

struct ProgramNode : Node
{
  ProgramNode (vector<DeclarationNode*> pDeclarations)
    : declarations (pDeclarations)
  { }

  virtual ~ProgramNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  vector<DeclarationNode*> declarations;
};

/********************************************************************/
// DECLARATION NODES

struct DeclarationNode : Node
{
  DeclarationNode (ValueType t, string id, DataType pDataType, int pRow, int pCol)
    : valueType (t), identifier (id), dataType(pDataType), row(pRow), col(pCol)
  { }

  virtual ~DeclarationNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ValueType valueType;
  string identifier;
  DataType dataType;

  // Set when the symbol table is built
  // Used for code gen
  int nestLevel;
  int row;
  int col;
};

struct FunctionDeclarationNode : DeclarationNode
{
  FunctionDeclarationNode (ValueType t, string id,
    vector<ParameterNode*> params, CompoundStatementNode* body, int pRow, int pCol)
    : DeclarationNode (t, id, DataType::FUNCTION, pRow, pCol), parameters (params), functionBody (body)
  { }

  virtual ~FunctionDeclarationNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  vector<ParameterNode*> parameters;
  CompoundStatementNode* functionBody;
};

struct VariableDeclarationNode : DeclarationNode
{
  VariableDeclarationNode (ValueType t, string id, DataType pDataType, int pRow, int pCol)
    : DeclarationNode (t, id, pDataType, pRow, pCol)
  { }

  virtual ~VariableDeclarationNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  // Used for code gen
  // Parameters and local variables are stored at addresses
  //   offset from the frame pointer (ebp)
  int framePointerOffset;
};

struct ArrayDeclarationNode : VariableDeclarationNode
{
  ArrayDeclarationNode (ValueType t, string id, size_t pSize, int pRow, int pCol)
    : VariableDeclarationNode (t, id, DataType::ARRAY, pRow, pCol), size (pSize)
  { }

  virtual ~ArrayDeclarationNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  size_t size;
};

struct ParameterNode : DeclarationNode
{
  ParameterNode (ValueType t, string id, bool pIsArray, int pRow, int pCol)
    : DeclarationNode (t, id, DataType::PARAMETER, pRow, pCol), isArray (pIsArray)
  { }

  virtual ~ParameterNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  bool isArray;
};

/********************************************************************/
// STATEMENT NODES

struct StatementNode : Node
{
  virtual ~StatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }
};

struct CompoundStatementNode : StatementNode
{
  CompoundStatementNode (vector<VariableDeclarationNode*> decls,
			 vector<StatementNode*> stmts)
    : localDeclarations (decls), statements (stmts)
  { }

  virtual ~CompoundStatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  vector<VariableDeclarationNode*> localDeclarations;
  vector<StatementNode*> statements;
};

struct IfStatementNode : StatementNode
{
  IfStatementNode (ExpressionNode* expr,
                   StatementNode* thenStmt,
                   StatementNode* elseStmt = nullptr)
    : conditionalExpression (expr), thenStatement (thenStmt), elseStatement (elseStmt)
  { }

  virtual ~IfStatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ExpressionNode* conditionalExpression;
  StatementNode* thenStatement;
  StatementNode* elseStatement;
};

struct WhileStatementNode : StatementNode
{
  WhileStatementNode (ExpressionNode* expr, StatementNode* stmt)
    : conditionalExpression (expr), body (stmt)
  { }

  virtual ~WhileStatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ExpressionNode* conditionalExpression;
  StatementNode* body;
};

struct ForStatementNode : StatementNode
{
  ForStatementNode (ExpressionNode* e1,
                    ExpressionNode* e2,
                    ExpressionNode* e3,
                    StatementNode* s);

  ~ForStatementNode ();

  void
  accept (IVisitor* visitor);

  ExpressionNode* initializer;
  ExpressionNode* condition;
  ExpressionNode* updater;
  StatementNode*  body;
};

struct ReturnStatementNode : StatementNode
{
  ReturnStatementNode (ExpressionNode* expr = nullptr)
    : expression (expr)
  { }

  virtual ~ReturnStatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ExpressionNode* expression;
};

struct ExpressionStatementNode : StatementNode
{
  ExpressionStatementNode (ExpressionNode* expr)
    : expression (expr)
  { }

  virtual ~ExpressionStatementNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ExpressionNode* expression;
};

/********************************************************************/
// EXPRESSION NODES

struct ExpressionNode : Node
{
  ExpressionNode(ValueType pValueType, int pRow, int pCol)
    : valueType(pValueType), row(pRow), col(pCol)
  { }

  virtual ~ExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }
  
  ValueType valueType;
  int row;
  int col;
};

struct AssignmentExpressionNode : ExpressionNode
{
  AssignmentExpressionNode (ValueType pValueType, VariableExpressionNode* var,
			    ExpressionNode* expr, int pRow, int pCol)
    : ExpressionNode(pValueType, pRow, pCol), variable (var), expression (expr)
  { }

  virtual ~AssignmentExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  VariableExpressionNode* variable;
  ExpressionNode* expression;
};

struct VariableExpressionNode : ExpressionNode
{
  VariableExpressionNode (string id, ValueType pValueType, DataType pdataType, int pRow, int pCol)
    : ExpressionNode(pValueType, pRow, pCol), identifier (id), dataType(pdataType)
  { }

  virtual ~VariableExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  string identifier;
  DataType dataType;
  DeclarationNode* usingDecNode;
};

struct SubscriptExpressionNode : VariableExpressionNode
{
  SubscriptExpressionNode (string id, ExpressionNode* pIndex, ValueType pValueType, int pRow, int pCol)
    : VariableExpressionNode (id, pValueType, DataType::ARRAY, pRow, pCol), index (pIndex)
  { }

  virtual ~SubscriptExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  ExpressionNode* index;
};

struct CallExpressionNode : ExpressionNode
{
  CallExpressionNode (string id, vector<ExpressionNode*> args, ValueType pValueType, int pRow, int pCol)
    : ExpressionNode(pValueType, pRow, pCol), identifier (id), arguments (args)
  { }

  virtual ~CallExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  string identifier;
  vector<ExpressionNode*> arguments;
  DeclarationNode* usingDecNode;
};

struct AdditiveExpressionNode : ExpressionNode
{
  AdditiveExpressionNode (AdditiveOperatorType addop,
			  ExpressionNode* lhs,
			  ExpressionNode* rhs, int pRow, int pCol)
    : ExpressionNode(ValueType::INT, pRow, pCol), addOperator (addop), left (lhs), right (rhs)
  { }

  virtual ~AdditiveExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  AdditiveOperatorType addOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};

struct MultiplicativeExpressionNode : ExpressionNode
{
  MultiplicativeExpressionNode (MultiplicativeOperatorType multop,
                                        ExpressionNode* lhs,
                                        ExpressionNode* rhs, int pRow, int pCol)
    : ExpressionNode(ValueType::INT, pRow, pCol), multOperator (multop), left (lhs), right (rhs)
  { }

  virtual ~MultiplicativeExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  MultiplicativeOperatorType multOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};

struct RelationalExpressionNode : ExpressionNode
{
  RelationalExpressionNode (RelationalOperatorType relop,
			    ExpressionNode* lhs,
			    ExpressionNode* rhs, int pRow, int pCol)
  : ExpressionNode(ValueType::INT, pRow, pCol), relationalOperator (relop), left (lhs), right (rhs)
  { }

  virtual ~RelationalExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  RelationalOperatorType relationalOperator;
  ExpressionNode* left;
  ExpressionNode* right;
};


struct UnaryExpressionNode : ExpressionNode
{
  UnaryExpressionNode (UnaryOperatorType unaryOp,
		       VariableExpressionNode* var);

  ~UnaryExpressionNode ();

  void
  accept (IVisitor* visitor);

  UnaryOperatorType unaryOperator;
  VariableExpressionNode* variable;
};

struct IntegerLiteralExpressionNode : ExpressionNode
{
  IntegerLiteralExpressionNode (int pValue, int pRow, int pCol)
    : ExpressionNode(ValueType::INT, pRow, pCol), value (pValue)
  { }

  virtual ~IntegerLiteralExpressionNode ()
  { }

  virtual void
  accept (IVisitor* visitor)
  {
    visitor->visit (this);
  }

  int value;
};

struct EvaluationVisitor : IVisitor
{
  // Returns the num tabs
  virtual std::string
  indentation (int num)
  {
    string returnStr = "";
    for (int x = 0; x < num; ++x)
    {
      returnStr += "\t";
    }
    return returnStr;
  }

  virtual void
  visit (ProgramNode* node)
  {
    output = "ProgramNode:\n";
  }

  virtual void
  visit (DeclarationNode* node)
  {
    output = indentation (num) + "Dec\n";
  }

  virtual void
  visit (VariableDeclarationNode* node)
  {
    output = (indentation (num) + "VariableDeclaration: " + node->identifier + ": " + valueTypeMap[node->valueType] + " type\n");
  }

  virtual void
  visit (FunctionDeclarationNode* node)
  {
    string localOutput = (indentation (num) + "Function: " + node->identifier + ": " + valueTypeMap[node->valueType] + " type\n");
    ++num;

    // Returns the parameters
    for (ParameterNode* parameter : node->parameters)
    {
      parameter->accept (this);
      localOutput += output;
    }

    node->functionBody->accept (this);
    localOutput += output;
    --num;
    output = localOutput;
  }

  virtual void
  visit (ArrayDeclarationNode* node)
  {
    output = (indentation (num) + "VariableDeclaration: " + node->identifier + "["
            + std::to_string (node->size) + "]: " + valueTypeMap[node->valueType] + " type\n");
  }

  virtual void
  visit (ParameterNode* node)
  {
    if (node->isArray)
      output = (indentation (num) + "Parameter: " + node->identifier + "[]: " + valueTypeMap[node->valueType] + " type\n");
    else 
      output = (indentation (num) + "Parameter: " + node->identifier + ": " + valueTypeMap[node->valueType] + " type\n");
  }

  virtual void
  visit (StatementNode* node)
  {
    output = "Statement\n";
  }

  virtual void
  visit (CompoundStatementNode* node)
  {
    string localOutput = indentation(num) + "CompoundStatement:\n";
    ++num;

    // Adds the variable declarations
    for (VariableDeclarationNode* varDec : node->localDeclarations)
    {
      varDec->accept (this);
      localOutput += output;
    }

    // Adds the statements
    for (StatementNode* statement : node->statements)
    {
      statement->accept (this);
      localOutput += output;
    }
    --num;
    output = localOutput;
  }

  virtual void
  visit (IfStatementNode* node)
  {
    string localOutput = indentation (num) + "If\n";
    ++num;

    node->conditionalExpression->accept (this);
    localOutput += output;

    node->thenStatement->accept (this);
    localOutput += output;
    
    // Adds the else statement if used
    if (node->elseStatement != nullptr)
    {
      node->elseStatement->accept (this);
      localOutput += output;
    }
    
    --num;
    output = localOutput;
  }

  virtual void
  visit (WhileStatementNode* node)
  {
    string localOutput = indentation (num) + "While\n";
    ++num;
    node->conditionalExpression->accept (this);
    localOutput += output;

    node->body->accept (this);
    localOutput += output;
    --num;

    output = localOutput;
  }

  virtual void
  visit (ForStatementNode* node)
  {
  }

  virtual void
  visit (ReturnStatementNode* node)
  {
    string localOutput = indentation (num) + "Return\n";
    ++num;
    if (node->expression != nullptr)
    {
      node->expression->accept (this);
      localOutput += output;
    }
    --num;
    output = localOutput;
  }

  virtual void
  visit (ExpressionStatementNode* node)
  {
    string localOutput = indentation (num) + "ExpressionStatement:\n";
    ++num;
    if (node->expression != nullptr)
    {
      node->expression->accept (this);
      localOutput += output;
    }
    --num;
    output = localOutput;
  }

  virtual void
  visit (ExpressionNode* node)
  {
    output = "Expression\n";
  }

  virtual void
  visit (AssignmentExpressionNode* node)
  {
    string localOutput = indentation (num) + "Assignment: \n";
    ++num;

    // Adds the left side of assignment
    if (node->variable != nullptr)
    {
      node->variable->accept (this);
      localOutput += output;
    }
    
    // Adds right side of assignment
    if (node->expression != nullptr)
    {
      node->expression->accept (this);
      localOutput += output;
    }
    --num;
    output = localOutput;
  }

  virtual void
  visit (VariableExpressionNode* node)
  {
    output = indentation (num) + "Variable: " + node->identifier + ": " + valueTypeMap[node->usingDecNode->valueType] + " type" + "\n";
  }

  virtual void
  visit (SubscriptExpressionNode* node)
  {
    string localOutput = indentation (num) + "Subscript: " + node->identifier + ": " + valueTypeMap[node->usingDecNode->valueType] + " type" + "\n";
    ++num;
    localOutput += indentation (num) + "Index:\n";
    ++num;

    if (node->index != nullptr)
    {
      node->index->accept (this);
      localOutput += output;
    }
    --num;
    --num;
    output = localOutput;
  }

  virtual void
  visit (CallExpressionNode* node)
  {
    string localOutput = indentation (num) + "FunctionCall: " + node->identifier + ": " + valueTypeMap[node->usingDecNode->valueType] + " type" + "\n";
    
    if (!node->arguments.empty ())
    {
      ++num;
      localOutput += indentation (num) + "Arguments:\n";
      ++num;
      for (ExpressionNode* arg : node->arguments)
      {
        arg->accept (this);
        localOutput += output;
      }
      --num;
      --num;
    }
    output = localOutput;
  }

  virtual void
  visit (AdditiveExpressionNode* node)
  {
    string localOutput = indentation (num) + "AdditiveExpression: " + addTypeMap[node->addOperator] + "\n";
    ++num;
    localOutput += indentation (num) + "Left:\n";
    ++num;
    if (node->left != nullptr)
    {
      node->left->accept (this);
      localOutput += output;
    }
    --num;
    localOutput += indentation (num) + "Right:\n";
    ++num;
    if (node->right != nullptr)
    {
      node->right->accept (this);
      localOutput += output;
    }
    --num;
    --num;
    output = localOutput;
  }

  virtual void
  visit (MultiplicativeExpressionNode* node)
  {
    string localOutput = indentation (num) + "MultiplicativeExpression: " + multTypeMap[node->multOperator] + "\n";
    ++num;
    localOutput += indentation (num) + "Left:\n";
    ++num;
    if (node->left != nullptr)
    {
      node->left->accept (this);
      localOutput += output;
    }
    --num;
    localOutput += indentation (num) + "Right:\n";
    ++num;
    if (node->right != nullptr)
    {
      node->right->accept (this);
      localOutput += output;
    }
    --num;
    --num;
    output = localOutput;
  }

  virtual void
  visit (RelationalExpressionNode* node)
  {
    string localOutput = indentation (num) + "RelationalExpression: " + relTypeMap[node->relationalOperator] + "\n";
    ++num;
    localOutput += indentation (num) + "Left:\n";
    ++num;
    if (node->left != nullptr)
    {
      node->left->accept(this);
      localOutput += output;
    }
    --num;
    localOutput += indentation (num) + "Right:\n";
    ++num;
    if (node->right != nullptr)
    {
      node->right->accept (this);
      localOutput += output;
    }
    --num;
    --num;
    output = localOutput;
  }

  virtual void
  visit (UnaryExpressionNode* node)
  {
  }

  virtual void
  visit (IntegerLiteralExpressionNode* node)
  {
    output = indentation (num) + "Integer: " + std::to_string (node->value) + "\n";
  }

  std::map<ValueType, std::string> valueTypeMap { {ValueType::INT, "Int"}, 
      {ValueType::VOID, "Void"}, {ValueType::ARRAY, "ARRAY"} };
  std::map<AdditiveOperatorType, std::string> addTypeMap { {AdditiveOperatorType::PLUS, "+"},
      {AdditiveOperatorType::MINUS, "-"} };
  std::map<MultiplicativeOperatorType, std::string> multTypeMap { {MultiplicativeOperatorType::TIMES, "*"},
      {MultiplicativeOperatorType::DIVIDE, "/"} };
  std::map<RelationalOperatorType, std::string> relTypeMap { {RelationalOperatorType::LT , "<"},
      {RelationalOperatorType::LTE , "<="}, {RelationalOperatorType::GT , ">"}, {RelationalOperatorType::GTE , ">="},
      {RelationalOperatorType::EQ , "=="}, {RelationalOperatorType::NEQ , "!="} };

  string output = "";
  int num = 1;
};

#endif
