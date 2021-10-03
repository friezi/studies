#include "graph.hpp"

using namespace std;
using namespace cmdl;
using namespace utils;
using namespace mexp;

Cycle::~Cycle(){

  delete forward;
  delete back;

}

string Cycle::toString(){

  ostringstream out;

  out << "apex: " << apex->getNumber() << " f=";
  
  for ( list<Edge *>::iterator eit = forward->begin(); eit != forward->end(); eit++ )
    out << "(" << (*eit)->getSourcenode()->getNumber() << "," << (*eit)->getTargetnode()->getNumber() << "),";
  
  out << " b=";
  
  for ( list<Edge *>::iterator eit = back->begin(); eit != back->end(); eit++ )
    out << "(" << (*eit)->getSourcenode()->getNumber() << "," << (*eit)->getTargetnode()->getNumber() << "),";

  return out.str();

}

Network::Network(NwkAst *nwk, CmdlParser &cmdlparser) throw (Exception<Network>,ExceptionBase) {

  String thisMethod = "Network()";

  option_info = cmdlparser.checkShortoption('i');
  option_debug = cmdlparser.checkShortoption('d');
  option_edgechange = cmdlparser.checkShortoption('e');
  option_processing = cmdlparser.checkShortoption('p');
  option_intercost = cmdlparser.checkShortoption('c');
  option_nonoptimized = cmdlparser.checkShortoption('n');
  option_updated = cmdlparser.checkShortoption('u');
  option_iteration = cmdlparser.checkShortoption('j');
  
  if ( option_processing )
    cout << thisMethod << endl;

  maxcost = 0;

  try{

    const list<NodeAst *> & nodeasts = nwk->getNodes();

    setNumberOfNodes(nwk->getNumberOfNodes());

    Node *node;
    map<numbertype,Node *> nodedict;

    for ( list<NodeAst *>::const_iterator nit = nodeasts.begin(); nit != nodeasts.end(); nit++ ){
    
      node = new Node((*nit)->getNumber(),(*nit)->getCoords(),(*nit)->getInflow());
      nodes.insert(node);
      nodedict[(*nit)->getNumber()] = node;

    }

    root = *nodes.begin();

    Node *sourcenode,*targetnode;
    Edge * edge;
    EdgeParameters *edge_parameters;

    for ( list<NodeAst *>::const_iterator nit = nodeasts.begin(); nit != nodeasts.end(); nit++ ){
      for ( list<ConnectionAst *>::const_iterator cit = (*nit)->getConnections().begin(); cit != (*nit)->getConnections().end(); cit++ ){

	if ( (LONG_MAX - abs((*cit)->getCost())) < maxcost ){
	  
	  if ( option_debug )
	    cout << "LONG_MAX: " << LONG_MAX << " maxcost: " << maxcost << " abs(cost): " << abs((*cit)->getCost()) << " difference: "
		 << (LONG_MAX - abs((*cit)->getCost())) << endl;
	  throw Exception<Network>("upper bound of maximal cost exceeded!");

	}

	maxcost += abs((*cit)->getCost());

	sourcenode = nodedict[(*nit)->getNumber()];
	targetnode = nodedict[(*cit)->getTargetnode()];

	edge_parameters = new EdgeParameters((*cit)->getCost(),(*cit)->getCapacity(),(*cit)->getLowerBound(),0,COMP_L);
	edge = new Edge(sourcenode,targetnode,edge_parameters);

	L.insert(edge);

	sourcenode->addOutEdge(edge);
	targetnode->addInEdge(edge);
      
      }
    }

    constructAntiparallelGraph();
  
  } catch (ExceptionBase &e){

    deleteNodes();
    throw e.prependMsg(thisMethod + ": ");

  }
}

Network::~Network(){
  
  deleteNodes();

}

void Network::deleteNodes(){

  for ( set<Node *>::iterator it = nodes.begin(); it != nodes.end(); it++ )
    delete *it;

}

void Network::constructAntiparallelGraph(){

  Edge *new_outedge, *new_inedge, *inedge;
  EdgeParameters *new_outedge_parameters, *new_inedge_parameters;
  Node *artificial;
  list<Edge *> deleted_edges;
  string thisMethod = "constructAntiparallelGraph()";

  if ( option_processing )
    cout << thisMethod << endl;

  unmarkNodes();

  for ( set<Node *>::iterator nit = nodes.begin(); nit != nodes.end(); nit++ ){

    (*nit)->setMarked();

    deleted_edges.clear();
    
    for ( set<Edge *>::iterator eit = (*nit)->getOutEdges().begin(); eit != (*nit)->getOutEdges().end(); eit++ ){
      
      if ( (*eit)->getTargetnode()->isMarked() )
	continue;
      
      if ( ( inedge = (*nit)->isInEdge((*eit)->getTargetnode())) != 0 ){
	  // found parallel arcs
	
	deleted_edges.push_back(inedge);
	
	artificial = new Node(getNumberOfNodes(),Coords(0,0),0);
	setNumberOfNodes(getNumberOfNodes()+1);
	nodes.insert(artificial);
	
	new_outedge_parameters = new EdgeParameters(inedge->getCost(),inedge->getCapacity(),0,inedge->getFlow(),COMP_L);
	new_outedge = new Edge((*eit)->getTargetnode(),artificial,new_outedge_parameters);
	
	new_inedge_parameters = new EdgeParameters(0,inedge->getCapacity(),0,inedge->getFlow(),COMP_L);
	new_inedge = new Edge(artificial,*nit,new_inedge_parameters);
	
	(*eit)->getTargetnode()->addOutEdge(new_outedge);
	artificial->addInEdge(new_outedge);
	artificial->addOutEdge(new_inedge);
	(*nit)->addInEdge(new_inedge);

	L.insert(new_inedge);
	L.insert(new_outedge);
	
      }

    }
	
    for ( list<Edge *>::iterator eit = deleted_edges.begin(); eit != deleted_edges.end(); eit++ ){

      (*eit)->getSourcenode()->getOutEdges().erase(*eit);
      (*eit)->getTargetnode()->getInEdges().erase(*eit);
      L.erase(*eit);
      delete *eit;

    }
    
  }

}

double Network::networksimplex() throw (Exception<Network>,Exception<ExceptionBase>){

  edgeinfo entering_edge_info,leaving_edge_info;
  changeinfo change_info;
  Cycle *cycle;
  string thisMethod = "networksimplex()";

  iterations = 0;

  if ( option_processing )
    cout << thisMethod << endl;

  initBasicFeasibleSolution();

  // @TODO
  while ( true ){

    iterations++;

    if ( option_iteration )
      cout << "ITERATION: " << iterations << endl;
    
    if ( (entering_edge_info = determineEnteringEdge()).first == 0 )
      break;

    if ( option_edgechange )
      cout << "entering edge: (" << entering_edge_info.first->getSourcenode()->getNumber() << "," << entering_edge_info.first->getTargetnode()->getNumber()
	   << ") from " << (leaving_edge_info.second == COMP_U ? "U" : "L") << endl;
    
    cycle = identifyCycle(entering_edge_info.first);

    if ( option_debug )
      cout << "cycle:" << endl << cycle->toString() << endl;

    unboundedCheck(cycle);
    
    change_info = determineLeavingEdge(cycle);

    leaving_edge_info = change_info.first;
    
    if ( option_edgechange )
      cout << "leaving edge: (" << leaving_edge_info.first->getSourcenode()->getNumber() << "," << leaving_edge_info.first->getTargetnode()->getNumber()
	   << ") to " << (leaving_edge_info.second == COMP_U ? "U" : "L") << endl;

    updateFlows(cycle,change_info.second);
    
    updateTree(entering_edge_info,leaving_edge_info);

    if ( option_debug ){
      
      cerr << "updated: tree:" << endl << treeToString() << endl;
      cerr << "U:" << endl << uToString() << endl;
      cerr << "L:" << endl << lToString() << endl;

    }

    updateIndices(cycle->apex);
 
    updatePotentials(entering_edge_info,leaving_edge_info);

    if ( option_debug ){
      
      cerr << "updated flows tree:" << endl << treeToString() << endl;
      cerr << "U:" << endl << uToString() << endl;
      cerr << "L:" << endl << lToString() << endl;

    }

    if ( option_intercost ){

      double cost = calculateCost();
      cerr << "current costs: " << nonScientific(cost) << endl;

    }
    
    delete cycle;
    
  }

  infeasibleCheck();

  return calculateCost();
  
}

void Network::infeasibleCheck() throw (Exception<Network>){

  string thisMethod = "infeasibleCheck()";

  if ( option_processing )
    cout << thisMethod << endl;

  for ( set<Edge *>::iterator eit = root->getOutEdges().begin(); eit != root->getOutEdges().end(); eit++ )
    if ( (*eit)->getFlow() != 0 )
      throw Exception<Network>("infeasible!");

  for ( set<Edge *>::iterator eit = root->getInEdges().begin(); eit != root->getInEdges().end(); eit++ )
    if ( (*eit)->getFlow() != 0 )
      throw Exception<Network>("infeasible!");

}

void Network::unboundedCheck(Cycle * cycle) throw (Exception<Network>){

  string thisMethod = "unboundedCheck()";
  
  if ( option_processing )
    cout << thisMethod << endl;
  
  if ( cycle->forward->empty() ){

    for ( list<Edge *>::const_iterator eit = cycle->back->begin(); eit != cycle->back->end(); eit++ )
      if ( (*eit)->getCapacity() != INFINIT_CAPACITY )
	return;
  
    throw Exception<Network>("unbounded!");

  } else if ( cycle->back->empty() ){

    for ( list<Edge *>::const_iterator eit = cycle->forward->begin(); eit != cycle->forward->end(); eit++ )
      if ( (*eit)->getCapacity() != INFINIT_CAPACITY )
	return;
  
    throw Exception<Network>("unbounded!");

  }

}

void Network::initBasicFeasibleSolution(){

  string thisMethod = "initBasicFeasibleSolution()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  Node *artificial = new Node(getNumberOfNodes(),Coords(0,0),0);

  root = artificial;

  setNumberOfNodes(getNumberOfNodes()+1);

  EdgeParameters *edge_parameters;
  Edge * edge;

  for (set<Node *>::const_iterator nit = nodes.begin(); nit != nodes.end(); nit++ ){

    edge_parameters = new EdgeParameters(getMaxCost()+1,INFINIT_CAPACITY,0,fabs((*nit)->getInflow()),COMP_T);

    if ( (*nit)->getInflow() >= 0 ){

      edge = new Edge((*nit),artificial,edge_parameters);

      T.insert(edge);

      (*nit)->addOutEdge(edge);
      artificial->addInEdge(edge);

    } else {
      
      edge = new Edge(artificial,(*nit),edge_parameters);
      
      T.insert(edge);
      
      (*nit)->addInEdge(edge);
      artificial->addOutEdge(edge);
      
    }

    (*nit)->getTreeEdges().insert(edge);
    artificial->getTreeEdges().insert(edge);

  }

  nodes.insert(artificial);

  computeIndices();

  computePotentials();

  if ( option_debug )
    cout << toString() << endl;
 
  if ( option_debug )    
    cout << endl << "tree:" << endl << treeToString() << endl;
  
  computeFlows();
  
  if ( option_debug ){
    
    cout << endl << endl << "tree:" << endl << treeToString() << endl;
    
    cout << endl << toString() << endl;
    
  }
  
}

void Network::unmarkNodes(){
  
  for ( set<Node *>::iterator nit = nodes.begin(); nit != nodes.end(); nit++ )
    (*nit)->clearMarked();
  
}

Node * Network::getExitingThreadAndUnmarkNodes(Node * descendant_root){

  Node * curr = descendant_root;
  
  do {

    curr->clearMarked();
    curr = curr->getThread();

  } while ( (curr != descendant_root) and (curr->getDepth() > descendant_root->getDepth()) );
  // curr == descendant_root occurs if descendant_root == root

  return curr;

}

void Network::updateIndices(Node * apex) throw (Exception<Network>){

  string thisMethod = "updateIndices()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  Node * threadpred, * exiting_thread;
  updated=0;

  exiting_thread = getExitingThreadAndUnmarkNodes(apex);

  threadpred = computeDescendantIndices(apex);

  if ( threadpred != apex )
    threadpred->setThread(exiting_thread);

  if ( option_updated )
    cout << "updated " << updated << " nodes of " << number_of_nodes << endl;

}

void Network::computeIndices() throw (Exception<Network>){

  string thisMethod = "computeIndices()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  Node * threadpred;
  updated=0;

  unmarkNodes();
  
  threadpred = computeDescendantIndices(root);

  if ( threadpred != root )
    threadpred->setThread(root);

  if ( option_updated )
    cout << "updated " << updated << " nodes of " << number_of_nodes << endl;
  
}

Node * Network::computeDescendantIndices(Node * curr) throw (Exception<Network>){

  String thisMethod = "computeDescendantIndices()";
  
  Node * threadpred = curr;

  curr->setMarked();
  updated++;
    
  for ( set<Edge *>::const_iterator eit = curr->getTreeEdges().begin(); eit != curr->getTreeEdges().end(); eit++ ){

    if ( curr == (*eit)->getSourcenode() ){ // it's an outedge
      
      // skip the edges pointing to the pred
      if ( curr->getPred() && ( curr->getPred() == (*eit)->getTargetnode() ) )
	continue;

      if ( (*eit)->getTargetnode()->isMarked() )
	throw Exception<Network>(String(thisMethod) + String(": edge: (") + (*eit)->getSourcenode()->getNumber()+ "," + (*eit)->getTargetnode()->getNumber()
				 + ")" + String(" targetnode ") + (*eit)->getTargetnode()->getNumber() + " already visited!"); 
      
      threadpred->setThread((*eit)->getTargetnode());
      (*eit)->getTargetnode()->setPred(curr);
      (*eit)->getTargetnode()->setDepth(curr->getDepth()+1);
      threadpred = computeDescendantIndices((*eit)->getTargetnode());
      
    } else if ( curr == (*eit)->getTargetnode() ){ // it's an inedge
      
      // skip the edges pointing from the pred
      if ( curr->getPred() && ( curr->getPred() == (*eit)->getSourcenode() ) )
      continue;
      
      if ( (*eit)->getSourcenode()->isMarked() )
	throw Exception<Network>(String(thisMethod) + String(": edge: (") + (*eit)->getSourcenode()->getNumber()+ "," + (*eit)->getTargetnode()->getNumber()
				 + ")" + String(" sourcenode ") + (*eit)->getSourcenode()->getNumber() + " already visited!"); 
      
      threadpred->setThread((*eit)->getSourcenode());
      (*eit)->getSourcenode()->setPred(curr);
      (*eit)->getSourcenode()->setDepth(curr->getDepth()+1);
      threadpred = computeDescendantIndices((*eit)->getSourcenode());
      
    } else
      throw Exception<Network>(thisMethod + ": curr no node in edge!");

  }
  
  return threadpred;
  
}

void Network::initInitialThread(){

  computeIndices();
  
//   Node * threadpred = root;

//   for ( set<Edge *>::const_iterator eit = root->getOutEdges().begin(); eit != root->getOutEdges().end(); eit++ ){
    
//     threadpred->setThread((*eit)->getTargetnode());
//     (*eit)->getTargetnode()->setPred(root);
//     (*eit)->getTargetnode()->setDepth(1);
//     threadpred = (*eit)->getTargetnode();

//   }

//   for ( set<Edge *>::const_iterator eit = root->getInEdges().begin(); eit != root->getInEdges().end(); eit++ ){
    
//     threadpred->setThread((*eit)->getSourcenode());
//     (*eit)->getSourcenode()->setPred(root);
//     (*eit)->getSourcenode()->setDepth(1);
//     threadpred = (*eit)->getSourcenode();

//   }

//   if ( threadpred != root )
//     threadpred->setThread(root);

}

void Network::computePotentials(){

  string thisMethod = "computePotentials()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  root->setPotential(0);

  Node * j = root->getThread();
  Node * i;
  edgeinfo e_info;

  if ( j == 0 )
    return;
  
  while ( j != root ){

    i = j->getPred();
    
    e_info = j->getTreeEdge(i);

    if ( e_info.second == TYPE_IN )
      j->setPotential(i->getPotential() - e_info.first->getCost());

    else if ( e_info.second  == TYPE_OUT )
      j->setPotential(i->getPotential() + e_info.first->getCost());

    j = j->getThread();

  }

}

// Ahuja's method does not work properly; this here does it better
void Network::updatePotentials(edgeinfo entering_edge_info, edgeinfo leaving_edge_info){
  
  Node *y,*z;
  Edge *entering_edge = entering_edge_info.first;
  Edge *leaving_edge = leaving_edge_info.first;  
  long change;

  if ( entering_edge == leaving_edge )
    return;
  
  if ( entering_edge->getTargetnode()->getDepth() > entering_edge->getSourcenode()->getDepth() ){

    y = entering_edge->getTargetnode();
    change = -calculateReducedCosts(entering_edge);


  }else{

    y = entering_edge->getSourcenode();
    change = calculateReducedCosts(entering_edge);

  }  
  
  y->setPotential(y->getPotential() + change);
  
  z = y->getThread();
  
  while ( z->getDepth() > y->getDepth() ){
    
    z->setPotential(z->getPotential() + change);
    z = z->getThread();
    
  }
  
}

void Network::computeFlows() throw (ExceptionBase){

  String thisMethod = "computeFlows()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  // init working-variables

  try{

    for ( set<Node *>::iterator nit = nodes.begin(); nit != nodes.end(); nit++ ){

      (*nit)->setWInflow((*nit)->getInflow());
      (*nit)->w_treeedges = (*nit)->getTreeEdges();

    }

    for ( set<Edge *>::iterator eit = U.begin(); eit != U.end(); eit++ ){

      (*eit)->getSourcenode()->setWInflow((*eit)->getSourcenode()->getWInflow() - (*eit)->getCapacity());
      (*eit)->getTargetnode()->setWInflow((*eit)->getTargetnode()->getWInflow() + (*eit)->getCapacity());

    }

    w_nodes = nodes;

    Node *i, *j;
    Edge *edge;

    while ( w_nodes.size() > 1 ){

      for ( set<Node *>::iterator nit = w_nodes.begin(); nit != w_nodes.end(); nit++ ){

	if ( (*nit) == root )
	  continue;

	j = (*nit);

	if ( j->isLeafNode(&j->w_treeedges) == false )
	  continue;
      
	i = j->getPred();

	if ( (edge = isEdgeContained(&j->w_treeedges,i,j)) != 0 )
	  edge->setFlow(-1 * j->getWInflow());
	else if ( (edge = isEdgeContained(&j->w_treeedges,j,i)) != 0 )
	  edge->setFlow(j->getWInflow());
	else
	  throw Exception<Network>(String("treeedge missing!"));

	i->setWInflow(i->getWInflow() + j->getWInflow());

	w_nodes.erase(j);

	i->w_treeedges.erase(edge);

	break;
      }     
      
    }

    for ( set<Edge *>::iterator eit = U.begin(); eit != U.end(); eit++ )
      (*eit)->setFlow((*eit)->getCapacity());

    for ( set<Edge *>::iterator eit = L.begin(); eit != L.end(); eit++ )
      (*eit)->setFlow(0);
  
  } catch (ExceptionBase & e){
    
    throw e.prependMsg(thisMethod);
    
  }
  
}

void Network::updateFlows(Cycle *cycle, double theta){

  string thisMethod = "updateFlows()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  if ( theta == 0 )
    return;

  for (list<Edge *>::iterator eit = cycle->forward->begin(); eit != cycle->forward->end(); eit++ )
    (*eit)->setFlow((*eit)->getFlow() + theta);

  for (list<Edge *>::iterator eit = cycle->back->begin(); eit != cycle->back->end(); eit++ )
    (*eit)->setFlow((*eit)->getFlow() - theta);

}

long Network::calculateReducedCosts(Edge * edge){

//   String thisMethod = "calculateReducedCosts()";
  
//   if ( option_processing )
//     cout << thisMethod << endl;

  return edge->getCost() - edge->getSourcenode()->getPotential() + edge->getTargetnode()->getPotential();
  
}

edgeinfo Network::determineEnteringEdge(){

  String thisMethod = "determineEnteringEdge()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  for (set<Edge *>::iterator eit = L.begin(); eit != L.end(); eit++ ){

    if ( calculateReducedCosts((*eit)) < 0 )
      return edgeinfo(*eit,COMP_L);

  }

  for (set<Edge *>::iterator eit = U.begin(); eit != U.end(); eit++ ){

    if ( calculateReducedCosts((*eit)) > 0 )
      return edgeinfo(*eit,COMP_U);

  }

  return edgeinfo(0,COMP_NONE);
 
}

changeinfo Network::determineLeavingEdge(Cycle * cycle) throw (Exception<Network>){

  String thisMethod = "determineLeavingEdge()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  list<Edge *> * forward = cycle->forward;
  list<Edge *> * back = cycle->back;

  double theta_back = DBL_MAX;
  double theta_forward = DBL_MAX;
  Edge * backedge = 0;
  Edge * forwardedge = 0;

  if ( back->empty() != true ){

    for ( list<Edge *>::iterator eit = back->begin(); eit != back->end(); eit++ ){
      
      if (  (*eit)->getFlow() <= theta_back ){
	
	theta_back = (*eit)->getFlow();	
	backedge = *eit;
	
      }
      
    }
    
  }
  
  if ( forward->empty() != true ){
    
    for ( list<Edge *>::iterator eit = forward->begin(); eit != forward->end(); eit++ ){
      
      if (  ((*eit)->getCapacity() - (*eit)->getFlow()) <= theta_forward ){
	
	theta_forward = ((*eit)->getCapacity() - (*eit)->getFlow());
	forwardedge = *eit;
	
      }
      
    }
    
  }

  if ( theta_forward == theta_back ){

    if ( forwardedge->getCyclePosition() < backedge->getCyclePosition() ){
      
      return changeinfo(edgeinfo(backedge,COMP_L),theta_forward);

    } else if ( forwardedge->getCyclePosition() > backedge->getCyclePosition() ){

      return changeinfo(edgeinfo(forwardedge,COMP_U),theta_forward);

    } else
      throw Exception<Network>(thisMethod + ": forwardedge->getCyclePosition() == backedge-<getCyclePosition()!");
    
  } else if ( theta_forward < theta_back ){

    return changeinfo(edgeinfo(forwardedge,COMP_U),theta_forward);
  
  } else{

    return changeinfo(edgeinfo(backedge,COMP_L),theta_back);

  }

}

Cycle * Network::identifyCycle(Edge * entering_edge) throw (Exception<Network>){

  String thisMethod = "identifyCycle()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  Node *i = entering_edge->getSourcenode(), *j = entering_edge->getTargetnode();
  list<Edge *> * k_forward = new list<Edge *>();
  list<Edge *> * l_forward = new list<Edge *>();
  list<Edge *> * k_back = new list<Edge *>();
  list<Edge *> * l_back = new list<Edge *>();
  Edge *edge;

  // determining the position of edge whithin cycle
  unsigned long pos_count_k, pos_count_l, max_k_pos, max_l_pos;

  pos_count_k = pos_count_l = 1;

  while ( i != j ){

    if ( i->getDepth() > j->getDepth() ){

      if ( ( (edge = isEdgeContained(&i->getOutEdges(),i,i->getPred())) != 0 ) && ( edge->isComponent() == COMP_T ))
	k_back->push_front(edge);

      else if ( ( (edge = isEdgeContained(&i->getInEdges(),i->getPred(),i)) != 0 ) && ( edge->isComponent() == COMP_T ) )
	k_forward->push_front(edge);

      if ( edge == 0 )
	throw Exception<Network>(thisMethod + String(": edge is 0: (") + i->getNumber() + "," + i->getPred()->getNumber() + ")");

      edge->setCyclePosition(pos_count_k++);

      i = i->getPred();

    } else if ( j->getDepth() > i->getDepth() ){
 
      if ( (edge = isEdgeContained(&j->getInEdges(),j->getPred(),j)) != 0 && ( edge->isComponent() == COMP_T ) )
	l_back->push_back(edge);
 
      else if ( (edge = isEdgeContained(&j->getOutEdges(),j,j->getPred())) != 0 && ( edge->isComponent() == COMP_T ) )
	l_forward->push_back(edge);

      if ( edge == 0 )
	throw Exception<Network>(thisMethod + String("edge is 0: (") + j->getNumber() + "," + j->getPred()->getNumber() + ")");

      edge->setCyclePosition(pos_count_l++);
     
      j = j->getPred();

    } else{

      if ( (edge = isEdgeContained(&i->getOutEdges(),i,i->getPred())) != 0 && ( edge->isComponent() == COMP_T ) )
	k_back->push_front(edge);

      else if ( (edge = isEdgeContained(&i->getInEdges(),i->getPred(),i)) != 0 && ( edge->isComponent() == COMP_T ) )
	k_forward->push_front(edge);

      if ( edge == 0 )
	throw Exception<Network>(thisMethod + String("edge is 0: (") + i->getNumber() + "," + i->getPred()->getNumber() + ")");

      edge->setCyclePosition(pos_count_k++);
 
      if ( (edge = isEdgeContained(&j->getInEdges(),j->getPred(),j)) != 0 && ( edge->isComponent() == COMP_T ) )
	l_back->push_back(edge);
 
      else if ( (edge = isEdgeContained(&j->getOutEdges(),j,j->getPred())) != 0 && ( edge->isComponent() == COMP_T ) )
	l_forward->push_back(edge);

      if ( edge == 0 )
	throw Exception<Network>(thisMethod + String("edge is 0: (") + j->getNumber() + "," + j->getPred()->getNumber() + ")");

      edge->setCyclePosition(pos_count_l++);

      i = i->getPred();
      j = j->getPred();

    }
  }

  max_k_pos = pos_count_k;
  max_l_pos = pos_count_l;

  list<Edge *> *forward = new list<Edge *>();
  list<Edge *> *back = new list<Edge *>();

  if ( entering_edge->isComponent() == COMP_L ){

    for ( list<Edge *>::iterator eit = k_forward->begin(); eit != k_forward->end(); eit++ ){

      forward->push_back(*eit);
      (*eit)->setCyclePosition(max_k_pos - (*eit)->getCyclePosition());

    }

    entering_edge->setCyclePosition(max_k_pos);
    forward->push_back(entering_edge);

    for ( list<Edge *>::iterator eit = l_forward->begin(); eit != l_forward->end(); eit++ ){

      forward->push_back(*eit);
      (*eit)->setCyclePosition((*eit)->getCyclePosition() + max_k_pos + 1);

    }

    for ( list<Edge *>::iterator eit = k_back->begin(); eit != k_back->end(); eit++ ){

      back->push_back(*eit);
      (*eit)->setCyclePosition(max_k_pos - (*eit)->getCyclePosition());

    }

    for ( list<Edge *>::iterator eit = l_back->begin(); eit != l_back->end(); eit++ ){

      back->push_back(*eit);
      (*eit)->setCyclePosition((*eit)->getCyclePosition() + max_k_pos + 1);

    }

  } else if ( entering_edge->isComponent() == COMP_U ){

    for ( list<Edge *>::reverse_iterator eit = l_back->rbegin(); eit != l_back->rend(); eit++ ){

      forward->push_back(*eit);
      (*eit)->setCyclePosition(max_l_pos - (*eit)->getCyclePosition());

    }

    for ( list<Edge *>::reverse_iterator eit = k_back->rbegin(); eit != k_back->rend(); eit++ ){

      forward->push_back(*eit);
      (*eit)->setCyclePosition((*eit)->getCyclePosition() + max_l_pos + 1);

    }

    for ( list<Edge *>::reverse_iterator eit = l_forward->rbegin(); eit != l_forward->rend(); eit++ ){

      back->push_back(*eit);
      (*eit)->setCyclePosition(max_l_pos - (*eit)->getCyclePosition());

    }
    
    entering_edge->setCyclePosition(max_l_pos);
    back->push_back(entering_edge);

    for ( list<Edge *>::reverse_iterator eit = k_forward->rbegin(); eit != k_forward->rend(); eit++ ){

      back->push_back(*eit);
      (*eit)->setCyclePosition((*eit)->getCyclePosition() + max_l_pos + 1);

    }

  }

  delete k_forward;
  delete k_back;
  delete l_forward;
  delete l_back;

  return new Cycle(i,forward,back);

}

void Network::updateTree(edgeinfo entering_edge_info, edgeinfo leaving_edge_info){

  String thisMethod = "updateTree()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  Edge * entering = entering_edge_info.first;
  Edge * leaving = leaving_edge_info.first;
  char entering_from = entering_edge_info.second;
  char leaving_to = leaving_edge_info.second;

  entering->getSourcenode()->getTreeEdges().insert(entering);
  entering->getTargetnode()->getTreeEdges().insert(entering);

  leaving->getSourcenode()->getTreeEdges().erase(leaving);
  leaving->getTargetnode()->getTreeEdges().erase(leaving);

  if ( leaving_to == COMP_U )
    U.insert(leaving);
  else if ( leaving_to == COMP_L )
    L.insert(leaving);
  
  if ( entering_from == COMP_L )
    L.erase(entering);
  else if ( entering_from == COMP_U )
    U.erase(entering);

  T.insert(entering);
  entering->setComponent(COMP_T);

  T.erase(leaving);
  leaving->setComponent(leaving_to);

}

double Network::calculateCost(){

  String thisMethod = "calculateCost()";
  
  if ( option_processing )
    cout << thisMethod << endl;

  double result = 0;

  for ( set<Edge *>::iterator eit = T.begin(); eit != T.end(); eit++ )
    result += (*eit)->getFlow() * (*eit)->getCost();

  for ( set<Edge *>::iterator eit = U.begin(); eit != U.end(); eit++ )
    result += (*eit)->getCapacity() * (*eit)->getCost();

  return result;

}

Edge * Network::isEdgeContained(set<Edge *> * edgeset, Node * source, Node * target){
  
  for ( set<Edge *>::iterator eit = edgeset->begin(); eit != edgeset->end(); eit++ )
    if ( (*eit)->getSourcenode() == source && (*eit)->getTargetnode() == target )
      return *eit;
  
  return 0;
  
}

string Network::info() const{

  ostringstream out;

  out << "network-info:" << endl;

  out << "number of nodes: " << getNumberOfNodes() << " | " << "rootnode: " << root->getNumber() << " | " << "maximal cost-entity: " << maxcost
      << " | " << "no T:" << T.size() << " | " << "no L:" << L.size() << " | " << "no U:" << U.size() << endl;

  return out.str();

}
  
string Network::toString() const{

  ostringstream out;

  out << info() << endl;

  out << "nodes and edges:" << endl;

  for ( set<Node *>::const_iterator it = nodes.begin(); it != nodes.end(); it++ )
    out << (*it)->toString() << endl;

  return out.str();

}

string Network::edgeListToString(set<Edge *> * edgelist){

  ostringstream out;

  for ( set<Edge *>::iterator eit = edgelist->begin(); eit != edgelist->end(); eit++ )
    out << "(" << (*eit)->getSourcenode()->getNumber() << "," <<  (*eit)->getTargetnode()->getNumber() << ")";

  return out.str();

}

string Network::treeToString() const {

  ostringstream out;

  for ( set<Edge *>::iterator eit = T.begin(); eit != T.end(); eit++ )
    out << (*eit)->toString();

//   out << endl;

//   for ( set<Node *>::const_iterator nit = getNodes().begin(); nit != getNodes().end(); nit++ )
//     out << (*nit)->treeToString();
  
  return out.str();
  
}

string Network::uToString() const {

  ostringstream out;

  for ( set<Edge *>::iterator eit = U.begin(); eit != U.end(); eit++ )
    out << (*eit)->toString();
  
  return out.str();
  
}

string Network::lToString() const {

  ostringstream out;

  for ( set<Edge *>::iterator eit = L.begin(); eit != L.end(); eit++ )
    out << (*eit)->toString();
  
  return out.str();
  
}
  

void Node::addOutEdge(Edge *edge){

  outedges.insert(edge);
  edge->increaseReferences();

}
void Node::addInEdge(Edge *edge){

  inedges.insert(edge);
  edge->increaseReferences();

}

Edge * Node::isOutEdge(Node *targetnode){
  
  for ( set<Edge *>::const_iterator eit = getOutEdges().begin(); eit != getOutEdges().end(); eit++ )
    if ( (*eit)->getTargetnode() == targetnode )
      return *eit;

  return 0;

}

Edge * Node::isInEdge(Node *sourcenode){
  
  for ( set<Edge *>::const_iterator eit = getInEdges().begin(); eit != getInEdges().end(); eit++ )
    if ( (*eit)->getSourcenode() == sourcenode )
      return *eit;

  return 0;

}

edgeinfo Node::getTreeEdge(Node *neighbour){
  
  for ( set<Edge *>::const_iterator eit = getTreeEdges().begin(); eit != getTreeEdges().end(); eit++ ){
    
    if ( ((*eit)->getSourcenode() == this) and ((*eit)->getTargetnode() == neighbour) )
      return edgeinfo(*eit,TYPE_OUT);
    else if ( ((*eit)->getTargetnode() == this) and ((*eit)->getSourcenode() == neighbour) )
      return edgeinfo(*eit,TYPE_IN);
    
  }
  
  return edgeinfo(0,COMP_NONE);
  
}

unsigned long Node::getTreeEdgeDegree(set<Edge *> * edgeset){

  unsigned long count = 0;

  for ( set<Edge *>::const_iterator eit = edgeset->begin(); eit != edgeset->end(); eit++ )
    if ( (*eit)->isComponent() == COMP_T )
      count++;

  return count;

}

bool Node::isLeafNode(set<Edge *> * edgeset){

  return ( getTreeEdgeDegree(edgeset) == 1 );

}

string Node::treeToString() const {

  ostringstream out;
  
  out << "n=" << getNumber();

  if ( getPred() != 0 )
    out << ", p=" << getPred()->getNumber();

  if ( getThread() != 0 )
    out << ", t=" << getThread()->getNumber();

  out << ", d=" << getDepth() << ", p=" << getPotential() << endl;
  
  return out.str();

}

Node::~Node(){

  for ( set<Edge *>::iterator it = outedges.begin(); it != outedges.end(); it++ )
    Edge::edgeDelete(*it);

  for ( set<Edge *>::iterator it = inedges.begin(); it != inedges.end(); it++ )
    Edge::edgeDelete(*it);
  
}

string Node::toString() const{

  ostringstream out;

  out << "n=" << getNumber() << " b=" << getInflow();

  if ( outedges.empty() == false )
    out << " o=";
  
  for ( set<Edge *>::const_iterator it = outedges.begin(); it != outedges.end(); it++ )
    out << "(" << (*it)->toString() << ")";

  if ( inedges.empty() == false )
    out << " i=";
  
  for ( set<Edge *>::const_iterator it = inedges.begin(); it != inedges.end(); it++ )
    out << "(" << (*it)->toString() << ")";

  return out.str();

}

Edge::Edge(Node *sourcenode, Node *targetnode, EdgeParameters *parameters) : sourcenode(sourcenode), targetnode(targetnode), parameters(parameters){

  parameters->increaseReferences();

}

Edge::~Edge(){

  parameters->decreaseReferences();

  if (parameters->getReferences() == 0 )
    delete parameters;

}

void Edge::edgeDelete(Edge * edge){
  
  edge->decreaseReferences();
  if ( edge->getReferences() == 0 )
    delete edge;
  
}

string EdgeParameters::toString() const{

  ostringstream out;

  out << "co=" << cost << " ca=" << capacity << " f=" << flow << " cmp=" << component;

  return out.str();

}

string Edge::toString() const{

  ostringstream out;

  out << "(sn=" << getSourcenode()->getNumber() << "," << "tn=" << getTargetnode()->getNumber() << "," << parameters->toString() << ")";

  return out.str();

}

string Network::nonScientific(double value){

  ostringstream out;
  
  out.setf(ios::fixed);
  
  out << value;

  return MathExpression::skipTrailingZeros(out.str());

}

