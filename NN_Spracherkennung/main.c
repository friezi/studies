#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

#define NB_N 5
#define NB_AU 2
#define EPOCHS 20

int nb_neurons[NB_N];
char byteorder;
int err_nb;
char trans[NB_N+1];
char *file="test.au";
char *descr="Descriptionblock fuer MN";

int main(int argc, char *argv[]){
  int i,n;
  MN mn1={0},*mn2=0,*mn3=0;
  time_t t;
  bool ok;
  MLP *mlpa=0,*mlpei=0,*mlpou=0;
  AudioSet auset;
  char **f=0;
  Hamming *hamm=0;
  
  int nb_layers=NB_N;
  int hamming=20;
  auset.nb_files=NB_AU;
  auset.descr=0;
  if (!(f=(char **)calloc(NB_AU,sizeof(char *)))){
    puts("zu wenig Speicher!");
    exit(1);
  }
  for (i=0;i<NB_AU;i++)
    f[i]=file;
  auset.files=f;
  srand((unsigned int)time(&t));
  
  
  for (i=0;i<NB_N;i++){
    nb_neurons[i]=rand()%NB_N+1;
    trans[i]=TANH;
  }
  
  if (!(mlpa=make_mlp(nb_layers,hamming,nb_neurons,trans))){
    puts("make_mlp fehlgeschlagen!");
    exit(1);
  }
  if (!(mlpei=make_mlp(nb_layers,hamming,nb_neurons,trans))){
    puts("make_mlp fehlgeschlagen!");
    exit(1);
  }
  if (!(mlpou=make_mlp(nb_layers,hamming,nb_neurons,trans))){
    puts("make_mlp fehlgeschlagen!"); 
    exit(1);
  }
  if (!(hamm=(Hamming *)calloc(1,sizeof(Hamming)))){
     puts("Kein Hamming!");
     exit(1);
   }
  hamm->start=0;
  hamm->stop=P_FFT_LEN-1;
  hamm->rednc=.5;
  hamm->fac=1.5;
  hamm->nb_hwin=hamming;
  
  mn1.a_net=mlpa;
  mn1.ei_net=mlpei;
  mn1.ou_net=mlpou;
  mn1.hamming=hamm;
  mn1.eps=0.01;
  mn1.descr_size=(int)strlen(descr);
  mn1.descr=descr;
  set_weights_bias(mlpa);
  set_weights_bias(mlpei);
  set_weights_bias(mlpou);

  for (i=0;i<EPOCHS;i++){
    printf("%s\t%d Durchlaeufe\n",auset.files[0],auset.nb_files);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(&mn1,&auset,0,n,0,(char)(TRAINFL),batch);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(&mn1,0,&auset,0,n,(char)(TESTFL),batch);
  }
  if (!err2gnu(mlpa->stats.firsttrain,"tnerr.gnu"))
    puts("Konnte kein Gnufile erstellen");
  if (!err2gnu(mlpa->stats.firsttest,"tterr.gnu"))
    puts("Konnte kein Gnufile erstellen");
  if (!err2gnu(mn1.stats.firsttrain,"mntnerr.gnu"))
    puts("Konnte kein Gnufile erstellen");
  if (!err2gnu(mn1.stats.firsttest,"mntterr.gnu"))
    puts("Konnte kein Gnufile erstellen");
  if (!save_mn(&mn1,"mn.mnet"))
    puts("Konnte MN nicht abspeichern");
  del_mlp(mlpa);
  del_mlp(mlpei);
  del_mlp(mlpou);
  if (!(mn2=load_mn("mn.mnet"))){
    puts("konnte mn2 nicht laden");
    return(1);
  }
  puts("");
  for (i=0;i<EPOCHS;i++){
    printf("%s\t%d Durchlaeufe\n",auset.files[0],auset.nb_files);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(mn2,&auset,0,n,0,(char)(TRAINFL),batch);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(mn2,0,&auset,0,n,(char)(TESTFL),batch);
  }
  printf("anz: %d\n",mn2->a_net->stats.train_epochs);
  if (!save_mn(mn2,"mn2.mnet")) 
    puts("Konnte MN nicht abspeichern"); 
  if (!(mn3=load_mn("mn2.mnet"))){
    puts("konnte mn2 nicht laden");
    return(1);
  }
  for (i=0;i<EPOCHS;i++){
    printf("%s\t%d Durchlaeufe\n",auset.files[0],auset.nb_files);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(mn3,&auset,0,n,0,(char)(TRAINFL),batch);
    for (n=0;n<auset.nb_files;n++)
      ok=manage_net(mn3,0,&auset,0,n,(char)(TESTFL),batch);
  }
  if (!err2gnu(mn3->a_net->stats.firsttrain,"tnerr3.gnu"))
    puts("Konnte kein Gnufile erstellen");     
  else 
    puts("tnerr3.gnu gespeichert!"); 
  if (!err2gnu(mn3->a_net->stats.firsttest,"tterr3.gnu"))   
    puts("Konnte kein Gnufile erstellen");   
  else 
    puts("tterr3.gnu gespeichert!");
  if (!err2gnu(mn3->stats.firsttrain,"mntnerr3.gnu"))
    puts("Konnte kein Gnufile erstellen"); 
  else 
    puts("mntnerr3.gnu gespeichert!");
  if (!err2gnu(mn3->stats.firsttest,"mntterr3.gnu"))
    puts("Konnte kein Gnufile erstellen");
  else 
    puts("mntnerr3.gnu gespeichert!");
  del_mn(mn2);
  del_mn(mn3);
  free(f);
  return(0);
}


