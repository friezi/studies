#ifndef FILEIO_H
#define FILEIO_H

bool save_mn(MN *mnet,char *filename);
MN *load_mn(char *filename);
bool err2gnu(ListElem *first, char *filename);
int au2gnu(AU * au, char *filename);
int fft2gnu(int start, int stop, AU * au, char *filename);
int hamm2gnu(int len, NNInp * nninp, char *filename);
bool read_au(AU * au, char * filename);
/* filename nach au->data einlesen */


#endif
