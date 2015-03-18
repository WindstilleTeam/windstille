//  Copyright (C) 2004 Matthias Braun <matze@braunis.de>
//  code in this file based on lispreader from Mark Probst
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#ifndef __LISPLEXER_H__
#define __LISPLEXER_H__

namespace lisp
{

class Lexer
{
public:
  enum TokenType {
    TOKEN_EOF,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_SYMBOL,
    TOKEN_STRING,
    TOKEN_INTEGER,
    TOKEN_REAL,
    TOKEN_TRUE,
    TOKEN_FALSE
  };

  Lexer(std::istream& stream);
  ~Lexer();

  TokenType getNextToken();
  const char* getString() const
  { return token_string; }
  int getLineNumber() const
  { return linenumber; }

private:
  enum {
    MAX_TOKEN_LENGTH = 16384,
    BUFFER_SIZE = 1024
  };

  inline void nextChar();

  std::istream& stream;
  bool eof;
  int linenumber;
  char buffer[BUFFER_SIZE+1];
  char* bufend;
  char* c;
  char token_string[MAX_TOKEN_LENGTH + 1];
  int token_length;

private:
  Lexer(const Lexer&);
  Lexer& operator=(const Lexer&);
};

} // end of namespace lisp

#endif

