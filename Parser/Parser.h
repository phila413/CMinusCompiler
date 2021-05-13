#include <deque>
#include "../Lexer/Lexer.h"
#include "CMinusAst.h"

class Parser
{
	public :
		Parser (std::deque<Token> tokensPar)
		{
			tokens = tokensPar;
		}

		ProgramNode*
		program ();

		ProgramNode*
		decList ();

		DeclarationNode*
		dec ();

		DeclarationNode*
		nameState ();

		VariableDeclarationNode*
		varDec (DeclarationNode* varName);

		void
		typeSpec ();

		DeclarationNode*
		funDec (DeclarationNode* funcName);

		vector<ParameterNode*>
		params ();

		vector<ParameterNode*>
		paramList (ValueType type, std::string lexeme);

		ParameterNode*
		param ();

		CompoundStatementNode*
		compoundStmt ();

		vector<VariableDeclarationNode*>
		localDec();

		vector<StatementNode*>
		stateList();

		StatementNode*
		state();

		ExpressionStatementNode*
		expressionStmt ();

		IfStatementNode*
		selectionStmt ();

		WhileStatementNode*
		iterationStmt ();

		ReturnStatementNode*
		returnStmt ();

		ExpressionNode*
		expression ();

		VariableExpressionNode*
		var (std::string tempID, ValueType type, int tempRow, int tempCol);

		ExpressionNode*
		simpleExpr ();

		RelationalOperatorType
		relop ();

		ExpressionNode*
		additiveExpr ();

		AdditiveOperatorType
		addop ();

		ExpressionNode*
		term ();

		MultiplicativeOperatorType
		mulop ();

		ExpressionNode*
		factor ();

		CallExpressionNode*
		call (std::string tempID, ValueType type, int tempRow, int tempCol);

		vector<ExpressionNode*>
		args ();

		std::string
		getAST (ProgramNode* tree)
		{
			std::string finalOutput = "";
			
			EvaluationVisitor visitor;
			tree->accept (&visitor);
			finalOutput += visitor.output;
			finalOutput += "\n";

			for (DeclarationNode* node : tree->declarations)
			{
				EvaluationVisitor visitor;
				node->accept (&visitor);
				finalOutput += visitor.output;
				finalOutput += "\n";
			}
			
			return finalOutput;
		}

	public :
		std::map<std::string, int> tokenMap { {"END_OF_FILE", 0}, {"ERROR", 1},
			    {"IF", IF}, {"ELSE", ELSE}, {"INT", INT}, {"VOID", VOID}, {"RETURN", RETURN},
				{"WHILE", WHILE}, {"PLUS", PLUS}, {"MINUS", MINUS}, {"TIMES", TIMES},
				{"DIVIDE", DIVIDE}, {"LT", LT}, {"LTE", LTE}, {"GT", GT}, {"GTE", GTE},
				{"EQ", EQ}, {"NEQ", NEQ}, {"ASSIGN", ASSIGN}, {"SEMI", SEMI},
				{"COMMA", COMMA}, {"LPAREN", LPAREN}, {"RPAREN", RPAREN}, {"LBRACK", LBRACK},
				{"RBRACK", RBRACK}, {"LBRACE", LBRACE}, {"RBRACE", RBRACE}, {"ID", ID},
				{"NUM", NUM} };
		
		Token g_token;
		std::deque<Token> tokens;
	private :
		// Pops the front token in the deque and returns it
		Token
		getToken ()
		{
			Token val = tokens.front ();
			tokens.pop_front ();
			return val;
		}

		void
		match (const std:: string& function, std::initializer_list<std::string> const &expectedTokenTypes)
		{
			for (std::string expectedToken : expectedTokenTypes)
			{
				if (g_token.type == tokenMap[expectedToken])
				{
					g_token = getToken ();
					return;
				}
			}
			error (function, expectedTokenTypes);
		}

		// Prints out an error message
		void 
		error (const std::string& function, std::initializer_list<std::string> const &expectedTokenTypes)
		{
			printf ("\nError while parsing \'%s\'\n", function.c_str ());
			printf ("  Encountered: \'%s\' (line %d, column %d)\n", g_token.lexeme.c_str (), g_token.lineNum, g_token.columnNum);
			printf ("  Expected   : %s\n", expectedTokenTypes.begin ()->c_str ());
			if (expectedTokenTypes.size () > 1)
			{
				for (std::initializer_list<std::string>::iterator x = expectedTokenTypes.begin ()+1; x != expectedTokenTypes.end (); ++x)
				{
					printf ("            or %s\n", x->c_str ());
				}
			}
			printf("\n");
			exit (1);
		}		
};