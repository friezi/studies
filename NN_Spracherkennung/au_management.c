#include <stdlib.h>
#include <stdio.h>
#include "datatypes.h"
#include "errors.h"
#include "au_management.h"
#include "fileio.h"
#include "au.h"

extern int err_nb;

bool au_check(AU * au, char * filename){
/*   printf("entering au_check ... \n"); */

  if(!(read_au(au, filename)))  /* filename nach au->data einlesen */
     return(false);             /* sonst Ende */
  if(!(check_au_type(au)))      /* checken, ob wirklich au-file */
    return(false);              /* sonst Ende */
  return(true);

/*   printf("leaving au_check ... \n"); */
}


NNInp * audiovv(MN * mn, AU * au, char * filename){
  NNInp * nninp;   /* Zeiger auf NNInp, welches gefuellt wird */

  if(!(nninp=calloc(1, sizeof(NNInp)))){ /* Speicher fuer NNInp-struct */
    err_nb=NEM;
    return(NULL);                       /* beschaffen, sonst Ende */
  }

#ifdef DBGAUVV
  else
    printf("Speicher fuer NNInp beschafft...\n");
#endif

  if(!(nninp->input=calloc((size_t)mn->hamming->nb_hwin, sizeof(double)))){ /* Speicher */
    err_nb=NEM;
    return(NULL);                /* fuer Input-Werte beschaffen, sonst Ende */
  }

#ifdef DBGAUVV
  else
    printf("Speicher fuer NNInp->Input beschafft... \n");
#endif

  if(!(au_check(au, filename)))  /* file einlesen und checken */
    return(NULL);                /* sonst Ende */

#ifdef DBGAUVV
  else
    printf("Checkfile erfolgreich...\n");
#endif

  if(!(decode(au)))              /* au-Format dekodieren */
     return(NULL);               /* sonst Ende */

#ifdef DBGAUVV
  else
    printf("decode erfolgreich...\n");
#endif

  fft_func(au);                  /* Fast-Fourier-Transformation durchfuehren */

#ifdef DBGAUVV
  printf("fft erledigt...\n");
#endif

  amp(mn->hamming->start, au->hamming->stop, au); /* Amplituden von start bis stop berechnen */

#ifdef DBGAUVV
  printf("Amplituden der FFT berechnet ...\n");
#endif

  hamming(au, nninp->input); /* Werte fuer nninp mittels Hamming-Fenster berechnen */
#ifdef DBGAUVV
  printf("Hammingfenster erfolgreich gebildet...\n");
#endif

  norm(mn, nninp);                 /* Input-Werte normieren */

#ifdef DBGAUVV
  printf("Inputwerte normiert ...\n");
#endif

  make_target(mn, au, &nninp->target);  /* Soll-Werte in nninp eintragen */

#ifdef DBGAUVV
  printf("Targetwerte eingetragen...\n");
#endif

  return(nninp);                   /* Zeiger auf nninp zurueckgeben */
}

void free_nninp(NNInp *nninp){
  free(nninp->input);
  free(nninp);
}
