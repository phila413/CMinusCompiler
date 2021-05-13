# CMinusCompiler
Made by: Philip Androwick

CMinusCompiler is a compiler for the c minus language.  The language is meant to be simple for people who are learning how to write compilers.  The C Minus functionality that this compiler can handle includes if-statements (and if-else-statements), while-loops, expressions (+, -, *, /, and all relational operators like < and >), functions, and parameters.  Types include void and int.  Variables can also be arrays.

# Stages
 1. Lexical Analysis - The CMinusCompiler first runs through the file and connects the lexemes to tokens.  If a lexeme is not a word that can be written in C Minus (such as #), then an error is thrown
 2. Parser - Next the compiler uses a Context-Free Grammar to make sure each token is in an appropriate order.  It also makes an abstract syntax tree (astTree) by making nodes out of the important tokens (for example semi-colons are not important, but variables are).
 3. Semantic Analyzer (Phase 1) - The compiler then creates a symbol table (a list of maps).  Using the IVisitor class in CMinusAst.h, the compiler runs through the astTree and finds the declarations/variables being used in each scope.  If the compiler found a declaration, then it makes sure the new declaration's identifier was not already used.  If the compiler found a variable being used, then it checks to make sure the variable's identifier was declared first.
 4. Semantic Analyzer (Phase 2) - With an astTree that now has its children expression node's connected to the declaration nodes, the semantic analyzer does one more pass through the tree to find possible semantic errors.  The errors it checks for includes:
   - Subscripting non-arrays
   - Operators being applied to non-integer types
   - Assigning to array or function names
   - Calling non-functions
   - Types of arguments and parameters not being compatible
   - Too few or too many function arguments
   - Returning a value from a void function
   - Not returning a value from a non-void function
   - Declaring variables or parameters as void
   - Not declaring "main" last
