#define NET_MGMNT_C

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "datatypes.h"
#include "au_management.h" 
#include "net.h"

Neuron *make_neuron(int nb_weights){
  Neuron *neuron=0;

  if (!(neuron=(Neuron *)calloc(1,sizeof(Neuron)))) /* Speicher f. Neuron beschaffen */
    return(0);
  if (!(neuron->weight=(double *)calloc((size_t)nb_weights,sizeof(double)))){ /* S. f. weightarray b. */
    free(neuron);
    return(0);
  }
  if (!(neuron->d_weight=(double *)calloc((size_t)nb_weights,sizeof(double)))){ /* S. f. d_weightarray b */
    free(neuron->weight);  /* bei Fehlschlag alles andere freigeben */
    free(neuron);
    return(0);
  }
  return(neuron);
}

void del_neuron(Neuron *neuron){
  free(neuron->weight);
  free(neuron->d_weight);
  free(neuron);
  neuron=0;
}

Layer *make_layer(int nb_neurons, int nb_weights, char trans){
  int n,i;
  Layer *layer=0;
  Neuron *neuron=0;
  Neuron **n_addr=0;
  
  if (!(layer=(Layer *)calloc(1,sizeof(Layer)))) /* Speicher fuer Layer beschaffen */
    return(0);
  
  if (!(n_addr=(Neuron **)calloc((size_t)nb_neurons,sizeof(Neuron *)))){ /* S. f. N-Adressarray besch. */
    free(layer);
    return(0);
  }
  layer->neuron=n_addr;  
  for (n=0;n<nb_neurons;n++){ /* Neuronen anlegen */
    if (!(neuron=make_neuron(nb_weights))){  /* falls Fehlschlag */
      for (i=0;i<nb_neurons;i++)      /* alle angelegten Neurone wieder freigeben */
	del_neuron(layer->neuron[i]);
      free(layer);                   /* layer freigeben */
      return(0);
    }
    layer->neuron[n]=neuron;  /* und in Adressarray eintragen */
  }

  layer->nb_neurons=nb_neurons;   /* uebrige Werte eintragen */
  layer->trans=trans;    
  layer->nb_weights=nb_weights;

  return(layer);
}

void del_layer(Layer *layer){

  int n;
  
  if (layer){
    for (n=0;n<layer->nb_neurons;n++)
      del_neuron(layer->neuron[n]);
    free(layer->neuron);
    free(layer);
    layer=0;
  }
}

MLP *make_mlp(int nb_layers, int nb_hammingw, int *nb_neurons, char *trans){
  int l,i;
  int *nb_weights=0;
  MLP *mlp=0;
  Layer *layer=0,*in=0,*out=0;
  Layer **l_addr=0;


#ifdef DBGMMLP
  puts("start");
#endif

  if (!(nb_weights=(int *)calloc((size_t)nb_layers+1,sizeof(int)))){
    return(0);
  }

#ifdef DBGMMLP
    puts("Speicher beschafft fuer nb_weights");
#endif

  nb_weights[0]=nb_hammingw;

#ifdef DBGMMLP
  puts("Gewicht erste Schicht");
#endif

  for (l=1;l<nb_layers+1;l++)
    nb_weights[l]=nb_neurons[l-1];

#ifdef DBGMMLP
  puts("gewichte aller Schichten");
#endif

  if (!(mlp=(MLP *)calloc(1,sizeof(MLP)))){
    free(nb_weights);
    return(0);
  }
  if (!(l_addr=(Layer **)calloc((size_t)nb_layers+2,sizeof(Layer *)))){
    free(mlp);
    free(nb_weights);
    return(0);
  }
  mlp->layer=l_addr;

#ifdef DBGMMLP
  puts("Speicher fuer Layeradressen beschafft");
#endif

  if (!(in=make_layer(nb_hammingw,0,(char)0))){
    free(mlp->layer);
    free(mlp);
    free(nb_weights);
    return(0);
  }
  mlp->layer[0]=in;
  if (!(out=make_layer(3,nb_weights[nb_layers],(char)trans[nb_layers]))){
    free(in);
    free(mlp->layer);
    free(mlp);
    free(nb_weights);
    return(0);
  }
  mlp->layer[nb_layers+1]=out;

#ifdef DBGMMLP
  puts("In und Out Layer geschaffen");
#endif

  for (l=1;l<nb_layers+1;l++){
    if (!(layer=make_layer(nb_neurons[l-1],(char)nb_weights[l-1],trans[l-1]))){
      for (i=0;i<nb_layers+2;i++)
	del_layer(mlp->layer[i]);
      free(mlp->layer);
      free(mlp);
      free(nb_weights);
      return(0);
    }
    mlp->layer[l]=layer;

#ifdef DBGMMLP
    puts("make_layer aufgerufen");
#endif

  }
  mlp->nb_layers=nb_layers+2;
  free(nb_weights);
  return(mlp);
}

void del_mlp(MLP *mlp){
  int l;

  if (mlp){
    for (l=0;l<mlp->nb_layers;l++)
      del_layer(mlp->layer[l]);
    free(mlp->layer);
    del_tnerrs(&mlp->stats);
    del_tterrs(&mlp->stats);
    free(mlp);
    mlp=0;
  }
}

void del_mn(MN *mn){
  if (mn){
    del_mlp(mn->a_net);
    del_mlp(mn->ei_net);
    del_mlp(mn->ou_net);
    free(mn->hamming);
    free(mn->descr);
    mn=0;
  }
}

void set_weights_bias(MLP *mlp){
  int l,n,w;
/*  time_t sw_t;*/

 /* srand((unsigned int)time(&sw_t));*/
  for (l=1;l<mlp->nb_layers;l++){
    
#ifdef DBGSWB
    printf("layer[%d]\n",l);
#endif
    
    for (n=0;n<mlp->layer[l]->nb_neurons;n++){
      
#ifdef DBGSWB
      printf("neuron[%d]\n",n);
#endif
      
      for (w=0;w<mlp->layer[l]->nb_weights;w++){
	mlp->layer[l]->neuron[n]->weight[w]=((double)rand())/RAND_MAX/5-.1;
	
#ifdef DBGSWB
	printf("weight[%d]: %f\n",w,mlp->layer[l]->neuron[n]->weight[w]);
#endif
	
      }
      mlp->layer[l]->neuron[n]->bias=((double)rand())/RAND_MAX/5-.1;
      
#ifdef DBGSWB
      printf("bias[%d]: %f\n",n,mlp->layer[l]->neuron[n]->bias);
#endif
      
    }
  }
}

bool manage_net(MN *mn, AudioSet *trainset, AudioSet *testset, int trainnb, int testnb, char tnt_flag, trainmode b_o){

  int n;
  NNInp *train, *test;
  MLP *mlp[3];
  ListElem *errelem,*curr[4];
  double err;
  AU au;                 /* fuer audiovv */

#ifdef DBGMNG
      puts("manage_net!");
#endif
  mlp[0]=mn->a_net;
  mlp[1]=mn->ei_net;
  mlp[2]=mn->ou_net;

  
  au.hamming=mn->hamming;
  
  /**************************************************
   ******************* TRAINING *********************
   **************************************************/
  if ((tnt_flag&TRAINFL)&&trainset){
#ifdef DBGMNG
    puts("Training!");
#endif
    /* pro epoche einen Fehler speichern, deshalb beim ersten file der Epoche */
    /* ein neues fehlerelement beschaffen*/
    for (n=0;n<NB_MLPS;n++){
      if (!trainnb){
	if (!(errelem=(ListElem *)calloc(1,sizeof(ListElem))))  /* speicher fuer fehlerelement beschaffen */
	  return(false);
	addtailftn(&mlp[n]->stats, errelem);
      }
      curr[n]=mlp[n]->stats.firsttrain;   /* Bis zum Fehlereintrag der aktuellen Epoche durchhangeln */
      while (curr[n]->next)
	curr[n]=curr[n]->next;
    }
    if (!trainnb){
      if (!(errelem=(ListElem *)calloc(1,sizeof(ListElem))))
	return(false);
      addtailftn(&mn->stats,errelem);
    }
    curr[M_N]=mn->stats.firsttrain;
    while (curr[M_N]->next)
      curr[M_N]=curr[M_N]->next;
    
    /*       printf("%d\n",file); */
    if (!(train=audiovv(mn,&au,trainset->files[trainnb]))){    /* macht gesamte audio-Vorverarbeitung fuer aktuelles file */
      
#ifdef DBGMNG
      puts("Fehler in auvv!");
#endif
      
      return(false);      /* falls nicht geklappt -> abbrechen */
    }
    
#ifdef DBGMNG
    puts("audiovv ausgefuehrt!");
#endif
    
    for (n=0;n<NB_MLPS;n++){    /* propagation u.a. fuer alle drei Netze */
      
#ifdef DBGMNG
      printf("mlp[%d]\n",n);
#endif
      prop(mlp[n],train); /* propagation der daten */
      
#ifdef DBGMNG
      puts("prop ausgefuehrt!");
#endif

      err=geterror((void *)mlp[n],train,n);  /* Fehler berechnen */
      curr[n]->data+=err/trainset->nb_files;  /* Fehler (gemittelt) addieren */
      backprop(mlp[n],&train->target,n);    /* backpropagation */
      
#ifdef DBGMNG
      puts("backprop ausgefuehrt!");
#endif
      
      postproc(mlp[n],mn->eps,b_o);  /* post processing */
      
#ifdef DBGMNG
      puts("postproc ausgefuehrt!");
#endif
      
      if (b_o==online){       /* nur update, wennn im online-Modus */
	update(mlp[n]);
	
#ifdef DBGMNG
	puts("online-update ausgefuehrt!");
#endif
	
      }
    }
    set_mn_outp(mn);                     /* Outputwerte des MN's setzen */
    err=geterror((void *)mn,train,M_N);   /* error fuer's mn berechnen */
    curr[M_N]->data+=err/trainset->nb_files; /* gemittelten Teilfehler addieren */
    free_nninp(train);
    if (b_o==batch)
      for (n=0;n<NB_MLPS;n++){
	
#ifdef DBGMNG
	puts("batch-update ausgefuehrt!");
#endif
	
	update(mlp[n]);
      }
    
  }
  /*************************************************
   ********************* Test ***********************
   **************************************************/
  if ((tnt_flag&TESTFL)&&testset){
#ifdef DBGMNG
    puts("Test!");
#endif
    /* pro epoche einen Fehler speichern, deshalb beim ersten file der Epoche */
    /* ein neues fehlerelement beschaffen */
    for (n=0;n<NB_MLPS;n++){
      if (!testnb){
	if (!(errelem=(ListElem *)calloc(1,sizeof(ListElem))))  /* speicher fuer fehlerelement beschaffen */
	  return(false);
	addtailftt(&mlp[n]->stats, errelem);
      }
      curr[n]=mlp[n]->stats.firsttest;   /* Bis zum Fehlereintrag der aktuellen Epoche durchhangeln */
      while (curr[n]->next)
	curr[n]=curr[n]->next;
    }
    if (!testnb){
      if (!(errelem=(ListElem *)calloc(1,sizeof(ListElem))))
	return(false);
      addtailftt(&mn->stats,errelem);
    }
    curr[M_N]=mn->stats.firsttest;
    while (curr[M_N]->next)
      curr[M_N]=curr[M_N]->next;
    
    if (!(test=audiovv(mn,&au,testset->files[testnb])))
      return(false);
    for (n=0;n<OU_N;n++){
      prop(mlp[n],test);
      err=geterror((void *)mlp[n],test,n);
      curr[n]->data+=err/testset->nb_files;  /* Fehler (gemittelt) addieren */
    }
    set_mn_outp(mn);                     /* Outputwerte des MN's setzen */
    err=geterror((void *)mn,test,M_N);   /* error fuer's mn berechnen */
    curr[M_N]->data+=err/testset->nb_files; /* gemittelten Teilfehler addieren */
    free_nninp(test);
  }
  return(true);
}

