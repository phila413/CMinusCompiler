/*
	Filename   : Parser.cc
	Author     : Philip Androwick
	
	program   -> decList
	decList   -> dec {dec}
	dec       -> nameState (varDec | funDec)
	
	nameState -> typeSpec ID
	varDec    -> [ '[' NUM ']' ] ;
	typeSpec  -> int | void

	funDec    -> '(' params ')' compoundStmt
	params    -> void [ID paramList] | int ID paramList
	paramList -> [[]] {, param} 
	param     -> nameState [ '[' ']' ]

	compound-stmt -> '{' localDec stateList '}'
	localDec      -> { nameState varDec }
	stateList     -> { state }
	state         ->  compoundState | selectionState | iterationState | returnState | expressionState
	
	expressionStmt -> expression ; | ;
	selectionStmt  -> if ( expression ) statement [ else statement ]
	iterationStmt  -> while ( expression ) statement
	returnStmt     -> return [ expression ] ;

	expression        -> [ ID var = expression ] simpleExpr
	var               -> '[' expression ']'
	simple-expression -> additiveExpr [ relop additiveExpr ]
	relop             -> <= | < | > | >= | == | !=
	additiveExpr      -> term { addop term }
	addop             -> +|-
	term              -> factor { mulop factor }
	mulop             -> * | /
	factor            -> '(' expression ')' | ID ( var | call ) | NUM
	call              -> ( args )
	args              -> [ expression { , expression } ]
*/

#include "Parser.h"
#include "CMinusAst.h"
#include <iostream>
#include <vector>
#include <map>

/**************************************************************************************/
// program, decList, dec
// These three functions figure out if the program is ending, or has more code to parse.
// It also decides if the tokens type will be a variable declaration or a function declaration.

// program -> decList
ProgramNode*
Parser::program ()
{
	g_token = getToken (); 
	return decList ();
}

// decList -> dec {dec}
ProgramNode*
Parser::decList ()
{
	vector<DeclarationNode*> declarations;
	declarations.push_back (dec ());
    
    while (g_token.type != tokenMap["END_OF_FILE"])
		declarations.push_back (dec ());

	return new ProgramNode (declarations);
}

// dec -> nameState (varDec | funDec)
DeclarationNode*
Parser::dec ()
{
	DeclarationNode* node = nameState ();
		
	if (g_token.type == tokenMap["LBRACK"] || g_token.type == tokenMap["SEMI"])
		return varDec (node);
	else if (g_token.type == tokenMap["LPAREN"])
		return funDec (node);
	else
	{
		error ("dec", {"VAR"});
		return node;
	}
}

/**************************************************************************************/
// nameState, varDec, typeSpec
// varDec is for variable declarations.  These productions make sure the code starts with
// int or void and has an ID.  the variable can also be an array.

//nameState -> typeSpec ID
DeclarationNode*
Parser::nameState ()
{
	std::map<int, ValueType> valueTypeMap { {INT, ValueType::INT}, 
		{VOID, ValueType::VOID} };
	ValueType type = valueTypeMap[g_token.type];
	 
	typeSpec ();
	
	DeclarationNode* varNode = new DeclarationNode (type, g_token.lexeme, DataType::VARIABLE, g_token.lineNum, g_token.columnNum);
	match ("nameState", {"ID"}); 
	return varNode;
}

// varDec -> [ '[' NUM ']' ] ;
VariableDeclarationNode*
Parser::varDec (DeclarationNode* decName)
{
	int tempRow = g_token.lineNum;
	int tempCol = g_token.columnNum;
	// Optional array declaration
	if (g_token.type == tokenMap["LBRACK"])
	{
		match ("varDec", {"LBRACK"});
		int x = std::stoi (g_token.lexeme);
		ArrayDeclarationNode* arrayName = new ArrayDeclarationNode (decName->valueType, decName->identifier, x, tempRow, tempCol);
		delete decName;
		match ("varDec", {"NUM"});
		match ("varDec", {"RBRACK"});
		match ("varDec", {"SEMI"});
		return arrayName;
	}
	
	// Necessary semicolon
	match ("varDec", {"SEMI"});
	return new VariableDeclarationNode (decName->valueType, decName->identifier, DataType::VARIABLE, tempRow, tempCol);
}

// typeSpec -> int | void
void
Parser::typeSpec ()
{
	match ("typeSpec", {"INT", "VOID"});
}

/**************************************************************************************/
// funDec, params, paramList, param
// funDec is for functions.  The params productions checks that the parameters code
// is valid.  The parameters can be just "void", or have variables/arrays that are
// of type void or int.  Multiple parameters are separated with commas.
	
// funDec -> '(' params ')' compoundStmt
DeclarationNode*
Parser::funDec (DeclarationNode* funcName)
{
	int tempRow = g_token.lineNum;
	int tempCol = g_token.columnNum;
	match ("funDec", {"LPAREN"});
	vector<ParameterNode*> parameters = params ();
	match ("funDec", {"RPAREN"});
	CompoundStatementNode* body = compoundStmt ();
	return new FunctionDeclarationNode (funcName->valueType, funcName->identifier, parameters, body, tempRow, tempCol);
}

// params -> void [ID paramList] | int ID paramList
vector<ParameterNode*>
Parser::params ()
{
	vector<ParameterNode*> parameters;
	// If the first parameter starts with void, the parameter list
	// can end with just void it can be a variable
	if (g_token.type == tokenMap["VOID"])
	{
		match ("params", {"VOID"});
		
		if (g_token.type == tokenMap["ID"])
		{
			std::string tempLex = g_token.lexeme;
			match ("params", {"ID"});
			return paramList (ValueType::VOID, tempLex);
		}
		return parameters;
	}
	// If the first parameter start with int, the parameter
	// MUST be a variable
	else if (g_token.type == tokenMap["INT"])
	{		
		match ("params", {"INT"});
		std::string tempLex = g_token.lexeme;
		match ("params", {"ID"});
		return paramList (ValueType::INT, tempLex);
	}
	else
	{
		error ("params", {"INT", "VOID"});
		return parameters;
	}
}

// paramList -> [[]] {, param} 
vector<ParameterNode*>
Parser::paramList (ValueType type, std::string lexeme)
{
	vector<ParameterNode*> parameters;
	bool isArray = false;
	int tempRow = g_token.lineNum;
	int tempCol = g_token.columnNum;
	// Optional array for first parameter
	if (g_token.type == tokenMap["LBRACK"])
	{
		match ("paramList", {"LBRACK"});
		match ("paramList", {"RBRACK"});
		isArray = true;
	}

	ParameterNode* parameter = new ParameterNode (type, lexeme, isArray, tempRow, tempCol);
	parameters.push_back (parameter);

	// Multiple parameters
	while (g_token.type == tokenMap["COMMA"])
	{
		match ("paramList", {"COMMA"});
		ParameterNode* newParameter = param ();
		parameters.push_back (newParameter);
	}

	return parameters;
}

// param -> nameState [ '[' ']' ] 
ParameterNode*
Parser::param ()
{
	DeclarationNode* node = nameState ();
	int tempRow = g_token.lineNum;
	int tempCol = g_token.columnNum;
	// Optional array for non-first parameters
	bool isArray = false;
	if (g_token.type == tokenMap["LBRACK"])
	{
		match ("param", {"LBRACK"});
		match ("param", {"RBRACK"});
		isArray = true;
	}

	return new ParameterNode (node->valueType, node->identifier, isArray, tempRow, tempCol);
}                     

/**************************************************************************************/
// compoundStmt, localDec, stateList, state
// compoundStmt catches the multiple states that can happen in functions.  states include
// expressions, if/else statements, while loops, a return statement, and even another 
// compoundStmt.  compoundStmts must start with variable declarations, followed by states.

// compound-stmt -> '{' localDec stateList '}'
CompoundStatementNode*
Parser::compoundStmt ()
{
	match ("compoundState", {"LBRACE"});
	vector<VariableDeclarationNode*> localDeclarations = localDec ();
	vector<StatementNode*> statements = stateList ();
	match ("compoundState", {"RBRACE"});
	return new CompoundStatementNode (localDeclarations, statements);
}

// localDec -> { nameState varDec }
vector<VariableDeclarationNode*>
Parser::localDec ()
{
	vector<VariableDeclarationNode*> varVec;
	while (g_token.type == tokenMap["INT"] || g_token.type == tokenMap["VOID"])
	{
		DeclarationNode* node = nameState ();
		VariableDeclarationNode* varNode = varDec (node);
		varVec.push_back (varNode);
	}
	return varVec;
}

// stateList -> { state } 
vector<StatementNode*>
Parser::stateList ()
{
	vector<StatementNode*> stateNodeVec;
	while (g_token.type != tokenMap["RBRACE"])
	{
		StatementNode* stateNode = state ();
		stateNodeVec.push_back (stateNode);
	}
	return stateNodeVec;
}

// state -> compoundState | selectionState | iterationState | returnState | expressionState 
StatementNode*
Parser::state()
{
	if (g_token.type == tokenMap["LBRACE"])
		return compoundStmt ();
	else if (g_token.type == tokenMap["IF"])
		return selectionStmt ();
	else if (g_token.type == tokenMap["WHILE"])
		return iterationStmt ();
	else if (g_token.type == tokenMap["RETURN"])
		return returnStmt ();
	else
		return expressionStmt ();
}

/**************************************************************************************/
// expressionStmt, selectionStmt, iterationStmt, returnStmt
// These for statements are different types of states.

// expressionStmt -> expression ; | ;
ExpressionStatementNode*
Parser::expressionStmt ()
{
	ExpressionStatementNode* exprNode = new ExpressionStatementNode (expression ());
	match ("expressionStmt", {"SEMI"});
	return exprNode;
}

// selectionStmt -> if ( expression ) statement [ else statement ]
IfStatementNode*
Parser::selectionStmt ()
{
	match ("selectionStmt", {"IF"});
	match ("selectionStmt", {"LPAREN"});
	ExpressionNode* exprNode = expression ();
	match ("selectionStmt", {"RPAREN"});
	StatementNode* thenNode = state ();
	
	// Optional else statement
	StatementNode* elseNode = nullptr;
	if (g_token.type == tokenMap["ELSE"])
	{
		match ("selectionStmt", {"ELSE"});
		elseNode = state ();
	}
	
	return new IfStatementNode (exprNode, thenNode, elseNode);
}

// iterationStmt -> while ( expression ) statement
WhileStatementNode*
Parser::iterationStmt ()
{
	match ("iterationStmt", {"WHILE"});
	match ("iterationStmt", {"LPAREN"});
	ExpressionNode* exprNode = expression ();
	match ("iterationStmt", {"RPAREN"});
	StatementNode* statement = state ();
  	return new WhileStatementNode (exprNode, statement);
}

// returnStmt -> return [ expression ] ;
ReturnStatementNode*
Parser::returnStmt ()
{
	match ("returnStmt", {"RETURN"});
	ReturnStatementNode* returnNode = new ReturnStatementNode (expression ());
	match ("returnStmt", {"SEMI"});

	return returnNode;
}

/**************************************************************************************/
// expression, var, simpleExpr, additiveExpr, addop, term, mulop, factor, call, args
// This huge section is for expressions.  These include math (+, -, /, and *), comparisons
// (<, <=, >, >=, ==, and !=), and even other function calls (with parameters) that can return
// values for the expression. 

// expression -> [ ID var = expression ] simpleExpr
ExpressionNode*
Parser::expression ()
{	
	std::deque<Token> tempTokens = tokens;
	tempTokens.push_front (g_token);

	if (g_token.type == tokenMap["ID"])
	{
		std::string tempID = g_token.lexeme;
		int tempRow = g_token.lineNum;
		int tempCol = g_token.columnNum;
		std::map<int, ValueType> valueTypeMap { {INT, ValueType::INT}, 
			{VOID, ValueType::VOID} };
		ValueType type = valueTypeMap[g_token.type];

		match ("expression", {"ID"});
		VariableExpressionNode* varExpNode = var (tempID, type, tempRow, tempCol);

		if (g_token.type == tokenMap["ASSIGN"])
		{
			tempRow = g_token.lineNum;
			tempCol = g_token.columnNum;
			match ("expression", {"ASSIGN"});
			return new AssignmentExpressionNode (type, varExpNode, expression (), tempRow, tempCol);
		}
		else
		{
			tokens = tempTokens;
			g_token = getToken ();
		}
	}

	ExpressionNode* node = simpleExpr ();
	return node;
}

// var -> '[' expression ']'
VariableExpressionNode*
Parser::var (std::string tempID, ValueType type, int tempRow, int tempCol)
{
	if (g_token.type == tokenMap["LBRACK"])
	{
		match ("var", {"LBRACK"});
		ExpressionNode* exNode = expression ();
		match ("var", {"RBRACK"});
		return new SubscriptExpressionNode (tempID, exNode, type, tempRow, tempCol);
	}
	return new VariableExpressionNode (tempID, type, DataType::VARIABLE, tempRow, tempCol);
}

// simple-expression -> additiveExpr [ relop additiveExpr ]
ExpressionNode*
Parser::simpleExpr ()
{
	ExpressionNode* left = additiveExpr ();
	if (left != nullptr)
	{
		RelationalOperatorType type;
		if (g_token.type == tokenMap["LTE"] || g_token.type == tokenMap["LT"] ||
			g_token.type == tokenMap["GT"] || g_token.type == tokenMap["GTE"] ||
			g_token.type == tokenMap["EQ"] || g_token.type == tokenMap["NEQ"])
		{
			int tempRow = g_token.lineNum;
			int tempCol = g_token.columnNum;
			type = relop ();
			ExpressionNode* right = additiveExpr ();
			left = new RelationalExpressionNode (type, left, right, tempRow, tempCol);
		}
	}
	return left;
}

// relop -> <= | < | > | >= | == | !=
RelationalOperatorType
Parser::relop ()
{
	TokenType tempTok = g_token.type;
	match ("relop", {"LTE", "LT", "GT", "GTE", "EQ", "NEQ"});
	if (tempTok == tokenMap["LTE"])
		return RelationalOperatorType::LTE;
	else if (tempTok == tokenMap["LT"])
		return RelationalOperatorType::LT;
	else if (tempTok == tokenMap["GT"])
		return RelationalOperatorType::GT;
	else if (tempTok == tokenMap["GTE"])
		return RelationalOperatorType::GTE;
	else if (tempTok == tokenMap["EQ"])
		return RelationalOperatorType::EQ;
	else
		return RelationalOperatorType::NEQ;	
}

// additiveExpr -> term { addop term }
ExpressionNode*
Parser::additiveExpr ()
{
	ExpressionNode* left = term ();

	AdditiveOperatorType type;
	while (g_token.type == tokenMap["PLUS"] || g_token.type == tokenMap["MINUS"])
	{
		int tempRow = g_token.lineNum;
		int tempCol = g_token.columnNum;
		type = addop ();
		ExpressionNode* right = term ();
		left = new AdditiveExpressionNode (type, left, right, tempRow, tempCol);
	}
	
	return left;
}

// addop -> +|-
AdditiveOperatorType
Parser::addop ()
{
	TokenType tempTok = g_token.type;
	match ("addop", {"PLUS", "MINUS"});
	if (tempTok == tokenMap["PLUS"])
		return AdditiveOperatorType::PLUS;
	else
		return AdditiveOperatorType::MINUS;
}

// term -> factor { mulop factor }
ExpressionNode*
Parser::term ()
{
	ExpressionNode* left = factor ();

	MultiplicativeOperatorType type;
	while (g_token.type == tokenMap["TIMES"] || g_token.type == tokenMap["DIVIDE"])
	{
		int tempRow = g_token.lineNum;
		int tempCol = g_token.columnNum;
		type = mulop ();
		ExpressionNode* right = factor ();
		left = new MultiplicativeExpressionNode (type, left, right, tempRow, tempCol);
	}
	
	return left;
}

// mulop -> * | /
MultiplicativeOperatorType
Parser::mulop ()
{
	TokenType tempTok = g_token.type;
	match ("mulop", {"TIMES", "DIVIDE"});
	if (tempTok == tokenMap["TIMES"])
		return MultiplicativeOperatorType::TIMES;
	else
		return MultiplicativeOperatorType::DIVIDE;
}

// factor -> '(' expression ')' | ID ( var | call ) | NUM
ExpressionNode*
Parser::factor ()
{
	if (g_token.type == tokenMap["LPAREN"])
	{
		match ("factor", {"LPAREN"});
		ExpressionNode* node = expression ();
		match ("factor", {"RPAREN"});
		return node;
	}
	else if (g_token.type == tokenMap["ID"])
	{
		std::string tempID = g_token.lexeme;
		int tempRow = g_token.lineNum;
		int tempCol = g_token.columnNum;
		std::map<int, ValueType> valueTypeMap { {INT, ValueType::INT}, 
		{VOID, ValueType::VOID} };
		ValueType type = valueTypeMap[g_token.type];
		match ("factor", {"ID"});
		if (g_token.type == tokenMap["LPAREN"])
			return call (tempID, type, tempRow, tempCol);
		else
			return var (tempID, type, tempRow, tempCol);
	}
	else if (g_token.type == tokenMap["NUM"])
	{
		int x = std::stoi (g_token.lexeme);
		int tempRow = g_token.lineNum;
		int tempCol = g_token.columnNum;
		match ("factor", {"NUM"});
		IntegerLiteralExpressionNode* node = new IntegerLiteralExpressionNode (x, tempRow, tempCol);
		return node;
	}
	else
	{
		return nullptr;
	}
}

// call -> ( args )
CallExpressionNode*
Parser::call (std::string tempID, ValueType type, int tempRow, int tempCol)
{
	match ("factor", {"LPAREN"});
	CallExpressionNode* callNode = new CallExpressionNode (tempID, args (), type, tempRow, tempCol);
	match ("factor", {"RPAREN"});
	return callNode;
}

// args -> [ expression { , expression } ]
vector<ExpressionNode*>
Parser::args ()
{
	vector<ExpressionNode*> argList;

	ExpressionNode* argument = expression ();
	if (argument == nullptr)
		return argList;
	else
		argList.push_back (argument);

	while (g_token.type == tokenMap["COMMA"])
	{
		match ("argsList", {"COMMA"});

		ExpressionNode* argument = expression ();
		if (argument == nullptr)
			return argList;
		else
			argList.push_back (argument);
	}

	return argList;
}
