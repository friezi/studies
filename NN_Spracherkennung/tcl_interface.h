#ifndef TCL_INTERFACE_H
#define TCL_INTERFACE_H

#include <tcl.h>

int c_load_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv);
int c_save_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv);
int c_generate_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv);
int c_nn_train(ClientData cd, Tcl_Interp *interp, int argc, char **argv);
int c_ende(ClientData cd, Tcl_Interp *interp, int argc, char **argv);

#endif
