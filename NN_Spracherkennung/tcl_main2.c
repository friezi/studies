#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <tcl.h>
#include <tk.h>
#include "datatypes.h"
#include "tcl_interface.h"

MN *mn=0;
char byteorder;
int err_nb;


/* ------------------- Main --------------------------*/
int Tcl_AppInit(Tcl_Interp *interp){
  if(Tcl_Init(interp)==TCL_ERROR || Tk_Init(interp)==TCL_ERROR)
    return TCL_ERROR;
  
  /* Tcl/Tk-C - Interface: */
  
/*   Tcl_CreateCommand(interp,"c_make_mlp", make_mlp, NULL, NULL); */
/*   Tcl_CreateCommand(interp,"c_audiovv", audiovv, NULL, NULL); */
/*   Tcl_CreateCommand(interp,"c_manage_net", manage_net, NULL, NULL); */
  Tcl_CreateCommand(interp,"c_load_net", c_load_net, NULL, NULL);
  Tcl_CreateCommand(interp,"c_save_net", c_save_net, NULL, NULL);
  Tcl_CreateCommand(interp,"c_ende", c_ende, NULL, NULL);
  Tcl_CreateCommand(interp,"c_generate_net", c_generate_net, NULL, NULL); 
  
  /* Tcl/Tk Hauptprogramm - Dateiname */
  Tcl_SetVar(interp, "tcl_rcFileName","hauptfenster.tcl",TCL_GLOBAL_ONLY);
  
  return TCL_OK;
}

int main(int argc, char **argv){
  time_t t;

  srand((unsigned int)time(&t));
  Tk_Main(argc,argv,Tcl_AppInit);			/* Kontrolle liegt bei Tcl */
  return 0;						/* ... und OK an das OS */
}


