#include <stdio.h>
#include "datatypes.h"

bool save_mn(MN *mn, char *filename){
  int m,l,n,w;
  FILE *file;
  ListElem *le;
  MLP *mlp[NB_MLPS];

  if ((file=fopen(filename,"r"))){
    fclose(file);
    return(false);
  }
  mlp[0]=mn->a_net;
  mlp[1]=mn->ei_net;
  mlp[2]=mn->ou_net;
  file=fopen(filename,"w");
  fprintf(file,"NTBL");

#ifdef DBGSMN
  puts("NTBL geschrieben");
#endif

  fwrite(&mn->descr_size,sizeof(int),1,file);
  if (mn->descr)
    fprintf(file,mn->descr);

#ifdef DBGSMN
  puts("descr geschrieben");
#endif

  fwrite(mn->output,sizeof(double),5,file);
  fwrite(mn->hamming,sizeof(Hamming),1,file);
  fwrite(&mn->stats.train_epochs,sizeof(int),1,file);

#ifdef DBGSMN
  puts("stats geschrieben");
#endif

  le=mn->stats.firsttrain;
  while (le){                         /* mn->stats.firsttrain */
    fwrite(&le->data,sizeof(double),1,file);
    le=le->next;
  }

#ifdef DBGSMN
  puts("mn: firsttraindaten geschrieben");
#endif

  fwrite(&mn->stats.test_epochs,sizeof(int),1,file);
  le=mn->stats.firsttest;
  while (le){                         /* mn->stats.firsttest */
    fwrite(&le->data,sizeof(double),1,file);
    le=le->next;
  }

#ifdef DBGSMN
  puts("mn: firsttestdaten geschrieben");
#endif

  fwrite(&mn->eps,sizeof(double),1,file);
  fwrite(&mn->epoch,sizeof(int),1,file);
  for (m=0;m<NB_MLPS;m++){
    fwrite(&mlp[m]->nb_layers,sizeof(int),1,file);

#ifdef DBGSMN
    printf("mlp[%d]->nb_layers geschrieben\n",m);
#endif

    for (l=0;l<mlp[m]->nb_layers;l++){
      fwrite(&mlp[m]->layer[l]->nb_neurons,sizeof(int),1,file);
      fwrite(&mlp[m]->layer[l]->nb_weights,sizeof(int),1,file);
      fwrite(&mlp[m]->layer[l]->trans,sizeof(char),1,file);
      
#ifdef DBGSMN
      printf("mlp[%d]->layer[%d]->trans geschrieben\n",m,l);
      printf("mlp[%d]->layer[%d]->nb_neurons:%d\n",m,l,mlp[m]->layer[l]->nb_neurons);
      printf("mlp[%d]->layer[%d]->nb_weights:%d\n",m,l,mlp[m]->layer[l]->nb_weights);
#endif

      for (n=0;n<mlp[m]->layer[l]->nb_neurons;n++){
	for(w=0;w<mlp[m]->layer[l]->nb_weights;w++){
	  fwrite(&mlp[m]->layer[l]->neuron[n]->weight[w],sizeof(double),1,file);
	  
#ifdef DBGSMN
	  printf("mlp[%d]->layer[%d]->neuron[%d]->weight[%d] geschrieben\n",m,l,n,w);
#endif

	}
	for(w=0;w<mlp[m]->layer[l]->nb_weights;w++){
	  fwrite(&mlp[m]->layer[l]->neuron[n]->d_weight[w],sizeof(double),1,file);

#ifdef DBGSMN
	  printf("mlp[%d]->layer[%d]->neuron[%d]->d_weight[%d] geschrieben\n",m,l,n,w);
#endif

	}
	fwrite(&mlp[m]->layer[l]->neuron[n]->bias,sizeof(double),1,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->d_bias,sizeof(double),1,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->y,sizeof(double),1,file);

#ifdef DBGSMN
	printf("mlp[%d]->layer[%d]->neuron[%d]->y geschrieben\n",m,l,n);
#endif

      }
    }
    fwrite(mlp[m]->output,sizeof(double),3,file);
    fwrite(&mlp[m]->stats.train_epochs,sizeof(int),1,file);
    le=mlp[m]->stats.firsttrain;
    while (le){                         /* mn->stats.firsttrain */
      fwrite(&le->data,sizeof(double),1,file);
      le=le->next;
    }
    fwrite(&mlp[m]->stats.test_epochs,sizeof(int),1,file);
    le=mlp[m]->stats.firsttest;
    while (le){                         /* mn->stats.firsttest */
      fwrite(&le->data,sizeof(double),1,file);
      le=le->next;
    }
  }
  fclose(file);
  return(true);
}  

MN *load_mn(char *filename){
  FILE *file;
  char id[4];
  MN *mn=0;
  ListElem *le;
  MLP *mlp[NB_NLPS]={0};
  int m,l,n,w;

  if (!(file=fopen(filename,"r")))
    return(0);
  fread(id,sizeof(char),4,file);
  if strcmp("NTBL",id){

#ifdef DBGLMN
    puts("Kein mnet file!");
#endif

    fclose(file);
    return(0);
  }

  if (!(mn=(MN *)calloc(1,sizeof(MN)))){
    fclose(file);
    return(0);
  }

  fread(&mn->descr_size,sizeof(int),1,file);
  if (!(mn->descr=(char *)calloc(1,mn->descr_size+1))){
    free(mn);
    fclose(file);
  }
  fread(mn->descr,sizeof(char),mn->descr_size,file);

#ifdef DBGLNM
  puts("Description eingelesen");
#endif

  fread(mn->output,sizeof(double),5,file);
  if (!(mn->hamming=(Hamming *)calloc(1,sizeof(Hamming)))){
    free(mn->descr);
    free(mn);
    fclose(file);
    return(0);
  }
  fread(mn->hamming,sizeof(Hamming),1,file);

  fread(&mn->stats.train_epochs,sizeof(int),1,file);
  if (!(mn->stats.firsttrain=(ListElem *)calloc(1,sizeof(ListElem)))){
    free(mn->hamming);
    free(mn->descr);
    free(mn);
    fclose(file);
    return(0);
  }
  fread(&mn->stats.firsttrain->data,sizeof(double),1,file);
  le=mn->stats.firsttrain;

  for (i=0;i<mn->stats.train_epochs-1;i++){
    if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
      del_ftn(mn->stats.firsttrain);
      free(mn->hamming);
      free(mn->descr);
      free(mn);
      fclose(file);
      return(0);
    }
    fread(&mn->stats.firsttrain->next->data,sizeof(double),1,file);
    le=le->next;
  }

#ifdef DBGLMN
  puts(" mn: Trainingsepochenerrors eingelesen");
#endif

  fread(&mn->stats.test_epochs,sizeof(int),1,file);
  if (!(mn->stats.firsttest=(ListElem *)calloc(1,sizeof(ListElem)))){
    del_ftn(mn->stats.firsttrain);
    free(mn->hamming);
    free(mn->descr);
    free(mn);
    fclose(file);
    return(0);
  }

  fread(&mn->stats.firsttest->data,sizeof(double),1,file);
  le=mn->stats.firsttest;

  for (i=0;i<mn->stats.test_epochs-1;i++){
    if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
      del_ftt(mn->stats.firsttest);
      del_ftn(mn->stats.firsttrain);
      free(mn->hamming);
      free(mn->descr);
      free(mn);
      fclose(file);
      return(0);
    }
    fread(&mn->stats.firsttest->next->data,sizeof(double),1,file);
    le=le->next;
  }

#ifdef DBGLMN
  puts(" mn: Testepochenerrors eingelesen");
#endif

  fread(&mn->eps,sizeof(double),1,file);
  fread(&mn->epoch,sizeof(int),1,file);

  for (m=0;m<NB_MLPS;m++){
    if (!(mlp[m]=(MLP *)calloc(1,sizeof(MLP)))){
      for (i=0;i<m;i++)
	del_mlp(mlp[m]);
      del_ftt(mn->stats.firsttest);
      del_ftn(mn->stats.firsttrain);
      free(mn->hamming);
      free(mn->descr);
      free(mn);
      fclose(file);
      return(0);
    }

    fread(&mlp[m]->nb_layers,sizeof(int),1,file);
    for (l=0;l<mlp[m]->nb_layers;l++){
      if (!(mlp[m]->layer[l]=(Layer *)calloc(1,sizeof(Layer)))){
	for (i=0;i<l;i++)
	  del_layer(layer[i]);
	for (i=0;i<m;i++)
	  del_mlp(mlp[m]);
	del_ftt(mn->stats.firsttest);
	del_ftn(mn->stats.firsttrain);
	free(mn->hamming);
	free(mn->descr);
	free(mn);
	fclose(file);
	return(0);
      }
      
      fread(&mlp[m]->layer[l]->nb_neurons,sizeof(int),1,file);
      fread(&mlp[m]->layer[l]->nb_weights,sizeof(int),1,file);
      fread(&mlp[m]->layer[l]->trans,sizeof(char),1,file);

      for (n=0;n<mlp[m]->layer[l]->nb_neurons,n++){
	if (!(mlp[m]->layer[l]->neuron[0]=(Neuron *


bool tnerr2gnu(Stats *stats, char *filename){
  int i=0;
  FILE *file=0;
  ListElem *LE;
  
  if (!(file=fopen(filename,"w")))
    return(0);
  LE=stats->firsttrain;
  
  while (LE){
    fprintf(file,"%d\t%f\n",i,LE->data);
    i++;
    LE=LE->next;
  }
  fclose(file);
  return(true);
}

bool tterr2gnu(Stats *stats, char *filename){
  int i=0;
  FILE *file=0;
  ListElem *LE;
  
  if (!(file=fopen(filename,"w")))
    return(0);
  LE=stats->firsttest;
  
  while (LE){
    fprintf(file,"%d\t%f\n",i,LE->data);
    i++;
    LE=LE->next;
  }
  fclose(file);
  return(true);
}
