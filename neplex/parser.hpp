#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include <iostream>
#include <sstream>
#include <set>
#include <lex.hpp>
#include <utils.hpp>
#include "ast.hpp"
#include "types.hpp"

class NwkParser{

protected:

  std::istream *input;

  lex::LexScanner *scanner;

  std::set<numbertype> nodenumbers;

public:

  NwkParser(std::istream *input) throw (Exception<NwkParser>, Exception<lex::LexScreener>, ExceptionBase);

  ~NwkParser();

  NwkAst * parse() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  NodeAst * parseNode() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  ConnectionAst * parseConnection() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  Coords parseCoords() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  long double parseReal() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  long parseInteger() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  long parseNatural() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  void skipChar(char c) throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  char lookAhead() throw (Exception<NwkParser>, Exception<lex::LexScanner>, Exception<lex::LexScreener>, ExceptionBase);

  void assertUniqueness(numbertype nodenumber) throw (Exception<NwkParser>);

  void checkMoreSemantic(NwkAst *nwk) throw (Exception<NwkParser>, ExceptionBase);

  void assertExistence(numbertype nodenumber) throw (Exception<NwkParser>);

};

#endif

