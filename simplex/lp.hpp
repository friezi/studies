#ifndef LP_HPP
#define LP_HPP

#include <utility>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <list>
#include <set>
#include <sstream>
#include <exception.hpp>
#include <cmdlparser.hpp>
#include <types.hpp>
#include <matrix.hpp>

#define SL_INFEASIBLE 0
#define SL_OPTIMAL 1
#define SL_UNBOUNDED 2

typedef std::pair<unsigned int,Matrix *> Solution;
typedef std::pair<Vector *,Dimensiontype> VectorIndexPair;
typedef std::set<Dimensiontype> Orderset;

class LP{

protected:
  
  //int phase_one(real* array,real* cost,int rows,int cols,map<string,int> vars_);
  int num_cons;
  Dimensiontype num_vars;

  real* ar;
  real* cost;

  cmdl::CmdlParser & options;
  
  std::map<std::string,int> vars;   //extern

public:

  LP(cmdl::CmdlParser & options) : num_cons(0),num_vars(0),ar(0),cost(0),options(options){}

  ~LP();

  std::map<std::string,int> & getVars(){ return vars; }

  void setNumCons(int num_cons){ this->num_cons = num_cons; }
  void setNumVars(Dimensiontype num_vars){ this->num_vars = num_vars; }

  void setMatrix(real *matrix){ this->ar = matrix; }
  void setCost(real *cost){ this->cost = cost; }
  void simplex() throw (ExceptionBase);

  real getOptimalCost();

  static std::string solutiontypeToString(Solution solution);


protected:
  
  struct LTVecInd{
    
  public:
    
    bool operator()(std::pair<Vector *,Dimensiontype> p1, std::pair<Vector *,Dimensiontype> p2){
      return ( p1.first->compare(*(p2.first)) < 0 );
    }
    
  };

  void makepos_rhs(real* array, int cols, int rows);
  Dimensiontype lookfor_unitvec(real* array,int cols,int rows,Vector* basis,Vector* index_basis,Orderset& order_basis) throw (ExceptionBase);
  Vector* write_rhs_vec(real* array, int rows, int cols) throw (ExceptionBase);
  Matrix* write_coeff_arr(real* array,int cols, int rows, Vector* basis, int unit_count,Vector* index_basis,Orderset& order_basis) throw (ExceptionBase);
  Matrix* build_trans_matrix(Vector* index_basis, Dimensiontype rows) throw (ExceptionBase);
  Vector* write_cost_vec_phase1(int cols,int added) throw (ExceptionBase);
  Vector* write_initial_basis_cost_vec_phase1(Orderset& order_basis, Vector* cost, Dimensiontype rows) throw (ExceptionBase);
  Vector* write_basis_cost_vec(Vector *basis, Vector* cost, Dimensiontype rows) throw (ExceptionBase);
  bool check_if_basis_var(Vector* basis, int i) throw (ExceptionBase);
  Vector* calc_u(Matrix* inverse_b,Vector* coeff_col) throw (ExceptionBase);
  std::string get_varname(int col) throw (ExceptionBase);
  Solution solve(Matrix* coeff_ar, Matrix* inverse_b,Vector* rhs_vec, Vector* basis,Vector* cost,Vector * & basis_cost, Vector* basis_indices) throw (ExceptionBase);
  Solution phase1(Matrix* coeff_ar,Matrix* inverse_b,Vector* rhs_vec,Vector* basis,int num_artificial_vars,Orderset& order_basis,Vector* basis_indices) throw (ExceptionBase);
  void adjustRowsByPivot(Matrix* inverse_b,Dimensiontype pivot,Vector* u) throw (ExceptionBase);
  Vector* phase1_cost(Dimensiontype dim,int num_artificial_vars) throw (ExceptionBase);
  Vector* opt_basis_var_values(Matrix* inverse_b,Vector* rhs) throw (ExceptionBase);
  real get_opt_cost(Vector* opt_vec, Vector* basis_indices, Vector* cost) throw (ExceptionBase);
  Solution phase2(Matrix* coeff_ar,Matrix* inverse_b,Vector* rhs_vec,Vector* basis,Vector* basis_indices) throw (ExceptionBase);
  Vector* phase2_cost(Dimensiontype dim, real* cost) throw (ExceptionBase);
  void print_opt_sol(Matrix* inverse_b,Vector* rhs,Vector* basis_indices, Dimensiontype cost_dim) throw (ExceptionBase);
  Dimensiontype give_lexic_smallest(std::set<Dimensiontype> & smallest_indices,Vector *u,Matrix *coeff_ar,Matrix *inverse_b) throw (ExceptionBase);
  Dimensiontype give_random(std::set<Dimensiontype> & smallest_indices);
  Dimensiontype give_blands_smallest(std::set<Dimensiontype> & smallest_indices, Vector *basis_indices) throw (Exception<LP>);
  Dimensiontype find_value_orderset(Orderset& oset,Dimensiontype index) throw (Exception<LP>);
  std::string toString(Orderset & orderset);
  void drive_out_art_vars(Matrix* coeff_ar,Matrix* inverse_b,Vector* basis,int num_artificial_vars,Orderset& oset, Vector* basis_indices) throw (ExceptionBase);
  bool is_redundant_art(Matrix* coeff_ar,Matrix* inverse_b,Dimensiontype row, Dimensiontype col) throw (ExceptionBase);
  Vector* construct_basis_indices(Orderset& orderbasis) throw (ExceptionBase);
  void construct_order_basis(Orderset& order_basis, Vector *basis) throw (ExceptionBase);

};


#endif
