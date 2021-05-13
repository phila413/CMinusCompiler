/*
  Filename   : Lexer.h
  Author     : Philip Androwick
  Course     : CSCI 435
  Assignment : Assignment 9 - Lexing With Class
*/

/***********************************************************************/

#ifndef LEXER_H
#define LEXER_H

/***********************************************************************/

#include <string>
#include <stdio.h>
#include <map>

/***********************************************************************/

enum TokenType
  {
   // Special tokens
   END_OF_FILE, ERROR,

   // Keywords
   IF, ELSE, INT, VOID, RETURN, WHILE,

   // Operators
   PLUS, MINUS, TIMES, DIVIDE, LT, LTE, GT, GTE, EQ, NEQ, ASSIGN,

   // Punctuators
   SEMI, COMMA, LPAREN, RPAREN, LBRACK, RBRACK, LBRACE, RBRACE,

   // Identifier and integer literals
   ID, NUM
  };

/***********************************************************************/

struct Token
{
  Token (TokenType pType = END_OF_FILE,
         int pLine = 0,
         int pColumn = 0,
         std::string pLexeme = "",
         int pNumber = 0)
    : type (pType), lineNum (pLine), columnNum (pColumn), lexeme (pLexeme), number (pNumber)
  {  }

  TokenType   type;
  int         lineNum;
  int         columnNum;
  std::string lexeme;
  int         number;
};

/***********************************************************************/

class Lexer
{
public:
  Lexer (FILE* srcFile)
  {
    m_srcFile = srcFile;
    m_lineNum = 1;
    m_columnNum = 1;
  }

  Token
  getToken ();

  int
  getLineNum ()
  {
    return m_lineNum;
  }

  int
  getColumnNum ()
  {
    return m_columnNum;
  }

private:
  int
  getChar ()
  {
    int letter = fgetc (m_srcFile);
    ++m_columnNum;
    if (letter == '\n')
    {
      ++m_lineNum;
      m_columnNum = 1;
    }
    recentLetter = letter;
    return letter;
  }

  void
  ungetChar (int c)
  {
    ungetc (c, m_srcFile);
    --m_columnNum;
  }

  Token
  lexId ()
  {
    std::string id = "";
    id += recentLetter;
    char c;

    // Continue down file, until you hit a non-alpha char
    while (isalpha (c = getChar ()))
    {
      id += c;
    }
    ungetChar (c);

    std::map<std::string, TokenType> tokenMap { {"if", IF}, {"else", ELSE},
      {"int", INT}, {"void", VOID}, {"return", RETURN}, {"while", WHILE} };
    
    // Return akeyword token if found in map
    if (tokenMap[id] != 0)
      return Token(tokenMap[id], m_lineNum, m_columnNum, id);
    else
      return Token(ID, m_lineNum, m_columnNum, id);
  }

  Token
  lexNum ()
  {
    std::string id = "";
    id += recentLetter;
    char c;

    // Continue down file, until you hit a non-digit char
    while (isdigit (c = getChar ()))
    {
      id += c;
    }
    ungetChar (c);

    // Convert string id into a number
    int num = stoi (id);

    return Token (NUM, m_lineNum, m_columnNum, id, num);
  }
  
private:
  FILE* m_srcFile;
  char  recentLetter;
  int   m_lineNum;
  int   m_columnNum;
};

/***********************************************************************/

#endif
