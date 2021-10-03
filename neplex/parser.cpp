#include "parser.hpp"

using namespace std;
using namespace lex;
using namespace utils;

NwkParser::NwkParser(istream *input) throw (Exception<NwkParser>, Exception<LexScreener>, ExceptionBase) : input(input){
  
  string thisMethod = "NwkParser()";

  if ( input == 0 )
    throw Exception<NwkParser>(thisMethod + ": input is 0!");

  scanner = new LexScanner(input);

  scanner->parseNumbers();
  scanner->reportEOL(true);
  scanner->useSignedNumbers();
  scanner->useFloatingpoints();

  LexCharClasses & charclasses = scanner->getLexCharClasses();

  // line-comment
  charclasses.setLineComment("#");

}

NwkParser::~NwkParser(){

  delete scanner;

}

NwkAst * NwkParser::parse() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  string thisMethod = "parse()";

  numbertype node_nmb = 0;
  NwkAst * nwk = new NwkAst();
  
  try{

    // parse number of nodes
    nwk->setNumberOfNodes((numbertype)parseNatural());

    if ( scanner->nextToken() != LexToken::TT_EOL ){
      
      if ( scanner->token.type == LexToken::TT_EOF )
	goto finished;
      else
	throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": invalid syntax!");

    }
    
    scanner->nextToken();

    // parse nodes and edges
    while ( scanner->token.type != LexToken::TT_EOF ){
      
      scanner->putback();

      if ( node_nmb == nwk->getNumberOfNodes() )
	throw Exception<NwkParser>(": more nodes than declared!");

      nwk->addNode(parseNode());
      
      node_nmb++;
      
      if ( scanner->nextToken() != LexToken::TT_EOL ){
	
	if ( scanner->token.type == LexToken::TT_EOF )
	  break;
	else
	  throw Exception<NwkParser>(String(": line ") + scanner->getLineNumber() + ": invalid syntax!");
	
      }

      scanner->nextToken();
      
    }
    
  finished:
    if ( node_nmb != nwk->getNumberOfNodes() )
      throw Exception<NwkParser>(": too few nodes defined!");

    checkMoreSemantic(nwk);
   
    return nwk;
    
  } catch (ExceptionBase &e){
    
    delete nwk;
    throw e.prependMsg(thisMethod + "->");
    
  }
  
}

NodeAst * NwkParser::parseNode() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  string thisMethod = "parseNode()";

  NodeAst *node = new NodeAst();
  char c;

  try{

    node->setNumber(parseNatural());

    // a bit of semantic check: nodenumbers must only occur once
    assertUniqueness(node->getNumber());

    node->setCoords(parseCoords());

    if ( lookAhead() == ',' ){

      skipChar(',');
      node->setInflow(parseReal());

    } else
      node->setInflow(0);

    skipChar(':');

    while ( scanner->nextToken() != LexToken::TT_EOL and scanner->token.type != LexToken::TT_EOF ){

      scanner->putback();

      node->addConnection(parseConnection());

      c = lookAhead();

      if ( c == ';' ){

	skipChar(';');

      } else {
	
	if ( c != LexToken::TT_EOL and c != LexToken::TT_EOF )
	  throw Exception<NwkParser>(String(": line ") + scanner->getLineNumber() + ": too many entries for edge!");

      }	

    }

    scanner->putback();

    return node;

  } catch (ExceptionBase &e){

    delete node;
    throw e.prependMsg(thisMethod + "->");

  }

}

ConnectionAst * NwkParser::parseConnection() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  string thisMethod = "parseConnection()";

  ConnectionAst * connection = new ConnectionAst();

  try{

    connection->setTargetnode(parseNatural());

    skipChar(',');

    connection->setCost(parseInteger());

    if ( lookAhead() == ',' ){
      
      skipChar(',');
      connection->setCapacity(parseInteger());

    } else
      connection->setCapacity(INFINIT_CAPACITY);

    return connection;

  } catch (ExceptionBase &e){

    delete connection;
    throw e.prependMsg(thisMethod + "->");

  }

}

Coords NwkParser::parseCoords() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  string thisMethod = "parseCoords()";
  
  long x,y;
  
  try{
    
    skipChar('(');
    
    x = parseInteger();
    
    skipChar(',');
    
    y = parseInteger();
    
    skipChar(')');
    
    return Coords(x,y);
    
  } catch (ExceptionBase &e){
    
    throw e.prependMsg(thisMethod + "->");
    
  }
  
}

long double NwkParser::parseReal() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  if ( scanner->nextToken() != LexToken::TT_NUMBER )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected number!");

  return scanner->token.nval;

}

long NwkParser::parseInteger() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  long double value;
  
  if ( scanner->nextToken() != LexToken::TT_NUMBER )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected number, got " + (char)scanner->token.type);
  
  value = scanner->token.nval;
  
  if ( value != (long)value )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected integer!");
  
  return (long)value;

}

long NwkParser::parseNatural() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  long double value;
  
  if ( scanner->nextToken() != LexToken::TT_NUMBER )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected number, got " + (char)scanner->token.type);
  
  value = scanner->token.nval;
  
  if ( value != (long)value )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected natural!");
  
  if ( value < 0 )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected natural!");
  
  return (long)value;

}

void NwkParser::skipChar(char c) throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  if ( scanner->nextToken() != c )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": expected '" + c + "'!");

}

char NwkParser::lookAhead() throw (Exception<NwkParser>, Exception<LexScanner>, Exception<LexScreener>, ExceptionBase){

  char c = scanner->nextToken();

  scanner->putback();

  return c;

}

void NwkParser::assertUniqueness(numbertype nodenumber) throw (Exception<NwkParser>){

  string thisMethod = "assertUniqueness()";

  if ( nodenumbers.find(nodenumber) != nodenumbers.end() )
    throw Exception<NwkParser>(String("line ") + scanner->getLineNumber() + ": nodenumber " + nodenumber + " not unique!");

  nodenumbers.insert(nodenumber);

}

void NwkParser::assertExistence(numbertype nodenumber) throw (Exception<NwkParser>){

  string thisMethod = "assertUniqueness()";

  if ( nodenumbers.find(nodenumber) == nodenumbers.end() )
    throw Exception<NwkParser>(String("nodenumber ") + nodenumber + " does not exist!");

}

void NwkParser::checkMoreSemantic(NwkAst *nwk) throw (Exception<NwkParser>, ExceptionBase){
  
  // assert that all referenced nodes are existing

  for ( list<NodeAst *>::const_iterator nit = nwk->getNodes().begin(); nit != nwk->getNodes().end(); nit++ ){
    for ( list<ConnectionAst *>::const_iterator cit = (*nit)->getConnections().begin(); cit != (*nit)->getConnections().end(); cit++ ){

      try{
	
	assertExistence((*cit)->getTargetnode());

      } catch (Exception<NwkParser> &e){

	throw e.prependMsg(String("edge-definition of node ") + (*nit)->getNumber() + ": ");

      }    
    }
  }
}
