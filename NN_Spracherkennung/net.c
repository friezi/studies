#define NET_C

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "datatypes.h"

#define K_FAC 1

double logi(double x){
  return(1/(1+exp(x)));
}

double lin(double x){
  return(K_FAC*x);
}

double step0(double x){
  if (x<0){
    return(0);
  }else if(x>0){
    return(1);
  }else{
    return(0.5);
  }
}

double step1(double x){
  if (x<0){
    return(-1);
  }else if(x>0){
    return(1);
  }else{
    return(0);
  }
}

static double (*transf[])(double)={tanh,logi,step0,step1,lin};  /* Array von Adressen auf Transferfunktionen */

void prop(MLP *mlp, NNInp *nninp){
  int i,l,n,w;
  double sum;

  for (i=0;i<mlp->layer[0]->nb_neurons;i++)  /* Inputs in erste Schicht eintragen */
    mlp->layer[0]->neuron[i]->y=nninp->input[i];

#ifdef DEBUG
  puts("Inputs in erste Schicht eingetragen!");
#endif

/*****************************************************************
       Propagierung
******************************************************************/

#ifdef DBGPROP
    printf("Layer0->nb_neurons %d\n",mlp->layer[0]->nb_neurons);
#endif

  for (l=1;l<mlp->nb_layers;l++){   /* pro Layer */

#ifdef DBGPROP
    printf("Layer%d->nb_neurons %d\n",l,mlp->layer[l]->nb_neurons);
#endif
#ifdef DBGPROP
    printf("Layer %d\n",l);
#endif

    for (n=0;n<mlp->layer[l]->nb_neurons;n++){  /* pro Neuron */

#ifdef DBGPROP
      printf("Neuron %d\n",n);
#endif

      sum=0;
      for (w=0;w<mlp->layer[l]->nb_weights;w++){  /* Ergebnisse summieren */
	sum+=mlp->layer[l]->neuron[n]->weight[w] * mlp->layer[l-1]->neuron[w]->y - mlp->layer[l]->neuron[n]->bias;
#ifdef DBGPROP
	printf("summe: %f\n",sum);
#endif
      }
      mlp->layer[l]->neuron[n]->y=(*transf[(int)mlp->layer[l]->trans])(sum); /* y eintragen */
#ifdef DBGPROP
      printf("y: %f\n",mlp->layer[l]->neuron[n]->y);
#endif
    }
  }
  for (i=0;i<3;i++)   /* Eintrag der Werte der letzten Schicht in ouput des MLP's */
    mlp->output[i]=mlp->layer[mlp->nb_layers-1]->neuron[i]->y;
}

double tanh_dr(double x){   /* Ableitungsfunktion zu tanh */
  return(1-x*x);
}

double logi_dr(double x){
  return(x-x*x);
}

double step0_dr(double x){
  return(1);
}

double step1_dr(double x){
  return(1);
}

double lin_dr(double x){
  return(K_FAC);
}

static double (*transf_dr[])(double)={tanh_dr,logi_dr,step0_dr,step1_dr,lin_dr};  /* Adressarray auf Ableitungen der Transferfunktionen */

void backprop(MLP *mlp, Target *tg, int which_n){
  int i,l,n;
  double *tv,sum;

  switch (which_n){
  case A_N:
    tv=tg->a;
    break;
  case EI_N:
    tv=tg->ei;
    break;
  case OU_N:
    tv=tg->ou;
    break;
  default:
    return;
  }
  l=mlp->nb_layers-1;
  for (n=0;n<mlp->layer[l]->nb_neurons-1;n++)
    mlp->layer[l]->neuron[n]->delta=(-1)*(*transf_dr[(int)mlp->layer[l]->trans])(mlp->layer[l]->neuron[n]->y)*(tv[n]-mlp->layer[l]->neuron[n]->y);
  
#ifdef DBGBACK
  puts("Letzte Schicht");
#endif
  
  for (l=mlp->nb_layers-2;l>0;l--){
#ifdef DBGBACK
    printf("Layer[%d]\n",l);
#endif
    for (n=0;n<mlp->layer[l]->nb_neurons;n++){
      
      sum=0;
      for (i=0;i<mlp->layer[l+1]->nb_neurons;i++){
#ifdef DBGBACK
	printf("Summe: %f\n",sum);
#endif
	sum+=mlp->layer[l+1]->neuron[i]->delta*mlp->layer[l+1]->neuron[i]->weight[n];
      }
      mlp->layer[l]->neuron[n]->delta=(*transf_dr[(int)mlp->layer[l]->trans])(mlp->layer[l]->neuron[n]->y)*sum;
    }
  }
}


void postproc(MLP *mlp, double eps, trainmode b_o){
  int l,n,w;

  for (l=mlp->nb_layers-1;l>0;l--){

#ifdef DBGPOSTP
    printf("layer[%d]\n",l);
#endif

    for (n=0;n<mlp->layer[l]->nb_neurons;n++){

#ifdef DBGPOSTP
      printf("neuron[%d]\n",n);
#endif

      for (w=0;w<mlp->layer[l]->nb_weights;w++){
	mlp->layer[l]->neuron[n]->d_weight[w]=(-1)*eps*mlp->layer[l]->neuron[n]->delta*mlp->layer[l-1]->neuron[w]->y;

#ifdef DBGPOSTP
	printf("d_weight[%d]: %f\n",w,mlp->layer[l]->neuron[n]->d_weight[w]);
#endif
	
      }
      mlp->layer[l]->neuron[n]->d_bias=eps*mlp->layer[l]->neuron[n]->delta;

#ifdef DBGPOSTP
      printf("d_bias[%d]: %f\n",n,mlp->layer[l]->neuron[n]->d_bias);
#endif
	
    }
  }
}

void update(MLP *mlp){
  int l,n,w;

  for (l=1;l<mlp->nb_layers;l++){

#ifdef DBGUP
    printf("layer[%d]\n",l);
#endif

    for (n=0;n<mlp->layer[l]->nb_neurons;n++){

#ifdef DBGUP
      printf("neuron[%d]\n",n);
#endif

      for (w=0;w<mlp->layer[l]->nb_weights;w++){
	mlp->layer[l]->neuron[n]->weight[w]+=mlp->layer[l]->neuron[n]->d_weight[w];

#ifdef DBGUP
	printf("weight[%d]: %f\n",w,mlp->layer[l]->neuron[n]->weight[w]);
#endif

      }
      mlp->layer[l]->neuron[n]->bias+=mlp->layer[l]->neuron[n]->d_bias;

#ifdef DBGUP
      printf("bias[%d]: %f\n",n,mlp->layer[l]->neuron[n]->bias);
#endif
    }
  }
}

void set_mn_outp(MN *mn){
  mn->output[0]=(mn->ou_net->output[2]+mn->a_net->output[0]+mn->ei_net->output[2])/3;
  mn->output[1]=(mn->ou_net->output[2]+mn->a_net->output[2]+mn->ei_net->output[0])/3;
  mn->output[2]=(mn->ou_net->output[2]+mn->a_net->output[2]+mn->ei_net->output[1])/3;
  mn->output[3]=(mn->ou_net->output[0]+mn->a_net->output[1]+mn->ei_net->output[2])/3;
  mn->output[4]=(mn->ou_net->output[1]+mn->a_net->output[1]+mn->ei_net->output[2])/3;
}

double geterror(void *net, NNInp *nninp, int which_n){
  double *tv, *outp;
  double sum=0.0;
  int i,nb=3;
  
  
  switch(which_n){  /* Zeiger auf Targetvektor holen */
  case A_N:
    tv=nninp->target.a;
    outp=((MLP *)net)->output;  /* Zeiger auf Ergebnis der letzten Schicht holen */
    break;
  case EI_N:
    tv=nninp->target.ei;
    outp=((MLP *)net)->output;  /* Zeiger auf Ergebnis der letzten Schicht holen */
    break;
  case OU_N:
    tv=nninp->target.ou;
    outp=((MLP *)net)->output;  /* Zeiger auf Ergebnis der letzten Schicht holen */
/*     for (i=0;i<3;i++) */
/*       printf("target[%d]: %f\toutput[%d]: %f\n",i,tv[i],i,outp[i]); */
    break;
  case M_N:
    tv=nninp->target.total;
    outp=((MN *)net)->output;  /* Zeiger auf Ergebnis der letzten Schicht holen */
    nb=5;
    break;
  default:
    return(0.0);
  }

  for (i=0;i<nb;i++)  /* Fehlersumme berechnen */
    sum+=pow(tv[i]-outp[i],(double)2); /* siehe Formel im Skript */
  return(sum/2);
}

void addtailftn(Stats *stats, ListElem *le){
  ListElem *curr;

  if (!stats->firsttrain){
    stats->firsttrain=le;
    stats->train_epochs=1;
    return;
  }
  curr=stats->firsttrain;
  while (curr->next)
    curr=curr->next;
  curr->next=le;
  stats->train_epochs++;
}

void del_ftn(ListElem *tn){
  if (!tn)
    return;
  if (tn->next)
    del_ftn(tn->next);
  free(tn);
}

void del_tnerrs(Stats *stats){
  del_ftn(stats->firsttrain);
  stats->train_epochs=0;
}

void addtailftt(Stats *stats, ListElem *le){
  ListElem *curr;

  if (!stats->firsttest){
    stats->firsttest=le;
    stats->test_epochs=1;
    return;
  }
  curr=stats->firsttest;
  while (curr->next)
    curr=curr->next;
  curr->next=le;
  stats->test_epochs++;
}

void del_ftt(ListElem *tt){
  if (!tt)
    return;
  if (tt->next)
    del_ftt(tt->next);
  free(tt);
}

void del_tterrs(Stats *stats){
  del_ftt(stats->firsttest);
  stats->test_epochs=0;
}
