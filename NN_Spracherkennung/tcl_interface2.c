#include <stdlib.h>
#include <strings.h>
#include <tcl.h> 
#include "datatypes.h"
#include "net-mgmnt.h"
#include "fileio.h"

extern MN *mn;

int c_load_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  if (argc!=2){
    interp->result="Invalid number of arguments in function c_load_net!";
    return TCL_ERROR;
  }

  if (mn)
    del_mn(mn);
  if (!(mn=load_mn(argv[1]))){
    interp->result="Could not load net!";
    return TCL_ERROR;
  }
  return TCL_OK;
}

int c_save_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  if (argc!=2){
    interp->result="Invalid number of arguments in function c_load_net!";
    return TCL_ERROR;
  }

  if (!mn){
    interp->result="No current net!";
    return TCL_ERROR;
  }
  if (!(save_mn(mn,argv[1]))){
    interp->result="Could not save net!";
    return TCL_ERROR;
  }
  return TCL_OK;
}

int c_generate_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  int nb_layers,m,i,*nb_neurons,t_buf;
  char nbn_string[15]={0},nbl_string[11]={0}, t_string[9]={0};
  char *descr, *trans;
  MLP *mlp[3]={0};
  
  if (mn)
    del_mn(mn);
  if (!(mn=(MN *)calloc(1,sizeof(MN)))){
    interp->result="Not enough memory!";
    return TCL_ERROR;
  }
  if (!(mn->hamming=(Hamming *)calloc(1,sizeof(Hamming)))){
    interp->result="Not enough memory!";
    del_mn(mn);
    return TCL_ERROR;
  }
  Tcl_GetInt(interp,"nb_hwin",&mn->hamming->nb_hwin);
  for (m=0;m<3;m++){
    strcpy(nbl_string,"nb_layers");
    nbl_string[9]='0'+m;
    Tcl_GetInt(interp,nbl_string,&nb_layers);
    if (!(nb_neurons=(int *)calloc(nb_layers,sizeof(int)))){
      interp->result="Not enough memory!";
      for (i=0;i<m;i++)
	del_mlp(mlp[i]);
      del_mn(mn);
      return TCL_ERROR;
    }
    if (!(trans=(char *)calloc(nb_layers,sizeof(char)))){
      interp->result="Not enough memory!";
      free(nb_neurons);
      for (i=0;i<m;i++)
	del_mlp(mlp[i]);
      del_mn(mn);
      return TCL_ERROR;
    }
    strcpy(nbn_string,"nb_neurons");
    nbn_string[10]='0'+m;
    nbn_string[11]='_';
    for (i=0;i<nb_layers;i++){
      if (i<10)
	nbn_string[12]='0'+i;
      else{
	nbn_string[12]='0'+i/10;
	nbn_string[13]='0'+i%10;
      }
      Tcl_GetInt(interp,nbn_string,&nb_neurons[i]);
    }
    strcpy(t_string,"trans");
    t_string[5]='0'+m;
    t_string[6]='_';
    for (i=0;i<nb_layers+1;i++){
      if (i<10)
	t_string[7]='0'+i;
      else{
	nbn_string[7]='0'+i/10;
	nbn_string[8]='0'+i%10;
      }
      Tcl_GetInt(interp,t_string,&t_buf);
      trans[i]=(char)t_buf;
    }
    if (!(mlp[m]=make_mlp(nb_layers,mn->hamming->nb_hwin,nb_neurons,trans))){
      interp->result="Not enough memory!";
      free(trans);
      free(nb_neurons);
      for (i=0;i<m;i++)
	del_mlp(mlp[i]);
      del_mn(mn);
      return TCL_ERROR;
    }
    set_weights_bias(mlp[m]);
    free(trans);
    free(nb_neurons);
  }
  mn->a_net=mlp[0];
  mn->ei_net=mlp[1];
  mn->ou_net=mlp[2];
	     
  Tcl_GetInt(interp,"descr_size",&mn->descr_size);
  if (!(mn->descr=(char *)calloc(1,mn->descr_size+1))){
    interp->result="Not enough memory!";
    del_mn(mn);
    return TCL_ERROR;
  }
  descr=Tcl_GetVar(interp,"descr",TCL_GLOBAL_ONLY);
  strncpy(mn->descr,descr,mn->descr_size);
  Tcl_GetDouble(interp,"eps",&mn->eps);
  Tcl_GetInt(interp,"start",&mn->hamming->start);
  Tcl_GetInt(interp,"stop",&mn->hamming->stop);
  Tcl_GetDouble(interp,"rednc",&mn->hamming->rednc);
  Tcl_GetDouble(interp,"fac",&mn->hamming->fac);
  return TCL_OK;
}

int c_ende(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  del_mn(mn);
  return TCL_OK;
}
