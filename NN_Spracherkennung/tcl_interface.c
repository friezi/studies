#include <stdlib.h>
#include <string.h>
#include <tcl.h>
#include <tk.h>
#include "datatypes.h"
#include "net-mgmnt.h"
#include "fileio.h"

extern MN *mn;

void update_settings(Tcl_Interp *interp){

  int m,l;
  char nbn_string[15]={0},t_string[10]={0};
  char buff[10];
  int result;
  MLP *mlp[3]={0};

  result=sprintf(buff,"%d",mn->hamming->nb_hwin);
  Tcl_SetVar(interp,"nb_hwin",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%d",mn->hamming->start);
  Tcl_SetVar(interp,"start",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%d",mn->hamming->stop);
  Tcl_SetVar(interp,"stop",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%f",mn->hamming->rednc);
  Tcl_SetVar(interp,"rednc",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%f",mn->hamming->fac);
  Tcl_SetVar(interp,"fac",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%f",mn->eps);
  Tcl_SetVar(interp,"eps",buff,TCL_GLOBAL_ONLY);
  Tcl_SetVar(interp,"descr",mn->descr,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%d",mn->a_net->nb_layers-2);
  Tcl_SetVar(interp,"nb_layers0",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%d",mn->ei_net->nb_layers-2);
  Tcl_SetVar(interp,"nb_layers1",buff,TCL_GLOBAL_ONLY);
  result=sprintf(buff,"%d",mn->ou_net->nb_layers-2);
  Tcl_SetVar(interp,"nb_layers2",buff,TCL_GLOBAL_ONLY);
  mlp[0]=mn->a_net;
  mlp[1]=mn->ei_net;
  mlp[2]=mn->ou_net;
  for (m=0;m<3;m++){
    strcpy(nbn_string,"nb_neurons");
    strcpy(t_string,"trans");
    nbn_string[10]='0'+m;
    nbn_string[11]='_';
    nbn_string[13]=0;
    t_string[5]='0'+m;
    t_string[6]='_';
    t_string[8]=0;
    for (l=0;l<mlp[m]->nb_layers-1;l++){
      if (l<10){
	nbn_string[12]='0'+l;
	t_string[7]='0'+l;
      }
      else{
	nbn_string[12]='0'+l/10;
	nbn_string[13]='0'+l%10;
	t_string[7]='0'+l/10;
	t_string[8]='0'+l%10;
      }
      if (l<mlp[m]->nb_layers-1){ /* hier naechstemal weiterdenken */
	result=sprintf(buff,"%d",mlp[m]->layer[l+1]->nb_neurons);
	Tcl_SetVar(interp,nbn_string,buff,TCL_GLOBAL_ONLY);
      }
      result=sprintf(buff,"%d",mlp[m]->layer[l+1]->trans);
      Tcl_SetVar(interp,t_string,buff,TCL_GLOBAL_ONLY);
    }
  }
}

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
  update_settings(interp);
  return TCL_OK;
}

int c_save_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  if (argc!=2){
    interp->result="Invalid number of arguments in function c_save_net!";
    return TCL_ERROR;
  }

  if (!mn){
    interp->result="no current net in function c_save_net!";
    return TCL_ERROR;
  }
  if (!(save_mn(mn,argv[1]))){
    interp->result="could not save net!";
    return TCL_ERROR;
  }
  return TCL_OK;
}

int c_generate_net(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  int nb_layers,m,i,*nb_neurons;
  char nbn_string[15]={0},nbl_string[11]={0}, t_string[10]={0};
  char *descr, *trans;
  MLP *mlp[3]={0};
  
#ifdef DBGGN
  puts("Generate Net");
  fflush(NULL);
#endif

  if (mn)
    del_mn(mn);
  if (!(mn=(MN *)calloc(1,sizeof(MN)))){
    interp->result="Not enough memory in function c_generate_net!";
    return TCL_ERROR;
  }
  if (!(mn->hamming=(Hamming *)calloc(1,sizeof(Hamming)))){
    interp->result="Not enough memory in function c_generate_net!";
    del_mn(mn);
    return TCL_ERROR;
  }
  mn->hamming->nb_hwin=atoi(Tcl_GetVar(interp,"nb_hwin",TCL_GLOBAL_ONLY)); 

#ifdef DBGGN
  printf("nb_hwin:%d\n",mn->hamming->nb_hwin);
  fflush(NULL);
#endif

  for (m=0;m<3;m++){
    strcpy(nbl_string,"nb_layers");
    nbl_string[9]='0'+m;
    nb_layers=atoi(Tcl_GetVar(interp,nbl_string,TCL_GLOBAL_ONLY));

#ifdef DBGGN
    printf(nbl_string);
    printf(": %d\n",nb_layers);
    fflush(NULL);
#endif

    if (!(nb_neurons=(int *)calloc(nb_layers,sizeof(int)))){
      interp->result="Not enough memory in function c_generate_net!";
      for (i=0;i<m;i++)
	del_mlp(mlp[i]);
      del_mn(mn);
      return TCL_ERROR;
    }
    if (!(trans=(char *)calloc(nb_layers+1,sizeof(char)))){
      interp->result="Not enough memory in function c_generate_net!";
      free(nb_neurons);
      for (i=0;i<m;i++)
	del_mlp(mlp[i]);
      del_mn(mn);
      return TCL_ERROR;
    }
    strcpy(nbn_string,"nb_neurons");
    nbn_string[10]='0'+m;
    nbn_string[11]='_';
    nbn_string[13]=0;
    for (i=0;i<nb_layers;i++){
      if (i<10)
	nbn_string[12]='0'+i;
      else{
	nbn_string[12]='0'+i/10;
	nbn_string[13]='0'+i%10;
      }
      nb_neurons[i]=atoi(Tcl_GetVar(interp,nbn_string,TCL_GLOBAL_ONLY));

#ifdef DBGGN
      printf(nbn_string);
      printf(": %d\n",nb_neurons[i]);
      fflush(NULL);
#endif

    }
    strcpy(t_string,"trans");
    t_string[5]='0'+m;
    t_string[6]='_';
    t_string[8]=0;
    for (i=0;i<nb_layers+1;i++){
      if (i<10)
	t_string[7]='0'+i;
      else{
	t_string[7]='0'+i/10;
	t_string[8]='0'+i%10;
      }
      trans[i]=(char)atoi(Tcl_GetVar(interp,t_string,TCL_GLOBAL_ONLY));

#ifdef DBGGN
      printf(t_string);
      printf(": %d\n",trans[i]);
      fflush(NULL);
#endif

    }
    if (!(mlp[m]=make_mlp(nb_layers,mn->hamming->nb_hwin,nb_neurons,trans))){
      interp->result="Not enough memory in function c_generate_net!";
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
	     
  mn->descr_size=atoi(Tcl_GetVar(interp,"descr_size",TCL_GLOBAL_ONLY));
  if (!(mn->descr=(char *)calloc(1,mn->descr_size+1))){
    interp->result="Not enough memory in function c_generate_net!";
    del_mn(mn);
    return TCL_ERROR;
  }
  descr=Tcl_GetVar(interp,"descr",TCL_GLOBAL_ONLY);
  strncpy(mn->descr,descr,mn->descr_size);

#ifdef DBGGN
  printf("descr: ");
  printf(mn->descr);
  puts("");
  fflush(NULL);
#endif

  mn->eps=atof(Tcl_GetVar(interp,"eps",TCL_GLOBAL_ONLY));
  mn->hamming->start=atoi(Tcl_GetVar(interp,"start",TCL_GLOBAL_ONLY))-1;
  mn->hamming->stop=atoi(Tcl_GetVar(interp,"stop",TCL_GLOBAL_ONLY))-1;
  mn->hamming->rednc=atof(Tcl_GetVar(interp,"rednc",TCL_GLOBAL_ONLY));
  mn->hamming->fac=atof(Tcl_GetVar(interp,"fac",TCL_GLOBAL_ONLY));

#ifdef DBGGN
  printf("a: Anzahl Layers: %d\n",mn->a_net->nb_layers);
  printf("ei: Anzahl Layers: %d\n",mn->ei_net->nb_layers);
  printf("ou: Anzahl Layers: %d\n",mn->ou_net->nb_layers);
  fflush(NULL);
#endif

  return TCL_OK;
}

int c_nn_train(ClientData cd, Tcl_Interp *interp, int argc, char **argv){
#define NB_AU 2
#define EPOCHS 5

  int i,n;
  char **f;
  AudioSet auset;
  char *file="test.au";
  auset.nb_files=NB_AU;
  auset.descr=0;
  if (!(f=(char **)calloc(NB_AU,sizeof(char *)))){
    puts("zu wenig Speicher!");
    exit(1);
  }
  for (i=0;i<NB_AU;i++)
    f[i]=file;
  auset.files=f;
  for (i=0;i<EPOCHS;i++){
    printf("%s\t%d Durchlaeufe\n",auset.files[0],auset.nb_files);
    for (n=0;n<auset.nb_files;n++)
      manage_net(mn,&auset,0,n,0,(char)(TRAINFL),batch);
    for (n=0;n<auset.nb_files;n++)
      manage_net(mn,0,&auset,0,n,(char)(TESTFL),batch);
  }
  free(f);

  return TCL_OK;
}
  

int c_ende(ClientData cd, Tcl_Interp *interp, int argc, char **argv){

  del_mn(mn);
  return TCL_OK;
}
