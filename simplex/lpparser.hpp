#ifndef LPPARSER_HPP
#define LPPARSER_HPP

#include <iostream>
#include <map>
#include <types.hpp>
#include <cmdlparser.hpp>
#include <lp.hpp>

class LPParser{
  
protected:

  std::string lpath;
  
  //int phase_one(real* array,real* cost,int rows,int cols,map<string,int> vars_);
  int num_cons;

  real* ar;
  real* cost;

  cmdl::CmdlParser & options;

public:

  LPParser(std::string lpath, cmdl::CmdlParser & options) : lpath(lpath), num_cons(0),ar(0),cost(0), options(options){}

  LP * parse();

protected:

  int is_string(std::string str);  //return 1 if in str is an alphanumeric sign
  real* read_array(std::string path,int num_cons_,std::map<std::string,int>& vars_);
  real* read_cost_vect(std::string path, std::map<std::string,int>& vars_);
  int read_varnames(std::string path, std::map<std::string,int> &vars);
  
};


#endif
