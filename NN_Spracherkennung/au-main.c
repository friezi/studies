#include <stdlib.h>
#include <stdio.h>
#include "datatypes.h"
#include "au_management.h"
#include "au.h"
#include <math.h>

int main(){
  /* nur zum TEST */

  AU aufile;
  char filename[]="test2.au";
  NNInp nninp;

  if(au_check(&aufile, filename)){
    if(!(decode(&aufile))){
      printf("decode fehlgeschlagen...\n");
      return(false);
    }
    fft_func(&aufile);
    amp(0,P_FFT_LEN-1, &aufile);
    aufile.hamming=calloc(1, sizeof(Hamming));
    aufile.hamming->start=0;
    aufile.hamming->stop=P_FFT_LEN-1;
    aufile.hamming->rednc=0.25;
    aufile.hamming->fac=1.5;
    aufile.hamming->nb_hwin=10;
    if(!(hamming(&aufile, nninp.input)))
      printf("Hammingfensterl'n fehlgeschlagen...\n");
  }
  else
    printf("au_check fehlgeschlagen...\n");
  if(aufile.data)
    free(aufile.data);  
  if(aufile.hamming)
    free(aufile.hamming);
  return(0);
}
