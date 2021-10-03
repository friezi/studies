#ifndef NET_H
#define NET_H

/********************************
Funktion: prop
propagiert die Daten fuer ein Netz
*********************************/
void prop(MLP *, NNInp *);

/********************************
funktion: backprop
macht die backpropagation fuer ein Netz
*********************************/
void backprop(MLP *mlp, Target *target, int which_n);

/********************************
funktion: postproc
fuehrt das Post-Processing durch in Abhaengigkeit von batch/online-flag:
batch-mode:
  delta_w's und delta_t's werden durch Anzahl der Files geteilt und zu den
  delta_w's und delta_t's der Neuron hinzuaddiert
online-mode:
  delta_w's und delta_t's werden direkt in den Neurons gespeichert
**********************************/
void postproc(MLP *mlp, double eps, trainmode b_o);
void update(MLP *mlp);
void set_mn_outp(MN *mn);

/********************************
Funktion: geterror
berechnet den Fehler der letzten Propagation (fuer Netz oder Multinetz)
*********************************/
double geterror(void *, NNInp *, int);

/********************************
Funktion: addtailftn
haengt ListElem:train-fehler an Liste (firsttrain) in  Stats ein
*********************************/
void addtailftn(Stats *, ListElem *);
void del_tnerrs(Stats *stats);
void del_ftn(ListElem *tn);

/********************************
Funktion: addtailftt
haengt ListElem:test-fehler an Liste (firsttest) in  Stats ein
*********************************/
void addtailftt(Stats *, ListElem *);
void del_tterrs(Stats *stats);
void del_ftt(ListElem *tt);

#endif
