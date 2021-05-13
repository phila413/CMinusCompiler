/*
  Filename   : Lexer.cc
  Author     : Philip Androwick
  Course     : CSCI 435
  Assignment : Lab 9 - Lexing With Class
*/

/***********************************************************************/
// System includes

#include <cstdlib>
#include <iostream>
#include <string>

/***********************************************************************/
// Local includes

#include "Lexer.h"

/***********************************************************************/
// Using declarations

using std::cout;
using std::endl;
using std::string;

/***********************************************************************/

Token
Lexer::getToken ()
{
  while (true)
  {
    char c = getChar ();
    
    if (isalpha (c))
      return lexId ();

    if (isdigit (c))
      return lexNum ();

    switch (c)
    {

    // Whitespace
    case ' ': case '\t': case '\r': case '\f': case '\n':
      break;

    case EOF:
      return Token (END_OF_FILE, m_lineNum, m_columnNum);

    // Operators
    case '+':
      return Token (PLUS, m_lineNum, m_columnNum, "+");
    
    case '-':
      return Token (MINUS, m_lineNum, m_columnNum, "-");

    case '*':
      return Token (TIMES, m_lineNum, m_columnNum, "*");

    case '/':
      c = getChar ();

      // One-line comment
      if (c == '/')
      {
        // Continue down file until end of line is found
        while (c != '\n') 
        {
          c = getChar ();
        }
        break;
      }
      // Multi-line comment
      else if (c == '*')
      {
        while (true)
        {  
          // If a * is found, and then a / is found after
          // it, then break the loop

          if ((c = getChar()) == '*')
          {
            if ((c = getChar ()) != '/') 
              ungetChar (c);
            else
              break;
          }
        }
        break;
      }
      ungetChar (c);
      return Token (DIVIDE, m_lineNum, m_columnNum, "/");

    case '<':
      c = getChar ();
      if (c != '=')
      {
        ungetChar (c);
        return Token (LT, m_lineNum, m_columnNum, "<");
      }
      return Token (LTE, m_lineNum, m_columnNum, "<=");

    case '>':
      c = getChar ();
      if (c != '=')
      {
        ungetChar (c);
        return Token (GT, m_lineNum, m_columnNum, ">");
      }
      return Token (GTE, m_lineNum, m_columnNum, ">=");

    case '!':
      c = getChar ();
      if (c != '=')
      {
        ungetChar (c);
        return Token (ERROR, m_lineNum, m_columnNum, "!");
      }
      return Token (NEQ, m_lineNum, m_columnNum, "!=");

    case '=':
      c = getChar ();
      if (c != '=')
      {
        ungetChar (c);
        return Token (ASSIGN, m_lineNum, m_columnNum, "=");
      }
      return Token (EQ, m_lineNum, m_columnNum, "==");

    // Punctuators
    case ';':
      return Token (SEMI, m_lineNum, m_columnNum, ";");

    case ',':
      return Token (COMMA, m_lineNum, m_columnNum, ",");

    case '(':
      return Token (LPAREN, m_lineNum, m_columnNum, "(");

    case ')':
      return Token (RPAREN, m_lineNum, m_columnNum, ")");

    case '[':
      return Token (LBRACK, m_lineNum, m_columnNum, "[");

    case ']':
      return Token (RBRACK, m_lineNum, m_columnNum, "]");

    case '{':
      return Token (LBRACE, m_lineNum, m_columnNum, "{");

    case '}':
      return Token (RBRACE, m_lineNum, m_columnNum, "}");

    case '$':
      return Token (END_OF_FILE, m_lineNum, m_columnNum);

    default:
      std::string s (1, c);
      return Token (ERROR, m_lineNum, m_columnNum, s);
    }
  }
}
