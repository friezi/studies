#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <sstream>
#include <set>
#include <exception.hpp>
#include "types.hpp"

class Vector{

protected:

  Dimensiontype dimension;
  
  std::string identifier;

  real* array;

  // allocates memory for the array
  void alloc();

  void assertInBounds(Dimensiontype index) throw (Exception<Vector>);

public:

  Vector(Dimensiontype dimension, std::string identifier);

  Vector(real* array,Dimensiontype dim, std::string identifier);

//   // Copyconstructor
//   Vector(Vector &vector);
  ~Vector();

  void clear();

  Dimensiontype getDimension(){ return dimension;}

  real & operator()(Dimensiontype index) throw (Exception<Vector>);

  int compare(Vector &vector) throw (Exception<Vector>);

  void deleteEntries(std::set<Dimensiontype> entryset) throw (ExceptionBase);

  std::string toString();

};

class Matrix{

protected:

  Dimensiontype rows;
  Dimensiontype columns;
  
  std::string identifier;

  real * array;

  // allocates memory for the array
  void alloc();

  void assertInBounds(Dimensiontype row, Dimensiontype column) throw(Exception<Matrix>);

public:

  Matrix(Dimensiontype rows, Dimensiontype columns, std::string identifier);

  Matrix(Vector* vector, std::string identifier);

  // new matrix by matrix-multiplication
  Matrix(Matrix *a, Matrix *b, std::string identifier) throw (Exception<Matrix>);

//   // Copyconstructor
//   Matrix(Matrix &vector);
  ~Matrix();

  real & operator()(Dimensiontype row, Dimensiontype column) throw(Exception<Matrix>);

  Vector* getColumn(Dimensiontype col) throw (ExceptionBase);
  Vector* getRow(Dimensiontype row) throw (ExceptionBase);
  void deleteRows(std::set<Dimensiontype> & rowset) throw (ExceptionBase);
  void deleteColumns(std::set<Dimensiontype> & columnset) throw (ExceptionBase);
  
  void assertInRowBounds(Dimensiontype row) throw (Exception<Matrix>);

  void assertInColumnBounds(Dimensiontype column) throw (Exception<Matrix>);

  void divideRowByValue(Dimensiontype row, real value) throw (Exception<Matrix>);

  Dimensiontype getRows(){ return rows;}
  Dimensiontype getColumns() throw (ExceptionBase){ return columns;}

  std::string toString();

};

#endif  
