#ifndef AU_MANAGEMENT_H
#define AU_MANAGEMENT_H

bool au_check(AU * au, char * filename);

NNInp * audiovv(MN * mn, AU * au, char * filename);
/* generiert aus einem au-File einen NNInp-Datensatz */

void free_nninp(NNInp *nninp);

#endif
