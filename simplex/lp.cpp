#include <lp.hpp>

using namespace std;

LP::~LP(){

  delete[] cost; //free memory
  delete[] ar;   //free memory 

}

//make rhs positive


void LP::makepos_rhs(real* array, int cols, int rows) {
  for(int i=0;i<rows;i++)
    if(*(array+i*cols)<0)  //check if RHS is negative
      for(int j=0;j<cols;j++)     //if negative multiply row with -1.0
	*(array+i*cols+j) *= -1.0;
}

Dimensiontype LP::lookfor_unitvec(real* array,int cols,int rows,Vector* basis,Vector* index_basis,Orderset& order_basis) throw (ExceptionBase) {

  string thisMethod = "lookfor_unitvec()";

  try{

    Dimensiontype count=0;

    for(int i=0;i<rows;i++)
      (*basis)(i)=-1;       //...if it is already available

    for(int i=1;i<cols;i++) {    //check if mxm unit matrix is already available

      // we must stop if we found enough unit-vectors
      if ( count == basis->getDimension() )
	break;

      int test=0;     //test is signum-var to check for unitvector; count gives number of 
      int pos=-1;              // available unit vectors; pos gives pos_unit vector

      for ( int j = 0; j < rows; j++ ) {
	if ( array[i+j*cols] == 1 || array[i+j*cols] == 0 ) {

	  if ( array[i+j*cols] == 1 ){

	    if( pos==-1 ){
	      pos = j;
	      test = 1;
	    } 
	    else{
	      test = 0;
	      break;
	    }
	  }
	}
	else{
	  test = 0;
	  break;
	}
      }

      if ( test==1 ){
	
	// check for multiple occurences of the same unit-vector. This is the case if (*basis)(pos) != -1
	// since then an index was already inserted
	if ( (*basis)(pos) == -1 ){
	  
	  (*basis)(pos)=i;         //pos_unitvector is at i-th column
	  order_basis.insert(i);
	  (*index_basis)(count)=pos;
	  cout << "index basis[" << count << "] = " << (*index_basis)(count) << endl;
	  ++count;      
	  
	}
      }
    }

    return count;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::write_rhs_vec(real* array, int rows, int cols) throw (ExceptionBase) {

  string thisMethod = "write_rhs_vec()";

  try{

    Vector* rhs = new Vector(rows,"rhs");            //rhs vector  
    for(int i=0;i<rows;i++)        //rhs gets values
      (*rhs)(i) = array[i*cols];
    return rhs;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

//write matrix with coefficients of A (lp: A x = b;)
Matrix* LP::write_coeff_arr(real* array,int cols, int rows, Vector* basis, int unit_count,Vector* index_basis,Orderset& order_basis) throw (ExceptionBase) {

  string thisMethod = "write_coeff_arr()";

  try{

    string art_varname = "art_slack";
    //    ostringstream int_to_string;

    Matrix* coeff_ar = new Matrix(rows,cols+rows-unit_count,"coeff_ar");

    /*    for(int i=0;i<rows;i++) {
	  for(int j=0;j<cols;j++)
	  cout << *(array+i*(cols+1)+(j+1)) << "   ";
	  cout << endl;}*/

    for(int i=0;i<rows;i++)
      for(int j=0;j<cols;j++)
	(*coeff_ar)(i,j) = *(array+i*(cols+1)+(j+1));

     cout << " coeff_ar1 = " << endl;
     cout << coeff_ar->toString() << endl;

    int k=1;
    int lastcol=cols-1;
    int index_count = unit_count;

    for(int i=0;i<rows;i++) {

      if((*basis)(i)==-1) {

	(*basis)(i)=cols+k;
	(*coeff_ar)(i,lastcol+k)=1;
	ostringstream int_to_string;
	cout << "k = " << k << endl;
	int_to_string << k;
	art_varname = "art_slack";
	art_varname += int_to_string.str();
	cout << "art varname " << art_varname << endl;
	vars[art_varname]=cols+k;
	(*index_basis)(index_count)=i;
	order_basis.insert(cols+k);
	++index_count;
	k++;

      }
    }
    
     cout << " coeff_ar2 = " << endl;
     cout << coeff_ar->toString() << endl;

    

    return coeff_ar;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Matrix* LP::build_trans_matrix(Vector* index_basis, Dimensiontype rows) throw (ExceptionBase) {

  string thisMethod = "build_trans_matrix()";

  try{

    Matrix* trans_b = new Matrix(rows,rows,"trans_b"); //inverse basis matrix

    for(Dimensiontype i=0;i<rows;i++) {
      (*trans_b)(i,(Dimensiontype)((*index_basis)(i)))=1;
    }

    return trans_b; 

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::write_cost_vec_phase1(int cols,int added) throw (ExceptionBase) {

  string thisMethod = "write_cost_vec_phase1()";

  try{

    Vector* cost = new Vector(cols+added,"cost");

    for(int i=cols;i<cols+added;i++)
      (*cost)(i)=1;

    return cost;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

string LP::toString(Orderset & orderset){

  ostringstream output;

  for ( Orderset::iterator it = orderset.begin(); it != orderset.end(); it++ )
    output << *it << " ";

  return output.str();

}

Vector* LP::write_initial_basis_cost_vec_phase1(Orderset& order_basis, Vector* cost, Dimensiontype rows) throw (ExceptionBase) {

  string thisMethod = "write_initial_basis_cost_vec_phase1()";

  try{

    Vector* basis_cost = new Vector(rows,"basis_cost");

    /*    cout << "rows = " << rows << " order_basis->dim = " << order_basis->getDimension() << "   cost->dim = " << cost->getDimension() << endl;*/

    //    cout << "order_basis = " << endl;
    //    cout << order_basis->toString() << endl;

    for(Dimensiontype i=0;i<rows;i++) {
      //      cout << "order_basis(" << i << ") = " << (*order_basis)(i) << endl;
      (*basis_cost)(i) = (*cost)((Dimensiontype)(find_value_orderset(order_basis,i))-1);
    }
    return basis_cost;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::write_basis_cost_vec(Vector* basis_indices, Vector* cost, Dimensiontype rows) throw (ExceptionBase) {

  string thisMethod = "write_basis_cost_vec()";

  try{

    Vector* basis_cost = new Vector(rows,"basis_cost");

//     cout << "rows = " << rows << "   basis->dim = " << basis_indices->getDimension() << "   cost->dim = " << cost->getDimension() << endl;

    for(Dimensiontype i=0;i<rows;i++) {
//       cout << "basis_indices(" << i << ") = " << (*basis_indices)(i) << endl;
      (*basis_cost)(i) = (*cost)((Dimensiontype)((*basis_indices)(i))-1);
    }
    return basis_cost;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}
        
bool LP::check_if_basis_var(Vector* basis, int i) throw (ExceptionBase) {

  string thisMethod = "check_if_basis_var()";

  try{

    for(Dimensiontype k=0;k<basis->getDimension();k++) 
      if(i==(*basis)(k))
	return true;

    return false;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::calc_u(Matrix* inverse_b,Vector* coeff_col) throw (ExceptionBase) {

  string thisMethod = "calc_u()";

  try{

    Vector* u = new Vector(inverse_b->getRows(),"u");

    for(Dimensiontype i=0;i<u->getDimension();i++) {
      for(Dimensiontype j=0;j<inverse_b->getColumns();j++)
	(*u)(i) += (*inverse_b)(i,j) * (*coeff_col)(j);
    }

    return u; 

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

string LP::get_varname(int col) throw (ExceptionBase) {

  string thisMethod = "get_varname()";

  try{

    for(map<string,int>::iterator iter=vars.begin();iter!=vars.end();iter++)
      if(col==(*iter).second) {
	//	cout << "in fkt get_varname" << (*iter).first << endl;
	return (*iter).first;}

    throw Exception<LP>("col does not correspond to any varname!");

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}


void LP::adjustRowsByPivot(Matrix* inverse_b,Dimensiontype pivot,Vector* u) throw (ExceptionBase) {

  string thisMethod = "adjustRowsByPivot()";

  try{

    for(Dimensiontype row=0;row<inverse_b->getRows();row++) {
      if(row==pivot) 
	continue;
      for(Dimensiontype col=0;col<inverse_b->getColumns();col++)
	(*inverse_b)(row,col) -= (*inverse_b)(pivot,col) * (*u)(row);
    }

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Dimensiontype LP::give_random(set<Dimensiontype> & smallest_indices){
  
  long index = random()%smallest_indices.size()+1;
  
  long position = 1;
  set<Dimensiontype>::iterator it;

  cout << "choosing random rule" << endl;
  
  for ( it = smallest_indices.begin(); it != smallest_indices.end(); it++, position++ )
    if ( position == index )
      break;
  
  cout << "random: " << *it << endl;
  
  return *it;
  
}

Dimensiontype LP::give_lexic_smallest(set<Dimensiontype> & smallest_indices,Vector *u,Matrix *coeff_ar,Matrix *inverse_b) throw (ExceptionBase) {
  
  string thisMethod = "give_lexic_smallest()";

  Dimensiontype smallest;

  cout << "choosing lexicographic-smallest-rule" << endl;
  
  try{
    
    Matrix* b_1a = new Matrix(inverse_b,coeff_ar,"b_1a");
    set<VectorIndexPair, LTVecInd> vectors;

    for ( set<Dimensiontype>::iterator it = smallest_indices.begin(); it != smallest_indices.end(); it++ ){

      b_1a->divideRowByValue(*it,(*u)(*it));
      vectors.insert(VectorIndexPair(b_1a->getRow(*it),*it));
      //      cout << "inserting vectorindex:" << *it << endl;
      
    }

    smallest = (*vectors.begin()).second;

    for ( set<VectorIndexPair, LTVecInd>::iterator it = vectors.begin(); it != vectors.end(); it ++ )
      delete (*it).first;

    delete b_1a;

    cout << "smallest: " << smallest << endl;

    return smallest;
    
  
  } catch (ExceptionBase &e){
    
    throw e.prependMsg(thisMethod + ":");
    
  }
  
} 

Dimensiontype LP::give_blands_smallest(set<Dimensiontype> & smallest_indices, Vector *basis_indices) throw (Exception<LP>) {

  string thisMethod = "give_blands_smallest()";

  if ( smallest_indices.empty() == true )
    throw Exception<LP>(thisMethod + ": smallest_indices not empty!");

  Dimensiontype index = *smallest_indices.begin();
  Dimensiontype value = (*basis_indices)(index);

  cout << "choosing bland's rule" << endl;

  for ( set<Dimensiontype>::iterator it = smallest_indices.begin(); it != smallest_indices.end(); it++ ){

    if ( (*basis_indices)(*it) < value ){

      index = *it;
      value = (*basis_indices)(index);

    }

  }
    
  return index;

}

Solution LP::solve(Matrix* coeff_ar, Matrix* inverse_b,Vector* rhs_vec, Vector* basis,Vector* cost_vec,Vector * & basis_cost, Vector* basis_indices)
  throw (ExceptionBase) {

  string thisMethod = "solve()";

  try{

    int rows = inverse_b->getRows();
    int cols = coeff_ar->getColumns();
    Vector* p = new Vector(rows,"p");
    Vector* u;
    bool found_pos_u_el=false;
    real theta;
    real tmp_b_div_u;
    string enter_var,exit_var;
    Dimensiontype l;
    Vector* coeff_col;
    Vector* x_b = opt_basis_var_values(inverse_b,rhs_vec);

    for(;;) {

       cout << "basis cost c_b " << basis_cost->toString() << endl;

      /*cout << "inverse_b " << endl;
	cout << inverse_b->toString() << endl;*/

      p->clear();

      for(int j=0;j<rows;j++)   //calculate vector p=c_b * B^-1  
	for(int i=0;i<rows;i++) 
	  (*p)(j) += (*basis_cost)(i) * (*inverse_b)(i,j);

       cout << "p = " << p->toString() << endl;

      real red_cost;      //reduced costs
      bool found_neg_red_cost = false;    

       cout << "first basis von i" << endl;
       cout << basis->toString() << endl;

      found_neg_red_cost = false;
      for(int i=0;i<cols;i++) {


	// 	cout << "for i = " << i << endl;
	if(!(check_if_basis_var(basis,i+1))) {    //calc red_costs only from nonbasis vars

	  red_cost = (*cost_vec)(i);

	  for(int j=0;j<rows;j++) {
	    red_cost -= (*p)(j) * (*coeff_ar)(j,i);

	  }

	  if(red_cost < 0) {

	    found_neg_red_cost=true;
	    enter_var = get_varname(i+1);
	    cout << "entering: " << enter_var << endl;

	    coeff_col = coeff_ar->getColumn(i);
	    u = calc_u(inverse_b,coeff_col);
	    
 	    cout << "vector u = " << u->toString() << endl;
	    
	    delete coeff_col;
	    found_pos_u_el = false;
	    theta = -1;

	    set<Dimensiontype> smallest_indices;

	    for(Dimensiontype k=0;k<u->getDimension();k++) {

	      if((*u)(k)>0) {

		// 		cout << "u von k " << (*u)(k) << endl;
		found_pos_u_el=true;
		// 		cout << "rhs = " << endl;
		// 		cout << rhs_vec->toString() << endl;
		tmp_b_div_u = (*x_b)(k) / (*u)(k);

		if(theta==-1) {

		  theta = tmp_b_div_u;
		  l = k;
		  smallest_indices.insert(l);

		} 
		else if(tmp_b_div_u < theta) {

		  smallest_indices.clear();
		  theta = tmp_b_div_u;
		  l = k; 
		  smallest_indices.insert(l);
		} 

		else if(tmp_b_div_u == theta) {
		  smallest_indices.insert(k);
		}

	      }

	    }
	    cout << "theta " << theta << endl;

	    if(found_pos_u_el==false) {

	      Matrix* unbound_direc = new Matrix(u,"unbounded direction");
	      // Leads to memory-corruption
	      delete p;
	      delete u;

	      return Solution(SL_UNBOUNDED,unbound_direc);

	    }
	    
	    if(smallest_indices.size() > 1) {

		cout << "smalles.indices > 1 " << endl;
		
		if ( options.checkOption("lexic") )
		  l = give_lexic_smallest(smallest_indices,u,coeff_ar,inverse_b);
		else if ( options.checkOption("blands") )
		  l = give_blands_smallest(smallest_indices,basis_indices);
		else if ( options.checkOption("random") )
		  l = give_random(smallest_indices);

	    }
	    
	    exit_var = get_varname((Dimensiontype)((*basis_indices)(l)));
	    cout << "exiting: " << exit_var << endl;
	    cout << "l = " << l << endl;
	    (*basis)(l) = i+1;
	    (*basis_indices)(l) = i+1;
	    cout << "basis von i = " << basis->toString() << endl;
	    inverse_b->divideRowByValue(l,(*u)(l));
	    cout << "inverse by divide by row" << endl;
	    adjustRowsByPivot(inverse_b,l,u);

	    delete x_b;
	    x_b = opt_basis_var_values(inverse_b,rhs_vec);

 	    cout << "inverse_b from solve " << endl;
  	    cout << inverse_b->toString() << endl;

	    delete u;
	    break;
	    
	  }
	}
      }
      if(found_neg_red_cost==false) {

	delete p;

	return Solution(SL_OPTIMAL,inverse_b);

      }

      delete basis_cost;

      basis_cost = write_basis_cost_vec(basis_indices,cost_vec,rows);
    
    
    }

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}


Vector* LP::opt_basis_var_values(Matrix* inverse_b,Vector* rhs) throw (ExceptionBase) {

  string thisMethod = "opt_basis_var_values()";

  try{

    Vector* opt = new Vector(inverse_b->getRows(),"opt_basis_var_values()->opt");
    for(Dimensiontype i=0;i<opt->getDimension();i++)
      for(Dimensiontype j=0;j<opt->getDimension();j++)
	(*opt)(i) += (*inverse_b)(i,j) * (*rhs)(j);
    return opt;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

real LP::get_opt_cost(Vector* opt_vec, Vector* basis_indices, Vector* cost) throw (ExceptionBase) {

  string thisMethod = "get_opt_cost()";

  try{

    real opt_cost=0;

    cout << "optimaler Basisvektor:" << endl;
    cout << opt_vec->toString() << endl;
    cout << "Kostenvektor:" << endl;
    for (int i = 0; i<cost->getDimension(); i++)
      cout << (*cost)(i) << " ";
    cout << endl;
    cout << "Basiskostenvektor:" << endl;
    for (int i = 0; i<opt_vec->getDimension(); i++)
      cout << (*cost)((Dimensiontype)((*basis_indices)(i))-1) << " ";
    cout << endl;
    //     cout << opt_vec->toString() << endl;
    for(Dimensiontype i=0;i<opt_vec->getDimension();i++)
      opt_cost += (*opt_vec)(i) * (*cost)((Dimensiontype)((*basis_indices)(i))-1);

    cout << "optimale Kosten: " << opt_cost << endl;
    
    return opt_cost;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

string LP::solutiontypeToString(Solution solution){

  switch( solution.first ){

  case SL_INFEASIBLE:

    return "infeasible";
    break;

  case SL_OPTIMAL:
    
    return "optimal";
    break;

  case SL_UNBOUNDED:

    return "unbounded";
    break;

  default:

    return "UNKNOWN SOLUTION-TYPE!!!";
    break;
    
  }
  
}

void LP::print_opt_sol(Matrix* inverse_b,Vector* rhs,Vector* basis_indices, Dimensiontype cost_dim) throw (ExceptionBase) {

  string thisMethod = "print_opt_sol()";

  try{

    Vector *cost_vec = phase2_cost(cost_dim,this->cost);
    Vector* optimal_basis_var = opt_basis_var_values(inverse_b,rhs);

    cout << endl;
    cout << "---------------------" << endl << endl;
    cout << "OPTIMAL COST = "<< get_opt_cost(optimal_basis_var,basis_indices,cost_vec) << endl;

    for(Dimensiontype i=0;i<basis_indices->getDimension();i++) {
      cout << get_varname((int)((*basis_indices)(i))) << ": " << (*optimal_basis_var)(i) << endl;
    }

    delete optimal_basis_var;
    delete cost_vec;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::phase1_cost(Dimensiontype dim,int num_artificial_vars) throw (ExceptionBase) {

  string thisMethod = "phase1_cost()";

  try{

    //   cout << "dim = " << " " << dim << "  basis->getDimension() = " << basis->getDimension() << endl;

    Vector* cost_vec = new Vector(dim,"phase1_cost->cost_vec");

    for( Dimensiontype i = dim - num_artificial_vars; i < dim; i++ ){
      //      cout << "basis(" << i << ")=" << (*basis)(i) << endl;
      (*cost_vec)(i) = 1;
    }

    cout << thisMethod << ": costvector: " << cost_vec->toString() << endl;

    return cost_vec;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

bool LP::is_redundant_art(Matrix* coeff_ar,Matrix* inverse_b,Dimensiontype row, Dimensiontype col) throw (ExceptionBase) {

  string thisMethod="is_redundant_art()";
  
  try {

    Matrix b_1a(inverse_b,coeff_ar,"b_1a");
    for(Dimensiontype i=0;i<b_1a.getColumns();i++) {
      if(i==col)
	continue;
      if(b_1a(row,i)!=0)
	return false;
    }
    return true;
  }catch(ExceptionBase &e){
    throw e.prependMsg(thisMethod+":");
  }
}

void LP::drive_out_art_vars(Matrix* coeff_ar,Matrix* inverse_b,Vector* basis,int num_artificial_vars,Orderset& oset, Vector* basis_indices) throw (ExceptionBase) {

  string thisMethod="drive_out_art_vars()";
  int col;
  bool found=false;
  set<Dimensiontype> row_list_coeff_ar,col_list_coeff_ar,row_list_inverse_b,col_list_inverse_b,list_basis;

  try {

  for(int i=1;i<=num_artificial_vars;i++) {
    ostringstream int_to_str;
    int_to_str << "art_slack" << i;
    map<string,int>::iterator iter;
    if((iter=vars.find(int_to_str.str()))==vars.end()) {
      throw Exception<LP>(thisMethod+":"+int_to_str.str()+" not contained in hashmap");
    }
    col = (*iter).second;
    found = false;
    for(Dimensiontype i=0;i<inverse_b->getRows();i++) {
      if(col==(*basis)(i)){
	if(is_redundant_art(coeff_ar,inverse_b,i,(Dimensiontype)((*basis)(i)))==true) {
	  list_basis.insert(i);
	  row_list_coeff_ar.insert(i);
	  col_list_coeff_ar.insert(col-1);
	  row_list_inverse_b.insert(i);
	  Orderset::iterator setiter;
	  if((setiter=oset.find((Dimensiontype)((*basis)(i))))==oset.end()) 
	    throw Exception<LP>(thisMethod+": Element not contained in oset");
	  col_list_inverse_b.insert(*setiter);
	  found = true;

	  cout << "redundant: " << i << endl;

	  break;
	}
      }
    }
    if(found==false) {

      cout << "driving out nonbasic artificila variable: " << col - 1 << endl;
      col_list_coeff_ar.insert(col-1);

    }
  }
  coeff_ar->deleteRows(row_list_coeff_ar);
  coeff_ar->deleteColumns(col_list_coeff_ar);
  inverse_b->deleteRows(row_list_inverse_b);
  inverse_b->deleteColumns(col_list_inverse_b);
  basis->deleteEntries(list_basis);
 
  }catch(ExceptionBase &e){
    throw e.prependMsg(thisMethod+":");
  }
}

Solution LP::phase1(Matrix* coeff_ar,Matrix* inverse_b,Vector* rhs_vec,Vector* basis,int num_artificial_vars,Orderset& order_basis, Vector* basis_indices) throw (ExceptionBase) {

  string thisMethod = "phase1()";

  Vector* basis_cost; 

  try{

    cout << "entering phase I" << endl;

    Vector* cost_vec = phase1_cost(coeff_ar->getColumns(),num_artificial_vars);

    basis_cost = write_initial_basis_cost_vec_phase1(order_basis,cost_vec,basis->getDimension());
  
    Solution sol = solve(coeff_ar,inverse_b,rhs_vec,basis,cost_vec,basis_cost,basis_indices);

    cout << "phase I - cost vec "  << cost_vec->toString() << endl;
    
    cout << "basis von i " << basis->toString() << endl;

    cout << inverse_b->toString() << endl;
    cout << "phase I: solution-type " << solutiontypeToString(sol) << endl;

    if(sol.first!= SL_OPTIMAL) {

      cout << " test 1 " << endl;

      delete basis_cost;
      delete cost_vec;
      return Solution(SL_INFEASIBLE,0);}

    else {
 
      cout << "inverse_b: " << endl << inverse_b->toString() << endl;
      cout << "rhs: " << rhs_vec->toString() << endl;

      Vector* opt_basis_var=opt_basis_var_values(inverse_b,rhs_vec);

      real opt_cost = get_opt_cost(opt_basis_var,basis_indices,cost_vec);

      drive_out_art_vars(coeff_ar,inverse_b,basis,num_artificial_vars,order_basis,basis_indices);

      cout << "Phase I: OPTIMAL COST = " << opt_cost << endl;

      if( opt_cost != 0 ) {

	delete basis_cost;
	delete cost_vec;
	return Solution(SL_INFEASIBLE,0);
      } else {
 
	delete basis_cost;
	delete cost_vec;
	return Solution(SL_OPTIMAL,inverse_b);

      }
    }

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Vector* LP::phase2_cost(Dimensiontype dim, real* cost) throw (ExceptionBase) {

  string thisMethod = "phase2_cost()";

  try{

    Dimensiontype dim_old_cost = (Dimensiontype)num_vars;

    cout << "dim = " << dim << "\tdim_old_cost = " << dim_old_cost << endl;

    if(dim == dim_old_cost) {

      return new Vector(cost,dim_old_cost,"phase2_cost->cost");}
    else {
      Vector* new_cost = new Vector(dim,"phase2_cost->new_cost");
    
      for(Dimensiontype i=0;i<dim_old_cost;i++)
	(*new_cost)(i) = cost[i];

      return new_cost;
    }

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}


Solution LP::phase2(Matrix* coeff_ar,Matrix* inverse_b,Vector* rhs_vec,Vector* basis,Vector* basis_indices) throw (ExceptionBase) {

  string thisMethod = "phase2()";

  Vector* basis_cost; 

  cout << "entering phase II" << endl;

  try{

    Vector* cost_vec = phase2_cost(coeff_ar->getColumns(),this->cost);

    basis_cost = write_basis_cost_vec(basis_indices,cost_vec,basis->getDimension());

    Solution solution =  solve(coeff_ar,inverse_b,rhs_vec,basis,cost_vec,basis_cost,basis_indices);

    delete basis_cost;
    delete cost_vec;
    return solution;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}

Dimensiontype LP::find_value_orderset(Orderset& oset,Dimensiontype index) throw (Exception<LP>) {
  
  string thisMethod = "find_value_orderset()";
  
  int k=0;
  for(Orderset::iterator iter=oset.begin();iter!=oset.end();iter++,k++) {

    if(k==index)
      return *iter;
  }
  throw Exception<LP>(thisMethod+":index out of bounds");
}


Vector* LP::construct_basis_indices(Orderset& orderbasis) throw (ExceptionBase) {
  
  Vector* basis_indices = new Vector(orderbasis.size(),"basis_indices");
  Dimensiontype i=0;
  for(Orderset::iterator iter=orderbasis.begin();iter!=orderbasis.end();iter++,i++)
    (*basis_indices)(i) = (*iter);

  return basis_indices;
}

void LP::construct_order_basis(Orderset& order_basis, Vector *basis) throw (ExceptionBase){

  for (Dimensiontype i = 0; i < basis->getDimension(); i++ )
    order_basis.insert((Dimensiontype)(*basis)(i));

}

void LP::simplex() throw (ExceptionBase) {

  string thisMethod = "simplex()";

  cout << "entering " << thisMethod << endl;

  try{

    real* array=ar;
    int cols = vars.size()+1;
    int rows = num_cons;
    Vector* basis = new Vector(rows,"basis");  //basis[i] gives the columnposition of ith-unit vector ...  
    int unit_count, num_artificial_vars;                //number of already avail. unit vectors
    Vector* rhs_vec;
    Matrix* coeff_ar;
    Vector* index_basis = new Vector(rows,"index_basis");
    Vector* basis_indices;
    Orderset order_basis;
    Matrix* inverse_b;
    Solution opt_phase1;
    Solution opt_phase2;
    makepos_rhs(array,cols,rows);  // make rhs positive
    rhs_vec=write_rhs_vec(array,rows,cols);
    cout << "simplex()->write_rhs_vec()" << endl;
  
    unit_count = lookfor_unitvec(array,cols,rows,basis,index_basis,order_basis);
    cout << "simplex()->lookfor_unitvec()" << endl;
    cols--;           //rhs is not the first columns in arr anymore
    coeff_ar = write_coeff_arr(array,cols,rows,basis,unit_count,index_basis,order_basis);
    num_artificial_vars = rows - unit_count;
    cout << "simplex()->write_coeff_arr()" << endl;

    inverse_b=build_trans_matrix(index_basis,rows);
    cout << "simplex()->build_trans_matrix()" << endl;
    cout << "first inverse basis matrix " << endl;
    cout << inverse_b->toString() << endl;

    cout << thisMethod << ":" << "order_basis = " << toString(order_basis) << endl;

    basis_indices = construct_basis_indices(order_basis);

    if(unit_count!=rows) {  //we have not enough unit vectors in our matrix

      cout << "simplex(): choosing phase I" << endl;
      opt_phase1 = phase1(coeff_ar,inverse_b,rhs_vec,basis,num_artificial_vars,order_basis,basis_indices);
      cout << "exiting phase 1 " << endl;

      if(opt_phase1.first != SL_OPTIMAL) {

	//       return opt_phase1;
	cout << "#LP not feasible!!!" << endl;

	goto exit;
      }
    }

    // buggy
//     delete basis_indices;

//     order_basis.clear();

//     construct_order_basis(order_basis,basis);

//     basis_indices = construct_basis_indices(order_basis);

    cout << "basis_indices: Größe: " << basis_indices->getDimension() << " vector: " << basis_indices->toString() << endl;

    opt_phase2 = phase2(coeff_ar,inverse_b,rhs_vec,basis,basis_indices);

    if(opt_phase2.first == SL_UNBOUNDED) {
      cout << "LP is unbounded" << endl;
    }
    else if(opt_phase2.first == SL_INFEASIBLE) {
      cout << "LP is infeasible" << endl;
    }
    else {
      print_opt_sol(inverse_b,rhs_vec,basis_indices,coeff_ar->getColumns());
    }
  exit:
   
    delete rhs_vec;
    delete basis;
    delete coeff_ar;
    delete inverse_b;
    delete index_basis; 
    delete basis_indices;

  } catch (ExceptionBase &e){

    throw e.prependMsg(thisMethod + ":");

  }

}
