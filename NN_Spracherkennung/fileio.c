#include <stdio.h>
#include <tcl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "datatypes.h"
#include "net-mgmnt.h"
#include "net.h"

extern MN *mn;

bool save_mn(MN *mnet,char *filename){
  int m,l,n,nb_layers;
  FILE *file;
  ListElem *le;
  MLP *mlp[NB_MLPS];

  if ((file=fopen(filename,"r"))){  /* Falls schon datei vorhanden, Abbruch */
    fclose(file);
    return(false);
  }
  mlp[0]=mnet->a_net;
  mlp[1]=mnet->ei_net;
  mlp[2]=mnet->ou_net;
  file=fopen(filename,"w");
  fprintf(file,"NTBL");

#ifdef DBGSMN
  puts("NTBL geschrieben");
#endif

  fwrite(&mnet->descr_size,sizeof(int),1,file);
  if (mnet->descr)
    fprintf(file,mnet->descr);

#ifdef DBGSMN
  puts("descr geschrieben");
#endif

  fwrite(mnet->output,sizeof(double),5,file);
  fwrite(mnet->hamming,sizeof(Hamming),1,file);
  fwrite(&mnet->stats.train_epochs,sizeof(int),1,file);

#ifdef DBGSMN
  puts("stats geschrieben");
#endif

  le=mnet->stats.firsttrain;
  while (le){                         /* mnet->stats.firsttrain */
    fwrite(&le->data,sizeof(double),1,file);
    le=le->next;
  }

#ifdef DBGSMN
  puts("mnet: firsttraindaten geschrieben");
#endif

  fwrite(&mnet->stats.test_epochs,sizeof(int),1,file);
  le=mnet->stats.firsttest;
  while (le){                         /* mnet->stats.firsttest */
    fwrite(&le->data,sizeof(double),1,file);
    le=le->next;
  }
#ifdef DBGSMN
  puts("mn: firsttestdaten geschrieben");
#endif

  fwrite(&mnet->eps,sizeof(double),1,file);
  fwrite(&mnet->epoch,sizeof(int),1,file);
  for (m=0;m<NB_MLPS;m++){
    nb_layers=mlp[m]->nb_layers-2;
    fwrite(&nb_layers,sizeof(int),1,file);
    fwrite(&mlp[m]->layer[0]->nb_neurons,sizeof(int),1,file);  /* Anzahl der Hammingfenster */
    for (l=1;l<mlp[m]->nb_layers-1;l++)
      fwrite(&mlp[m]->layer[l]->nb_neurons,sizeof(int),1,file);
    for (l=1;l<mlp[m]->nb_layers;l++)
      fwrite(&mlp[m]->layer[l]->trans,sizeof(char),1,file);
    for (l=1;l<mlp[m]->nb_layers;l++)
      fwrite(&mlp[m]->layer[l]->nb_weights,sizeof(int),1,file);
  }
  for (m=0;m<NB_MLPS;m++){
    for (l=1;l<mlp[m]->nb_layers;l++){
      for (n=0;n<mlp[m]->layer[l]->nb_neurons;n++){
	fwrite(mlp[m]->layer[l]->neuron[n]->weight,sizeof(double),(size_t)mlp[m]->layer[l]->nb_weights,file);
	fwrite(mlp[m]->layer[l]->neuron[n]->d_weight,sizeof(double),(size_t)mlp[m]->layer[l]->nb_weights,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->bias,sizeof(double),1,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->d_bias,sizeof(double),1,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->y,sizeof(double),1,file);
	fwrite(&mlp[m]->layer[l]->neuron[n]->delta,sizeof(double),1,file);
      }
    }
    fwrite(mlp[m]->output,sizeof(double),3,file); 
    fwrite(&mlp[m]->stats.train_epochs,sizeof(int),1,file); 
    le=mlp[m]->stats.firsttrain; 
    while (le){                         /* mnet->stats.firsttrain */ 
      fwrite(&le->data,sizeof(double),1,file); 
      le=le->next; 
    }      
    fwrite(&mlp[m]->stats.test_epochs,sizeof(int),1,file); 
    le=mlp[m]->stats.firsttest; 
    while (le){                         /* mnet->stats.firsttest */ 
      fwrite(&le->data,sizeof(double),1,file);
      le=le->next; 
    }
  }
  fclose(file);
  return(true);
} 

MN *load_mn(char *filename){
  int i,m,l,n,nb_layers,*nb_neurons=0,*nb_weights=0,nb_hamming;
  MLP *mlp[NB_MLPS]={0};
  FILE *file=0;
  MN *mnet=0;
  char id[4],*trans=0;
  ListElem *le=0;

  if (!(file=fopen(filename,"r")))
    return(0);
  fread(id,sizeof(char),4,file);
  if (strncmp("NTBL",id,4)){

#ifdef DBGLMN
    puts("Kein mnet file!");
#endif

    fclose(file);
    return(0);
  }

  if (!(mnet=(MN *)calloc(1,sizeof(MN)))){
    fclose(file);
    return(0);
  }

  fread(&mnet->descr_size,sizeof(int),1,file);
  if (mnet->descr_size){
    if (!(mnet->descr=(char *)calloc(1,(size_t)mnet->descr_size+1))){
      free(mnet);
      fclose(file);
      return(0);
    }
    fread(mnet->descr,sizeof(char),(size_t)mnet->descr_size,file);

#ifdef DBGLMN
    puts("Description eingelesen");
    printf("description: %s\n",mnet->descr);
#endif
  
  }  
  fread(mnet->output,sizeof(double),5,file);
  if (!(mnet->hamming=(Hamming *)calloc(1,sizeof(Hamming)))){
    free(mnet->descr);
    free(mnet);
    fclose(file);
    return(0);
  }
  fread(mnet->hamming,sizeof(Hamming),1,file);

  fread(&mnet->stats.train_epochs,sizeof(int),1,file);
  if (mnet->stats.train_epochs){
    if (!(mnet->stats.firsttrain=(ListElem *)calloc(1,sizeof(ListElem)))){
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }
    fread(&mnet->stats.firsttrain->data,sizeof(double),1,file);
  }
  le=mnet->stats.firsttrain;
  if (le){
    for (i=0;i<mnet->stats.train_epochs-1;i++){
      if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
	del_ftn(mnet->stats.firsttrain);
	free(mnet->hamming);
	free(mnet->descr);
	free(mnet);
	fclose(file);
	return(0);
      }
      fread(&le->next->data,sizeof(double),1,file);
      le=le->next;
    }
  }
  
#ifdef DBGLMN
  printf("Trainingsepochenerrors: %d\n",mnet->stats.train_epochs);
#endif

  fread(&mnet->stats.test_epochs,sizeof(int),1,file);
  if (mnet->stats.test_epochs){
    if (!(mnet->stats.firsttest=(ListElem *)calloc(1,sizeof(ListElem)))){
      del_ftn(mnet->stats.firsttrain);
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }
    fread(&mnet->stats.firsttest->data,sizeof(double),1,file);
  }
  le=mnet->stats.firsttest;
  if (le){
    for (i=0;i<mnet->stats.test_epochs-1;i++){
      if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
	del_ftt(mnet->stats.firsttest);
	del_ftn(mnet->stats.firsttrain);
	free(mnet->hamming);
	free(mnet->descr);
	free(mnet);
	fclose(file);
	return(0);
      }
      fread(&le->next->data,sizeof(double),1,file);
      le=le->next;
    }
  }

#ifdef DBGLMN
  printf("Testepochenerrors: %d\n",mnet->stats.test_epochs);
#endif

  fread(&mnet->eps,sizeof(double),1,file);
  fread(&mnet->epoch,sizeof(int),1,file);
  for (m=0;m<NB_MLPS;m++){
    fread(&nb_layers,sizeof(int),1,file);
    fread(&nb_hamming,sizeof(int),1,file);
    if (!(nb_neurons=(int *)calloc((size_t)nb_layers,sizeof(int)))){
      for (i=0;i<m;i++)
	del_mlp(mlp[m]);
      del_ftt(mnet->stats.firsttest);
      del_ftn(mnet->stats.firsttrain);
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }

#ifdef DBGLMN
  puts("Speicher fuer nb_neurons beschafft");
#endif

    if (!(trans=(char *)calloc((size_t)nb_layers+1,sizeof(char)))){
      free(nb_neurons);
      for (i=0;i<m;i++)
	del_mlp(mlp[m]);
      del_ftt(mnet->stats.firsttest);
      del_ftn(mnet->stats.firsttrain);
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }

#ifdef DBGLMN
    puts("Speicher fuer trans beschafft");
#endif

    if (!(nb_weights=(int *)calloc((size_t)nb_layers+1,sizeof(int)))){
      free(nb_neurons);
      free(trans);
      for (i=0;i<m;i++)
	del_mlp(mlp[m]);
      del_ftt(mnet->stats.firsttest);
      del_ftn(mnet->stats.firsttrain);
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }

#ifdef DBGLMN
    puts("Speicher fuer nb_weights beschafft");
#endif

    fread(nb_neurons,sizeof(int),(size_t)nb_layers,file);
    fread(trans,sizeof(char),(size_t)nb_layers+1,file);
    fread(nb_weights,sizeof(int),(size_t)nb_layers+1,file);
/*     for (i=0;i<nb_layers+1;i++) */

#ifdef DBGLMN
    puts("nb_neurons, trans und nb_weights eingelesen");
    printf("layers: %d\thammings: %d\n",nb_layers,nb_hamming);
#endif


    if (!(mlp[m]=make_mlp(nb_layers, nb_hamming, nb_neurons, trans))){

#ifdef DBGLMN
      puts("konnte kein mlp erschaffen");
#endif

      free(nb_weights);
      free(trans);
      free(nb_neurons);
      for (i=0;i<m;i++)
	del_mlp(mlp[m]);
      del_ftt(mnet->stats.firsttest);
      del_ftn(mnet->stats.firsttrain);
      free(mnet->hamming);
      free(mnet->descr);
      free(mnet);
      fclose(file);
      return(0);
    }

#ifdef DBGLMN
    puts("make_mlp aufgerufen");
#endif

    for (l=1;l<mlp[m]->nb_layers;l++)       /* nb_weigts in die layers eintragen */
      mlp[m]->layer[l]->nb_weights=nb_weights[l-1];

    free(nb_weights);
    free(trans);
    free(nb_neurons);
  }

  for (m=0;m<NB_MLPS;m++){

#ifdef DBGLMN
    printf("mlp[%d]\n",m);
#endif

    for (l=1;l<mlp[m]->nb_layers;l++){

#ifdef DBGLMN
      printf("layer[%d]\n",l);
#endif

      for (n=0;n<mlp[m]->layer[l]->nb_neurons;n++){

#ifdef DBGLMN
	printf("neuron[%d]\n",n);
#endif

	fread(mlp[m]->layer[l]->neuron[n]->weight,sizeof(double),(size_t)mlp[m]->layer[l]->nb_weights,file);
	fread(mlp[m]->layer[l]->neuron[n]->d_weight,sizeof(double),(size_t)mlp[m]->layer[l]->nb_weights,file);
	fread(&mlp[m]->layer[l]->neuron[n]->bias,sizeof(double),1,file);
	fread(&mlp[m]->layer[l]->neuron[n]->d_bias,sizeof(double),1,file);
	fread(&mlp[m]->layer[l]->neuron[n]->y,sizeof(double),1,file);
	fread(&mlp[m]->layer[l]->neuron[n]->delta,sizeof(double),1,file);
      }
    }
    fread(mlp[m]->output,sizeof(double),3,file); 
    fread(&mlp[m]->stats.train_epochs,sizeof(int),1,file);
    if (mlp[m]->stats.train_epochs){
      if (!(mlp[m]->stats.firsttrain=(ListElem *)calloc(1,sizeof(ListElem)))){
	for (i=0;i<m;i++)
	  del_mlp(mlp[m]);
	free(mnet->hamming);
	free(mnet->descr);
	free(mnet);
	fclose(file);
	return(0);
      }
      fread(&mlp[m]->stats.firsttrain->data,sizeof(double),1,file);
      le=mlp[m]->stats.firsttrain;
      for (i=0;i<mlp[m]->stats.train_epochs-1;i++){
	if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
	  for (i=0;i<m;i++)
	    del_mlp(mlp[m]);
	  free(mnet->hamming);
	  free(mnet->descr);
	  free(mnet);
	  fclose(file);
	  return(0);
	}
	fread(&le->next->data,sizeof(double),1,file);
	le=le->next;
      }
    }
    fread(&mlp[m]->stats.test_epochs,sizeof(int),1,file);
    if (mlp[m]->stats.test_epochs){
      if (!(mlp[m]->stats.firsttest=(ListElem *)calloc(1,sizeof(ListElem)))){
	for (i=0;i<m;i++)
	  del_mlp(mlp[m]);
	free(mnet->hamming);
	free(mnet->descr);
	free(mnet);
	fclose(file);
	return(0);
      }
      fread(&mlp[m]->stats.firsttest->data,sizeof(double),1,file);
      le=mlp[m]->stats.firsttest;
      for (i=0;i<mlp[m]->stats.test_epochs-1;i++){
	if (!(le->next=(ListElem *)calloc(1,sizeof(ListElem)))){
	  for (i=0;i<m;i++)
	    del_mlp(mlp[m]);
	  free(mnet->hamming);
	  free(mnet->descr);
	  free(mnet);
	  fclose(file);
	  return(0);
	}
	fread(&le->next->data,sizeof(double),1,file);
	le=le->next;
      }
    }
  }
  mnet->a_net=mlp[0];
  mnet->ei_net=mlp[1];
  mnet->ou_net=mlp[2];
  fclose(file);
  return(mnet);
}

bool err2gnu(ListElem *first, char *filename){
  int i=0;
  FILE *file=0;
  ListElem *le;
  
  if ((file=fopen(filename,"r"))){  /* Falls schon datei vorhanden, Abbruch */
    fclose(file);
    return(false);
  }
  if (!(file=fopen(filename,"w")))
    return(false);
  le=first;

  while (le){
    fprintf(file,"%d\t%f\n",i,le->data);
    i++;
    le=le->next;
  }
  fclose(file);
  return(true);
}

int au2gnu(AU * au, char *filename){
  int i=0;
  FILE *file=0;
  
  if (!(file=fopen(filename,"w")))
    return(0);
  
  for(i=0;i<au->datalength/32;i++)
    fprintf(file,"%d\t%f\n",i,au->decode[32*i]);
  fclose(file);
  return(1);
}

int fft2gnu(int start, int stop, AU * au, char *filename){
  int i=0;
  FILE *file=0;
  
  if (!(file=fopen(filename,"w")))
    return(0);
  
  for(i=start;i<=stop;i++)
    fprintf(file,"%d\t%f\n",i,au->fft[i]);
  fclose(file);
  return(1);
}

int hamm2gnu(int len, NNInp * nninp, char *filename){
  int i=0;
  FILE *file=0;
  
  if (!(file=fopen(filename,"w")))
    return(0);
  
  for(i=0;i<len;i++)
    fprintf(file,"%d\t%f\n",i,nninp->input[i]);
  fclose(file);
  return(1);
}

bool read_au(AU * au, char * filename){
/* filename nach au->data einlesen */
  int filedesc=0;     /* Filedescriptor */
  int filelen=0;      /* Laenge der Datei */
  struct stat buf;    /* Statusinformationen zur Datei */

  if((filedesc=open(filename, O_RDONLY))==-1) /* File oeffnen */
    return(false);                            /* sonst Ende */
#ifdef DBGAUIO
  printf("File geoeffnet ...\n");
#endif

  if(fstat(filedesc, &buf))   /* Statusinformationen holen */
    return(false);            /* sonst Ende */
#ifdef DBGAUIO
  printf("Status des File geholt ...\n");
#endif

  filelen=(int)buf.st_size;   /* Filelaenge speichern */
  if(!(au->data=calloc((size_t)filelen, sizeof(char))))   /* Speicher allokieren */
    return(false);                                /* sonst Ende */
#ifdef DBGAUIO
  printf("Speicher fuer au-data beschafft ...\n");
#endif

  au->datalength=filelen;                         /* Speichergroesse merken */
  if(read(filedesc, au->data,(size_t) filelen)!=filelen)  /* File einlesen */
    return(false);                                /* sonst Ende */
#ifdef DBGAUIO
  printf("au-data eingelesen ...\n");
#endif

  return(!(close(filedesc))?true:false);          /* File schliessen */
}
