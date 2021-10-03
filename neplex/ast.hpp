#ifndef __AST_HPP__
#define __AST_HPP__

/*
  here are defined the abstract syntax trees for the input-data
*/

#include <list>
#include <sstream>
#include "types.hpp"

class NodeAst;
class ConnectionAst;
class Coords;

class Coords{

public:

  long x;

  long y;

  Coords(long x, long y) : x(x), y(y){}

  std::string toString();
  
};

class NwkAst{
  
protected:
  
  numbertype number_of_nodes;

  std::list<NodeAst *> node_asts;

public:

  NwkAst() : number_of_nodes(0){}
  ~NwkAst();

  void setNumberOfNodes(numbertype number_of_nodes){ this->number_of_nodes = number_of_nodes; }
  numbertype getNumberOfNodes(){ return number_of_nodes; }
  void addNode(NodeAst * node){ node_asts.push_back(node); }
  const std::list<NodeAst *> & getNodes(){ return node_asts; }

  std::string toString();

};

class NodeAst{

protected:

  numbertype number;

  Coords coords;

  double inflow;
  
  // are only outedges
  std::list<ConnectionAst *> connection_asts;

public:

  NodeAst() : number(0), coords(0,0), inflow(0){}

  ~NodeAst();

  void setNumber(numbertype number){ this->number = number; }
  numbertype getNumber(){ return number; }
  void setCoords(Coords coords){ this->coords = coords; }
  Coords getCoords(){ return coords; }
  void setInflow(double inflow){ this->inflow = inflow; }
  double getInflow(){ return inflow; } 
  void addConnection(ConnectionAst *edge){ connection_asts.push_back(edge); }
  const std::list<ConnectionAst *> & getConnections(){ return connection_asts; }

  std::string toString();

};
  
class ConnectionAst{

protected:

  numbertype targetnode;

  long cost;

  long capacity;

  long lowerbound;

public:

  ConnectionAst() : targetnode(0), cost(0), capacity(0), lowerbound(0){}

  void setTargetnode(numbertype targetnode){ this->targetnode = targetnode; }
  numbertype getTargetnode(){ return targetnode; }
  
  void setCost(long cost){ this->cost = cost; }
  long getCost(){ return cost; }

  void setCapacity(long capacity){ this->capacity = capacity; }
  long getCapacity(){ return capacity; }

  void setLowerBound(long lowerbound){ this->lowerbound = lowerbound; }
  long getLowerBound(){ return lowerbound; }

  std::string toString();

};

#endif
