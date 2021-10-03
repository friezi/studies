#ifndef NET_MGMNT_H
#define NET_MGMNT_H

/*************************************************
funktion: make_neuron

richtet eine Neuron-Struktur ein
***************************************************/
Neuron *make_neuron(int nb_weights);


/*************************************************
funktion: del_neuron

gibt den Speicher eines Neurons samt Eintraegen frei
*************************************************/
void del_neuron(Neuron *neuron);


/**************************************************************
funktion: make_layer

richtet eine Layer-Struktur ein, beschafft dabei Speicher fuer nb_neuron-Anzahl
Neurone
*****************************************************************/
Layer *make_layer(int nb_neuron, int nb_weights, char trans);


/*************************************************
funktion: del_layer

gibt den Speicher eines layers samt Eintraegen frei
*************************************************/
void del_layer(Layer *layer);


/****************************************************
funktion: make_mlp

richtet eine MLP-Struktur ein, beschafft dabei speicher fuer alle Layers und alle Neurone
******************************************************/
MLP *make_mlp(int nb_layers, int nb_hammingw, int *nb_neurons, char *trans);


/*************************************************
funktion: del_mlp

gibt den Speicher eines Mlps samt Eintraegen frei, auch alle Train- und Testfehler
*************************************************/
void del_mlp(MLP *mlp);
void del_mn(MN *mn);
void set_weights_bias(MLP *mlp);


/***********************************************************************
funktion: manage_net

ruft nacheinander fuer alle drei Netze die Propagationsfunktionen auf
************************************************************************/
bool manage_net(MN *mn, AudioSet *trainset, AudioSet *testset, int trainnb, int testnb, char tnt_flag, trainmode b_o);

#endif
