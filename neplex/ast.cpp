#include "ast.hpp"

using namespace std;

string Coords::toString(){

  ostringstream output;

  output << '(' << x << ',' << y << ')';

  return output.str();

}

NwkAst::~NwkAst(){

  for ( list<NodeAst *>::iterator it = node_asts.begin(); it != node_asts.end(); it++ )
    delete *it;

}

string NwkAst::toString(){

  ostringstream output;

  output << getNumberOfNodes() << endl;

  for ( list<NodeAst *>::iterator it = node_asts.begin(); it != node_asts.end(); it++ )
    output << (*it)->toString() << endl;

  return output.str();

}

NodeAst::~NodeAst(){

  for ( list<ConnectionAst *>::iterator it = connection_asts.begin(); it != connection_asts.end(); it++ )
    delete *it;

}

string NodeAst::toString(){

  ostringstream output;

  output << number << " " << coords.toString() << ", " << inflow  << ": "; 

  for ( list<ConnectionAst *>::iterator it = connection_asts.begin(); it != connection_asts.end(); it++ )
    output << (*it)->toString() << "; ";

  return output.str();

}

string ConnectionAst::toString(){

  ostringstream output;

  output << targetnode << ", " << cost << ", " << capacity;

  return output.str();

}
