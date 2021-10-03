#include "matrix.hpp"

using namespace std;


Vector::Vector(Dimensiontype dimension, string identifier) : dimension(dimension), identifier(identifier), array(0){

  alloc();

}

// Vector::Vector(Vector &vector){

//   dimension = vector.dimension;
//   alloc();

//   for ( Dimensiontype i = 0; i < dimension; i++ )
//     array[i] = vector.array[i];
  
// }

Vector::Vector(real* array,Dimensiontype dim, std::string identifier) :
  dimension(dim),identifier(identifier), array(0) {

  alloc();
  for(Dimensiontype i=0;i<dim;i++)
    this->array[i] = array[i];
}


Vector::~Vector(){

  //   cout << "~Vector(): attempting to delete " << identifier << "     ";
  free(array);
  array = 0;
  //   cout << "~Vector(): " << identifier << ":" << this << " deleted" << endl;

}

void Vector::alloc(){

  array = (real *)calloc(dimension,sizeof(real));

}

void Vector::clear(){

  for ( Dimensiontype i = 0; i < getDimension(); i++)
    (*this)(i) = 0;

}

void Vector::deleteEntries(set<Dimensiontype> entryset) throw (ExceptionBase){

  string thisMethod = "deleteEntries()";

  if ( entryset.empty() == true )
    return;

  try{
    
    for ( set<Dimensiontype>::iterator it = entryset.begin(); it != entryset.end(); it++ )
      assertInBounds(*it);
    
    Dimensiontype oldentries = getDimension();
    real * oldarray = array;
    
    this->dimension = oldentries - entryset.size();
    
    alloc();
    
    for ( Dimensiontype newentry = 0, oldentry = 0; oldentry < oldentries; oldentry++ ){
      
      if ( entryset.find(oldentry) != entryset.end() )
	continue;

      (*this)(newentry) = oldarray[oldentry];
      
      newentry++;
      
    }
    
    delete oldarray;
    
  } catch (ExceptionBase &e){
    
    throw e.prependMsg(thisMethod + ":");
    
  }
  
  
}

void Vector::assertInBounds(Dimensiontype index) throw (Exception<Vector>){

  if ( index < 0 )
    throw Exception<Vector>(identifier + string(": below lower bound!"));
  else if ( index >= dimension )
    throw Exception<Vector>(identifier + string(": beyond upper bound!"));

}

real & Vector::operator()(Dimensiontype index) throw(Exception<Vector>){

  assertInBounds(index);

  return array[index];

}

int Vector::compare(Vector &vector) throw (Exception<Vector>){
  
  string thisMethod = "compare()";
  
  if ( this->getDimension() != vector.getDimension() )
    throw Exception<Vector>(thisMethod + ": dimensions not equal!");
  
  for ( Dimensiontype i = 0; i < this->getDimension(); i++ ){
    
    if ( (*this)(i) > vector(i) )
      return 1;
    else if ( (*this)(i) < vector(i) )
      return -1;
    
  }

  return 0;
  
}

string Vector::toString(){
  
  ostringstream output;

  for ( Dimensiontype i = 0; i < dimension; i++)
    output << array[i] << " ";
  
  return output.str();

}

Matrix::Matrix(Dimensiontype rows, Dimensiontype columns, string identifier) : rows(rows), columns(columns), identifier(identifier), array(0){

  alloc();

}


Matrix::Matrix(Vector* vector, string identifier) {

  rows = vector->getDimension();
  columns = 1;
  alloc();

  for(Dimensiontype i=0;i<rows;i++) 
    (*this)(i,0) = (*vector)(i);
}

Matrix::Matrix(Matrix *a, Matrix *b, string identifier) throw (Exception<Matrix>){

  string thisMethod = "Matrix(Matrix *a, Matrix *b, string identifier)";

  if ( a->getColumns() != b->getRows() )
    throw Exception<Matrix>(thisMethod +": columns of a not equal to rows of b");

  rows = a->getRows();
  columns = b->getColumns();
  this->identifier = identifier;

  alloc();

  for ( Dimensiontype i = 0; i < rows; i++ )
    for ( Dimensiontype j =0; j < columns; j++ )
      for ( Dimensiontype k = 0; k < a->getColumns(); k++ )
	(*this)(i,j) += (*a)(i,k) * (*b)(k,j);

}


Matrix::~Matrix(){

  free(array);
  array = 0;

}

void Matrix::alloc(){

  array = (real *)calloc(rows * columns,sizeof(real));

}

Vector* Matrix::getColumn(Dimensiontype col) throw (ExceptionBase) {

  Vector* columnvector;

  assertInColumnBounds(col);

  ostringstream name;
  name << "column" << col;
  
  columnvector =  new Vector(getRows(),name.str());

  for( Dimensiontype row = 0; row < getRows(); row++ ) 
    (*columnvector)(row) = (*this)(row,col);

  return columnvector;

}

Vector* Matrix::getRow(Dimensiontype row) throw (ExceptionBase){

  Vector *rowvector;

  assertInRowBounds(row);

  ostringstream name;
  name << "row" << row;

  rowvector = new Vector(getColumns(),name.str());
  
  for ( Dimensiontype column = 0; column < getColumns(); column++ )
    (*rowvector)(column) = (*this)(row,column);

  return rowvector;

}

void Matrix::assertInRowBounds(Dimensiontype row) throw (Exception<Matrix>) {

  if ( row < 0 )
    throw Exception<Matrix>(identifier + string(": below lower row-bound!"));
  else if ( row >= rows )
    throw Exception<Matrix>(identifier + string(": beyond upper row-bound!"));

}

void Matrix::assertInColumnBounds(Dimensiontype column) throw (Exception<Matrix>) {

  if ( column < 0 )
    throw Exception<Matrix>(identifier + string(": below lower column-bound!"));
  else if ( column >= columns )
    throw Exception<Matrix>(identifier + string(": beyond upper column-bound!"));

}

void Matrix::assertInBounds(Dimensiontype row, Dimensiontype column) throw (Exception<Matrix>){

  assertInRowBounds(row);
  assertInColumnBounds(column);

}

void Matrix::deleteRows(set<Dimensiontype> & rowset) throw (ExceptionBase){

  string thisMethod = "deleteRows()";

  if ( rowset.empty() == true )
    return;

  try{
    
    for ( set<Dimensiontype>::iterator it = rowset.begin(); it != rowset.end(); it++ )
      assertInRowBounds(*it);
    
    Dimensiontype oldrows = getRows();
    real * oldarray = array;
    
    this->rows = oldrows - rowset.size();
    
    alloc();
    
    for ( Dimensiontype newrow = 0, oldrow = 0; oldrow < oldrows; oldrow++ ){
      
      if ( rowset.find(oldrow) != rowset.end() )
	continue;
      
      for ( Dimensiontype column = 0; column < getColumns(); column++ )
	(*this)(newrow,column) = oldarray[oldrow*getColumns()+column];
      
      newrow++;
      
    }
    
    delete oldarray;
    
  } catch (ExceptionBase &e){
    
    throw e.prependMsg(thisMethod + ":");
    
  }
  
  
}

void Matrix::deleteColumns(set<Dimensiontype> & columnset) throw (ExceptionBase){

  string thisMethod = "deleteColumns()";

  if ( columnset.empty() == true )
    return;

  try{
    
    for ( set<Dimensiontype>::iterator it = columnset.begin(); it != columnset.end(); it++ )
      assertInColumnBounds(*it);
    
    Dimensiontype oldcolumns = getColumns();
    real * oldarray = array;
    
    this->columns = oldcolumns - columnset.size();
    
    alloc();
    
    for ( Dimensiontype newcolumn = 0, oldcolumn = 0; oldcolumn < oldcolumns; oldcolumn++ ){
      
      if ( columnset.find(oldcolumn) != columnset.end() )
	continue;
      
      for ( Dimensiontype row = 0; row < getRows(); row++ )
	(*this)(row,newcolumn) = oldarray[row*oldcolumns+oldcolumn];
      
      newcolumn++;
      
    }
    
    delete oldarray;
    
  } catch (ExceptionBase &e){
    
    throw e.prependMsg(thisMethod + ":");
    
  }
  
  
}

void Matrix::divideRowByValue(Dimensiontype row, real value) throw (Exception<Matrix>) {
  
  assertInRowBounds(row);
  for(Dimensiontype i=0;i<getColumns();i++)
    (*this)(row,i) /= value;
}


real & Matrix::operator()(Dimensiontype row, Dimensiontype column) throw(Exception<Matrix>){

  assertInBounds(row,column);

  return array[row*columns+column];

}

string Matrix::toString(){

  ostringstream output;

  for ( Dimensiontype row = 0; row < rows; row++){

    for ( Dimensiontype column = 0; column < columns; column++ )
      output << array[row*columns+column] << "\t";
    output << endl;

  }

  return output.str();

}
