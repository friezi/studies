#include <lpparser.hpp>

using namespace std;

LP * LPParser::parse(){

  LP * lp = new LP(options);

  map<string,int> &vars = lp->getVars();

  num_cons = read_varnames(lpath,vars);  //number of constraints we will get

  lp->setNumCons(num_cons);

  for(map<string,int>::iterator iter=vars.begin();iter!=vars.end();iter++)
    cout << (*iter).first << "--" << (*iter).second << "  ";
  cout << endl;
  cout << "Anzahl variables " << vars.size() << endl;
  cout << "Anzahl constraints " << num_cons << endl; 

  cost = read_cost_vect(lpath,vars);

  lp->setCost(cost);

  for(unsigned int i=0;i<vars.size();i++)
    cout << *(cost+i) << " " ;
  cout << endl << endl; 
  
  ar = read_array(lpath,num_cons,vars);

  lp->setMatrix(ar);

  lp->setNumVars(vars.size());

  for(int j=0;j<num_cons;j++) {
    for(unsigned int i=1;i<(vars.size()+1);i++) {
      cout << *(ar+i+j*(vars.size()+1)) << "   "; }
    cout << " = " << *(ar+j*(vars.size()+1)) << endl;
  }

  return lp;

}

int LPParser::is_string(string str) {  //return 1 if in str is an alphanumeric sign
  int k=0;
  for(unsigned int i=0;i<str.length();i++)
    if(isalpha(str[i])) {
      k=1;
      break;
    }
  return k;
}

real* LPParser::read_array(string path,int num_cons_,map<string,int>& vars_) {

  ifstream file; //stream for file
  string line; //line corresponds to a read line in given file
  file.open(&path[0]);
  while(getline(file,line) && line.find("Subject")==string::npos) {
  } //go to Subject to line
  real* array = new real[(vars_.size()+1)*num_cons_];
  for(unsigned int i=0;i<(vars_.size()+1)*num_cons_;i++)
    *(array+i) = 0.0;
  int slack=0,cur_cons=0;  //cur_cons - current constraintline
  int pos;   //position of variablenames in map

  while(getline(file,line) && line.find("Bounds")==string::npos && line.find("End")==string::npos) {
    
    if(line.find("\\")==string::npos) {
      istringstream ist(line);
      string token;
      int sig=1,sig_endcons=0;
      real zahl=1.0;
      while(ist >> token) {
	if(token.find(":")==string::npos) {   //we do not take constraintname into consideration
	  if(token == "+") {  //go further
	  }
	  else if(token =="-") { //set sig to -1
	    sig = -1; }
	  else if(token.find("<")!=string::npos || token.find(">")!=string::npos) { 
	    ++slack;                                 //we have arrived at <= or >=
	    string slack_token;
	    stringstream tmp;
	    tmp << slack;
	    slack_token="slack"+tmp.str();
	    sig_endcons=1;    //we are almost at the end of constraint/only RHS is left
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == slack_token) {  //look for slack_token in map
		pos = (*iter).second;        //get number of slack_token
	      }
	    }
	    if(token.find("<")!=string::npos) {
	      *(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	    else { /*(token.find(">")!=string::npos)*/ 
	      *(array+cur_cons*(vars_.size()+1)+pos) = -1.0;}
	  }
	  else if(token.find("=")!=string::npos) {  //we are in constraint LHS = RHS
	    sig_endcons=1; }
	  else if(is_string(token)!=1 && sig_endcons==0) { //token is a coeff of LHS
	    stringstream tmp_str;
	    tmp_str << token;
	    tmp_str >> zahl; }
	  else if(is_string(token)==1) {  //token is a variablename of LHS
	    zahl *= sig;          //coefficient = coeff * signum
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == token) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = zahl; }
	      else if((*iter).first ==("minus_"+token)) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1*zahl; }
	    }
	    zahl = 1.0;  //set zahl back
	    sig = 1;     //set signum back
	  }
	  else if(is_string(token)!=1 && sig_endcons==1) { //we have arrived at RHS
	    stringstream tmp_rhs;
	    real rhs;
	    tmp_rhs << token;
	    tmp_rhs >> rhs;
	    *(array+cur_cons*(vars_.size()+1)) = rhs;
	    sig_endcons = 0;  //set back sig_endcons
	    ++cur_cons;      //next line will be a new constraint
	  }
	  else {
	    cout << "something went wrong" << endl;}
	}
      }
    }
  }
  if(line.find("Bounds")!=string::npos) {
    while(getline(file,line) && line.find("End")==string::npos) { //run through Bounds
      istringstream ist(line);
      string token;
      real zahl=1.0;
      int count=0;
      string bound[5];
      string str_tmp;
      string slack_token;
      int w;
      while(ist >> token) {
	bound[count]=token;
	count++; }
      if(count==3) { //bound shorter version
	if(bound[1] == "=") { //bound looks like variable = zahl or zahl = variable
	  if(is_string(bound[0])==1)  //bound looks like var = zahl
	    w=0;
	  else if(is_string(bound[2])==1)
	    w=2;
	  for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	    if((*iter).first==bound[w]) {
	      pos = (*iter).second;
	      string help;
	      stringstream tmp_str;
	      if(w==0) 
		help = bound[2];
	      else 
		help = bound[0];
	      tmp_str << help;
	      tmp_str >> zahl;
	      *(array+cur_cons*(vars_.size()+1)) = zahl;
	      *(array+cur_cons*(vars_.size()+1)+pos) = 1;
	    }}
	  ++cur_cons;}
	else if(line.find("INF")!=string::npos) { //bound is -INF<=var or var <= INF
	} //variable is unbounded and  var+ , var- is already written in map
	else {    //var <= zahl or zahl <= var
	  if(is_string(bound[0])==1) {  //bound looks like var <=zahl
	    ++slack;
	    stringstream tmp;
	    tmp << slack;
	    slack_token="slack"+tmp.str();
	    stringstream tmp_str;
	    str_tmp = bound[2];  
	    tmp_str << str_tmp;
	    tmp_str >> zahl; 
	    string token_neg="minus_"+bound[0];
	    //	    cout << token_neg << endl;
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == bound[0]) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	      if((*iter).first == slack_token) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	      if((*iter).first == token_neg) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }}
	    *(array+cur_cons*(vars_.size()+1)) = zahl;	    
	    ++cur_cons;}
	  else if(is_string(bound[2])==1) { //bound looks like zahl <= var
	    ++slack;
	    stringstream tmp;
	    tmp << slack;
	    slack_token="slack"+tmp.str();
	    stringstream tmp_str;
	    str_tmp = bound[0];  
	    tmp_str << str_tmp;
	    tmp_str >> zahl; 
	    if(zahl != 0) {
	      for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
		if((*iter).first == bound[2]) {
		  pos = (*iter).second;
		  *(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
		if((*iter).first == slack_token) {
		  pos = (*iter).second;
		  *(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }
	      }
	      if(zahl<0) {
		string token_neg = "minus_"+bound[2];
		for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
		  if((*iter).first == token_neg) {
		    pos = (*iter).second;
		    *(array+cur_cons*(vars_.size()+1)+pos) = -1.0;}
		} }
	      *(array+cur_cons*(vars_.size()+1)) = zahl;	    
	      ++cur_cons;}}}}
	       

      else if(count==5) { //bound looks like bound1 <= var <= bound2
	if(bound[0].find("-INF")!=string::npos) { //bound looks like -INF <= var <= bound2
	  if(bound[4].find("+INF")!=string::npos) { //bound looks like -INF <= var <= +INF
	  } //variable is unbounded; is already written in map
	  else {  //bound looks like -INF <= var <= zahl
	    ++slack;
	    stringstream tmp;
	    tmp << slack;
	    slack_token="slack"+tmp.str();
	    stringstream tmp_str;
	    str_tmp = bound[4];
	    tmp_str << str_tmp;
	    tmp_str >> zahl;
	    *(array+cur_cons*(vars_.size()+1)) = zahl;  //write rhs in matrix
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == slack_token) {  //write slack coeff in matrix
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = 1.0; } 
	      if((*iter).first == bound[2]) {  //write var coefficient in matrix
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	      string neg = "minus_"+bound[2];
	      if((*iter).first == neg) { //write var- coefficient in matrix
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }
	    }
	    ++cur_cons; }}
	else if(bound[4].find("+INF")!=string::npos) { //bound looks like zahl <= var <= +INF
	  stringstream tmp_str;
	  str_tmp = bound[0];
	  tmp_str << str_tmp;
	  tmp_str >> zahl;
	  if(zahl!=0) { //otherwise bound looks like 0<=var<=+INF and there is nothing to do
	    stringstream tmp;
	    ++slack;
	    tmp << slack;
	    slack_token="slack"+tmp.str();
	    *(array+cur_cons*(vars_.size()+1)) = zahl;
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == slack_token) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }
	      if((*iter).first == bound[2]) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	      if(zahl < 0) {
		string neg = "minus"+bound[2];
		if((*iter).first == neg) {
		  pos = (*iter).second;
		  *(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }}
	    }
	    ++cur_cons; 
	  }}
	else if(line.find("INF")==string::npos) { //bound looks like zahl1<=var<=zahl2
	  int zahl1,zahl2;
	  stringstream tmp_str1,tmp_str2;
	  tmp_str1 << bound[0];
	  tmp_str1 >> zahl1;
	  tmp_str2 << bound[4];
	  tmp_str2 >> zahl2;
	  stringstream tmp;
	  ++slack;                                 
	  tmp << slack;
	  slack_token="slack"+tmp.str();
	  for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	    if((*iter).first == slack_token) {
	      pos = (*iter).second;
	      *(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }
	    if((*iter).first == bound[2]) {
	      pos = (*iter).second;
	      *(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	    if(zahl1<0) {
	      string neg = "minus_"+bound[2];
	      if((*iter).first == neg) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1.0; }}
	  }
	  *(array+cur_cons*(vars_.size()+1)) = zahl1;
	  ++cur_cons;
	  stringstream tmp2;
	  ++slack;                              
	  tmp2 << slack;
	  slack_token="slack"+tmp.str();
	  for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	    if((*iter).first == slack_token) {
	      pos = (*iter).second;
	      *(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	    if((*iter).first == slack_token) {
	      pos = (*iter).second;
	      *(array+cur_cons*(vars_.size()+1)+pos) = 1.0; }
	    if(zahl1 < 0) {
	      string neg = "minus_"+bound[2];
	      if((*iter).first == neg) {
		pos = (*iter).second;
		*(array+cur_cons*(vars_.size()+1)+pos) = -1.0;}}
	  }
	  *(array+cur_cons*(vars_.size()+1)) = zahl2;
	  ++cur_cons; 
	}
      }
    }
  }	
  if((cur_cons)!=num_cons_) 
    cout << "number of constraint lines not equal" << endl;
  //  cout << "current cons" << cur_cons << endl;
  //  cout << "num_cons_" << num_cons_ << endl;
  return array;
}

real* LPParser::read_cost_vect(string path, map<string,int>& vars_) {
  ifstream file; //stream for file
  string line;    //line corresponds to a read line in given file
  file.open(&path[0]);  //open file with given path
  getline(file,line);
  while((line.find("Min")==string::npos) && (line.find("Max")==string::npos)) {
    getline(file,line); } //if at beginning of file are some blank lines 
  int sig_max=1;
  if(line.find("Max") != string::npos) 
    sig_max = -1;
  real* cost_vect = new real[vars_.size()];
  for(unsigned int i=0;i<vars_.size();i++)
    *(cost_vect+i) = 0;
  while(getline(file,line) && line.find("Subject")==string::npos) {
    istringstream ist(line);
    string token;
    int sig=1;
    real zahl=1.0;
    int pos;   //position of variablenames in map
    while(ist >> token){
      if(token.find(":")==string::npos) { //don't take obj fct name into consideration
	if(token == "+") {  //if token = + do nothing
	}
	else if(token == "-")
	  sig = -1;
	else if(is_string(token) != 1) {
	  stringstream tmp_str;
	  tmp_str << token;
	  tmp_str >> zahl;
	}
	else if(is_string(token) == 1) { //we have got a variablename
	  int test;
	  for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	    //   	    cout << token << " " << (*iter).first << endl;
	    if((*iter).first == token) {
	      pos = (*iter).second; 
	      test = 1; } 
	  }
	  if(test != 1) {
	    cout << "wrong " << token << endl;
	    cout << "error with constraint variable name" << endl; }
	  else {
	    *(cost_vect+pos-1) = (zahl*sig);
	    string token_neg = "minus_"+token;
	    for(map<string,int>::iterator iter=vars_.begin();iter!=vars_.end();iter++) {
	      if((*iter).first == token_neg) {
		pos = (*iter).second;
		*(cost_vect+pos-1) = -1*(zahl*sig);
	      } 
	    }
	    zahl = 1;  // set zahl back to 1 for next run
	    sig = 1;   //set signum back to 1 for next run
	  }
	}
      }
    }
  }
  file.close();
  return cost_vect;  /*&cost_vect[0]*/
}

int LPParser::read_varnames(string path, map<string,int> &vars) {

  ifstream file;  //stream for file
  string line;     //line corresponds to a read line in given file
  file.open(&path[0]);  //open file with given path
  if (file.bad()) {    //if problems occur during file opening
    cerr << "unable to open file" << endl;
    exit (8); }
  getline(file,line);  //read first line in given file
  while(line.find("Subject") == string::npos) {
    getline(file,line); }  //go to "Subject to" line
  int k=1,m=0,slack=0;    //k is for variable number; m will give number of constraints
  while(getline(file,line) && (line.find("Bounds")==string::npos) && (line.find("End")==string::npos))  {
    char c;
    int zahl=1;
    string token = "";
    for(unsigned int i=0;i<line.length();i++) {
      if((c=line.at(i))=='/') {
	break;}  //go to|read next constraint line
      else if((c != ' ') && (c!='+') && (c!='-')) {
	if(isalpha(line.at(i)))   //we have encountered a variable name
	  zahl = 0;
	token += line.at(i); }    //read token
      if((c == ' ') || (i == line.length()-1)) {
	if((token != "") && (token[token.length()-1] != ':')) 
	  if((token.find(">")!=string::npos) || (token.find("<")!=string::npos)) { 
	    ++slack;
	    stringstream tmp;
	    tmp << slack;
	    token="slack"+tmp.str();
	    vars.insert(make_pair(token,k));  //we insert new slack variable
	    ++k; 
	    ++m;  //constraint line was worked through; there won't be new variablenames in line
	    break;}
	  else if(token.find("=")!=string::npos) {
	    ++m;
	    break;}
	  else if(zahl==0) {
	    map<string,int>::iterator iter = vars.find(token);
	    if(iter == vars.end()) {
	      vars.insert(make_pair(token,k)); //we insert new variable
	      ++k; }
	  }
	token="";
	zahl=1;
      }
    }
    token="";
    zahl=1; }

  if(line.find("Bounds")!=string::npos) {
    while(getline(file,line) && (line.find("End")==string::npos)) {
      if(line.find("/")!=string::npos) 
	break;
      else {
	istringstream ist(line);
	string token;
	if(line.find("<")==string::npos) {
	  if(line.find("=")!=string::npos)
	    ++m;  //  new equality constraint increase m - number of constraints
	  else if(line.find("free")!=string::npos || line.find("Free")!=string::npos) {
	    ist >> token;  //token becomes variable name
	    map<string,int>::iterator iter = vars.find(token);
	    if(iter == vars.end()) {
	      cout << "variable " << token << " never occured in constraint line" << endl;
	      return (8); }
	    else {
	      string token_neg;
	      token_neg = "minus_" + token;  //var free -> var = var+ - var-; var+,var- nonneg
	      vars.insert(make_pair(token_neg,k));
	      ++k; }
	  }
	}
	else {
	  int count=0;  //counts if bound is complete or shorter version
	  string bound[5];
	  while(ist >> token) {
	    bound[count]=token;
	    count++; }
	  if(count!=3 && count!=5) {
	    cout << "count = " << count << endl;
	    cout << endl << "bad bound statement" << endl;
	    continue; }
	
	  if(count==3) {
	    if(line.find("INF")==string::npos) { //no INF in bound constraint
	      if(is_string(bound[0])==1) { // var <= zahl
		string token_neg;
		token_neg = "minus_"+bound[0]; //var becomes var+ - var-
		vars.insert(make_pair(token_neg,k));
		++k;
		++slack;
		stringstream tmp;
		tmp << slack;
		token="slack"+tmp.str();
		vars.insert(make_pair(token,k));  //we insert new slack variable
		++k; 
		++m; }
	      else if(is_string(bound[2])==1) { //zahl <= var
		stringstream tmp_str;
		int tmp_int;
		tmp_str << bound[0];
		tmp_str >> tmp_int;
		if(tmp_int<0) { //zahl < 0
		  string token_neg;
		  token_neg = "minus_"+bound[0]; //var becomes var+ - var-
		  vars.insert(make_pair(token_neg,k));
		  ++k; 
		  ++slack;
		  stringstream tmp;
		  tmp << slack;
		  token="slack"+tmp.str();
		  vars.insert(make_pair(token,k));  //we insert new slack variable
		  ++k; 
		  ++m; }
		else if(tmp_int>0) { //zahl > 0
		  ++slack;
		  stringstream tmp;
		  tmp << slack;
		  token="slack"+tmp.str();
		  vars.insert(make_pair(token,k)); //we insert new slack variable
		  ++k; 
		  ++m; }
		else if(tmp_int == 0) { //zahl = 0
		}
		else {
		  cout << "error in bound declaration tmp_int = " << tmp_int << endl;
		  return (8); }
	      }
	    }
	    /* else {
	       cout << "some error in bound declaration" << endl;
	       return (8); }*/
	
	    else if(line.find("-INF")!=string::npos) {
	      string token_neg;
	      token_neg = "minus_"+bound[2]; //var free -> var = var+ - var-; var+,var- nonneg
	      vars.insert(make_pair(token_neg,k));
	      ++k; }
	    else if(line.find("+INF")!=string::npos) {
	      string token_neg;
	      token_neg = "minus_"+bound[0]; //var free -> var = var+ - var-; var+,var- nonneg
	      vars.insert(make_pair(token_neg,k));
	      ++k; }
	    else {
	      cout << "bad bound statement" << endl;
	      cout << line << endl;
	      return (8); }
	  }
				
	  if(count==5) {
	    //   cout << "count = " << count << endl;
	    if(line.find("-INF")!=string::npos) {
	      if(line.find("+INF")!=string::npos) {  //-INF <= var <= +INF
		string token_neg;
		token_neg = "minus_"+bound[2]; //var becomes var+ - var-
		vars.insert(make_pair(token_neg,k));
		++k; }
	      else {  // -INF <= var <= zahl
		string token_neg;
		token_neg = "minus_"+bound[2]; //var becomes var+ - var-
		vars.insert(make_pair(token_neg,k));
		++k; 
		++slack;
		stringstream tmp;
		tmp << slack;
		token="slack"+tmp.str();
		vars.insert(make_pair(token,k));
		++k; 
		++m; }
	    }
	    else if(line.find("+INF")!=string::npos) { //zahl <= var <= INF
	      stringstream tmp_str;
	      int tmp_int;
	      string str_tmp0;
	      str_tmp0 = bound[0];
	      //tmp_str << bound[0];
	      tmp_str << str_tmp0;
	      tmp_str >> tmp_int;
	      if(tmp_int<0) { //zahl < 0
		string token_neg;
		token_neg = "minus_"+bound[2]; //var becomes var+ - var-
		vars.insert(make_pair(token_neg,k));
		++k; 
		++slack;
		stringstream tmp;
		tmp << slack;
		token="slack"+tmp.str();
		vars.insert(make_pair(token,k));
		++k; 
		++m; }
	      else if(tmp_int > 0) { //zahl > 0
		++slack;
		stringstream tmp;
		tmp << slack;
		token="slack"+tmp.str();
		vars.insert(make_pair(token,k)); //we insert new slack variable
		++k; 
		++m; } 
	      else {  // 0 <= var <= +INF   - do nothing
	      }
	    }
	    else {  //zahl1 <= var <= zahl2
	      stringstream tmp_str;
	      int tmp_int;
	      string str_tmp2;
	      str_tmp2 = bound[0];
	      tmp_str << str_tmp2;
	      tmp_str >> tmp_int;
	      if(tmp_int<0) {  //zahl1 < 0
		string token_neg;
		token_neg = "minus_"+bound[2]; //var becomes var+ - var-
		vars.insert(make_pair(token_neg,k));
		++k;
		for(int i=0;i<2;i++) {
		  ++slack;
		  stringstream tmp;
		  tmp << slack;
		  token="slack"+tmp.str();
		  vars.insert(make_pair(token,k));
		  ++k; 
		  ++m; }
	      }
	      else if(tmp_int >= 0) { //zahl 1 >= 0
		for(int i=0;i<2;i++) {
		  ++slack;
		  stringstream tmp;
		  tmp << slack;
		  token="slack"+tmp.str();
		  vars.insert(make_pair(token,k));
		  ++k; 
		  ++m; }
	      }
	    }
	  }
	}
      }
    }
  }
  file.close();
  return m;
}
