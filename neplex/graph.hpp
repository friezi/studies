#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <set>
#include <map>
#include <sstream>
#include <algorithm>
#include <utility>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <exception.hpp>
#include <utils.hpp>
#include <cmdlparser.hpp>
#include <mathexpression.hpp>
#include "types.hpp"
#include "ast.hpp"

#define COMP_NONE 0
#define COMP_T 1
#define COMP_L 2
#define COMP_U 3
#define TYPE_IN 4
#define TYPE_OUT 5

class Network;
class Node;
class Edge;

typedef std::pair<Edge *, char> edgeinfo;

// second should be the calculated theta
typedef std::pair<edgeinfo,double> changeinfo;

class Cycle{

public:

  Node * apex;

  std::list<Edge *> *forward;
  std::list<Edge *> *back;

  Cycle(Node *apex, std::list<Edge *> *forward, std::list<Edge *> *back) :
    apex(apex), forward(forward), back(back){}
  ~Cycle();

  std::string toString();

};

class Network{

protected:

  numbertype number_of_nodes;

  Node *root;

  long maxcost;

  std::set<Node *> nodes;

  std::set<Edge *> T;
  std::set<Edge *> L;
  std::set<Edge *> U;

  // working-variables

  std::set<Node *> w_nodes;

  // number of updated nodes in computeDescendantIndices()
  unsigned int updated;

  unsigned long iterations;

  bool option_info;

  bool option_debug;

  bool option_edgechange;

  bool option_processing;

  bool option_intercost;

  bool option_nonoptimized;

  bool option_updated;

  bool option_iteration;

  void deleteNodes();

private:

  Network(Network &){}

public:

  Network(NwkAst *nwk, cmdl::CmdlParser &cmdlparser) throw (Exception<Network>,ExceptionBase);
  ~Network();

  void setNumberOfNodes(numbertype number_of_nodes){ this->number_of_nodes = number_of_nodes; }
  numbertype getNumberOfNodes() const { return number_of_nodes; }

  void setRoot(Node *node){ root = node; }
  Node * getRoot() const { return root; }

//   void setMaxCost(long cost){ maxcost = cost; }
  long getMaxCost() const { return maxcost; }

  void addNode(Node *node){ nodes.insert(node); }
  const std::set<Node *> & getNodes() const { return nodes; }

  double networksimplex() throw (Exception<Network>,Exception<ExceptionBase>);

  void constructAntiparallelGraph();

  void initBasicFeasibleSolution();

  void initInitialThread();

  void computePotentials();

  void updatePotentials(edgeinfo entering_edge_info, edgeinfo leaving_edge_info);

  void computeFlows() throw (ExceptionBase);

  void updateFlows(Cycle *cycle, double theta);
  
  Node * getExitingThreadAndUnmarkNodes(Node * descendant_root);

  void computeIndices() throw (Exception<Network>);

  Node * computeDescendantIndices(Node * curr) throw (Exception<Network>);

  void updateIndices(Node * apex) throw (Exception<Network>);

  long calculateReducedCosts(Edge * edge);

  double calculateCost();

  edgeinfo determineEnteringEdge();

  changeinfo determineLeavingEdge(Cycle * cycle) throw (Exception<Network>);

  // must only be called on an already correctly indicated network
  void updateTree(edgeinfo entering_edge_info, edgeinfo leaving_edge_info);

  void infeasibleCheck() throw (Exception<Network>);

  void unboundedCheck(Cycle * cycle) throw (Exception<Network>);
  
  void unmarkNodes();

  Cycle * identifyCycle(Edge * entering_edge) throw (Exception<Network>);

  Edge * isEdgeContained(std::set<Edge *> * edgeset, Node * source, Node * target);

  std::string edgeListToString(std::set<Edge *> * edgelist);

  unsigned long getIterations(){ return iterations; }
  
  std::string info() const ;
  std::string toString() const ;
  std::string treeToString() const ;
  std::string uToString() const ;
  std::string lToString() const ;
  static std::string nonScientific(double value);

};

class Node{

protected:

  numbertype number;

  Coords coords;

  double inflow;

  // tree-parameters
  Node * pred;
  
  unsigned long depth;

  Node * thread;

  long potential;

  // working-variables

  double w_inflow;

  bool marked;
  
public:
  std::set<Edge *> w_treeedges;

protected:
  std::set<Edge *> outedges;
  std::set<Edge *> inedges;
  std::set<Edge *> treeedges;

public:

  Node(numbertype number, Coords coords, double inflow) :
    number(number), coords(coords), inflow(inflow), pred(0), depth(0), thread(0), potential(0), w_inflow(0), marked(false){}
  ~Node();  

  void addOutEdge(Edge *edge);
  void addInEdge(Edge *edge);

  numbertype getNumber() const { return number; }
  Coords getCoords() const { return coords; }
  double getInflow() const { return inflow; }

  void setPred(Node * node){ pred = node; }
  Node * getPred() const { return pred; }

  void setDepth(unsigned long depth){ this->depth = depth; }
  unsigned long getDepth() const { return depth; }

  void setThread(Node * node){ thread = node; }
  Node * getThread() const { return thread; }

  void setPotential(long potential){ this->potential = potential; }
  long getPotential() const { return potential; }

  void setWInflow(double inflow){ w_inflow = inflow; }
  double getWInflow(){ return w_inflow; }

  void setMarked(){ marked = true; }
  void clearMarked(){ marked = false; }
  bool isMarked(){ return marked; }
  
  std::set<Edge *> & getOutEdges(){ return outedges; }
  std::set<Edge *> & getInEdges(){ return inedges; }
  std::set<Edge *> & getTreeEdges(){ return treeedges; }

  Edge * isOutEdge(Node *targetnode);
  Edge * isInEdge(Node *sourcenode);

  edgeinfo getTreeEdge(Node *neighbour);

  std::string toString() const ;
  std::string treeToString() const ;

  unsigned long getTreeEdgeDegree(std::set<Edge *> * edgeset);
  bool isLeafNode(std::set<Edge *> * edgeset);

};

class EdgeParameters{

public:
  
  long cost;
  
  long capacity;  
  
  long lowerbound;
  
  double flow;
  
  char component;

  unsigned long references;

  EdgeParameters(long cost, long capacity, long lowerbound, double flow, char component = COMP_NONE) :
    cost(cost),capacity(capacity),lowerbound(lowerbound),flow(flow),component(component), references(0){}

  void increaseReferences(){ references++; }
  void decreaseReferences(){ references--; }
  unsigned long getReferences(){ return references; }
  
  std::string toString() const ;

};
  
class Edge{

  friend class Node;

protected:

  // the linked node
  Node *sourcenode;
  Node *targetnode;

  EdgeParameters *parameters;

  unsigned long references;

  unsigned long cycle_position;

public:

//   static Edge *newOutEdge(Node *targetnode, Node *sourcenode, EdgeParameters *parameters);
//   static Edge *newInEdge(Node *targetnode, Node *sourcenode, EdgeParameters *parameters);

  Edge(Node *sourcenode, Node *targetnode, EdgeParameters *parameters);

  ~Edge();

  static void edgeDelete(Edge * edge);
 
  void setTargetnode(Node *targetnode){ this->targetnode = targetnode; }
  Node * getTargetnode() const { return targetnode; }
 
  void setSourcenode(Node *sourcenode){ this->sourcenode = sourcenode; }
  Node * getSourcenode() const { return sourcenode; }
  
  void setCost(long cost){ this->parameters->cost = cost; }
  long getCost() const { return parameters->cost; }

  void setCapacity(long capacity){ this->parameters->capacity = capacity; }
  long getCapacity() const { return parameters->capacity; }

  void setLowerBound(long lowerbound){ this->parameters->lowerbound = lowerbound; }
  long getLowerBound() const { return parameters->lowerbound; }

  void setFlow(double flow){ parameters->flow = flow; }
  double getFlow() const { return parameters->flow; }

  void setComponent(char value){ parameters->component = value; }
  char isComponent() const { return parameters->component; }

  void increaseReferences(){ references++; }
  void decreaseReferences(){ references--; }
  unsigned long getReferences(){ return references; }

  void setCyclePosition(unsigned long position){ cycle_position = position; }
  unsigned long getCyclePosition(){ return cycle_position; }

  std::string toString() const ;

};

#endif
