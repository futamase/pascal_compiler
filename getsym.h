/*
  getsym() -- simple lexical analyzer
  getsym.h
  Copyright (C) 1996 Toyohashi University of Technology
*/
#ifndef GETSYM
#define GETSYM

#include <iostream>
#include <fstream>

#define MAXIDLEN	15

typedef struct {
    int	attr;
    int value;
    char charvalue[MAXIDLEN+1];
    int sline;
} TOKEN;

#define IDENTIFIER   1
#define NUMBER       2
#define RWORD        3
#define SYMBOL       4
#define ENDFILE      5
#define SYMERR       6

#define	BEGIN		260
#define	DIV		261
#define	DO		262
#define	ELSE		263
#define	END		264
#define	IF		265
#define	PROCEDURE	266
#define	PROGRAM		267
#define	THEN		268
#define	VAR		269
#define	WHILE		270

#define	READ		271
#define	WRITE		272

#define	PLUS		'+'
#define	MINUS		'-'
#define	TIMES		'*'
#define	LPAREN		'('
#define	RPAREN		')'
#define	EQL		'='
#define	COMMA		','
#define	PERIOD		'.'
#define	SEMICOLON	';'
#define	BECOMES		256	/* := */
#define	LESSTHAN	'<'
#define	LESSEQL		257	/* <= */
#define	NOTEQL		258	/* <> */
#define	GRTRTHAN	'>'
#define	GRTREQL		259	/* >= */
#define	COLON		':'

class GetSym
{
public:

  struct resword { char *name; int token; };

  GetSym(TOKEN& intok, std::ifstream& ifs)
    :tok(intok), ist(ifs){
  }
  void init_getsym();
  void getsym();

private:
  TOKEN& tok;
  // FILE *infile;
  std::istream& ist;

  int sline;
  int toklen;

  char symbols[12] = {
    '+', '-', '*', '(', ')', '=',
    ',', '.', ';', ':', '<', '>'
  };

  int symvalue[12] = {
    PLUS, MINUS, TIMES, LPAREN, RPAREN, EQL,
    COMMA, PERIOD, SEMICOLON, COLON, LESSTHAN, GRTRTHAN
  };

  int get_char();
  void unget_char(register int c);
  int skip_blanks();
  void flush_tokenbuf();
  void push_tokenbuf(int c);
  int hash(register char *str, register int len);
  resword * is_reserved_word(register char * str, register int len);
  void scan_identifier();
  void scan_number();
  void scan_symbol(int, int);

};

#endif
