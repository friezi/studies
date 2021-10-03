/*
    Copyright (C) 1999-2004 Friedemann Zintel

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    For any questions, contact me at
    friezi@cs.tu-berlin.de
*/

/**
   @file exception.cpp
   @author Friedemann Zintel
*/

#include <exception.hpp>

using namespace std;
  
ExceptionBase::ExceptionBase(const string &id){
    this->errormsg = "";
    this->spc_id = id;
    this->id_errormsg = this->spc_id;
  }
  
ExceptionBase::ExceptionBase(const string &id, const string &errormsg){
    this->errormsg = errormsg;
    this->spc_id = id;
    this->id_errormsg = this->spc_id + ": " + this->errormsg;
  }

string ExceptionBase::skipDigits(const string &s){

  const char *sptr = s.c_str();

  while (isdigit(*sptr))
    sptr++;

  return string(sptr);
}

string ExceptionBase::skipLetters(const string &s){

  const char *sptr = s.c_str();

  while (isalpha(*sptr))
    sptr++;

  return string(sptr);
}

ostream& operator<<(ostream& ostr, const ExceptionBase& e){

  ostr << e.getIdMsg();
  return ostr;
}

void ExceptionBase::show() const{

  cerr << *this << endl;
}

ExceptionBase & ExceptionBase::prependMsg(string msg){

  this->errormsg = msg + this->errormsg;
  this->id_errormsg = this->spc_id + ": " + this->errormsg;

  return *this;

}
