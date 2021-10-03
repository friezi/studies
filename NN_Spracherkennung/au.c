#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "datatypes.h"
#include "errors.h"
#include "au.h"
#include "fft.h"

extern char byteorder;
extern int err_nb;

char * change_order(char *i, int len){
  /* kehrt die Reihenfolge der Bytes um */

  int j;
  signed char *t;
  if(!(t=(signed char *)calloc((size_t)len, sizeof(char)))){
    err_nb=NEM;
    return(0);
  }

  for(j=0;j<len;j++){
    t[j]=*(((signed char *)i)+j);
  }
  for(j=0;j<len;j++){
    *(((signed char *)i)+j)=t[3-j];
  }
  free(t);
  return(i);
}

bool check_au_type(AU * au){
/* checken, ob wirklich au-file */

  char target_a[]="A_V";
  char target_e[]="E_V";
  char target_i[]="I_V";
  char target_o[]="O_V";
  char target_u[]="U_V";
  char target_k[]="K_V";

  signed int au_type1;       /* Magicnumber1 fuer au-files */
  signed int au_type2;       /* Magicnumber2 fuer au-files */
  signed int encoding;       /* u-law encoding */
  signed int samplerate;     /* samplerate */
  signed int nb_channels;    /* mono */

  /* fuer Intel-Recher */
  signed int au_type1_i;     /* Magicnumber1 fuer au-files */
  signed int au_type2_i;     /* Magicnumber2 fuer au-files */
  signed int encoding_i;     /* u-law encoding */
  signed int samplerate_i;   /* samplerate */
  signed int nb_channels_i;  /* mono */

  signed int headersize;     /* Groesse des Headers */
  int targetsize;            /* Groesse der Target-Beschreibung */

  au_type1      = 0x2E736E64;  /* Magicnumber1 fuer au-files */
  au_type2      = 0x2E736400;  /* Magicnumber2 fuer au-files */
  encoding      = 0x00000001;  /* u-law encoding */
  samplerate    = 0x00002B11;  /* samplerate */
  nb_channels   = 0x00000001;  /* mono */

  /* fuer Intel-Rechner */
  au_type1_i    = 0x646E732E;  /* Magicnumber1 fuer au-files */
  au_type2_i    = 0x0064732E;  /* Magicnumber2 fuer au-files */
  encoding_i    = 0x01000000;  /* u-law encoding */
  samplerate_i  = 0x112B0000;  /* samplerate */
  nb_channels_i = 0x01000000;  /* mono */


#ifdef DBGAU
  printf("checking magicnumbers ... ");
#endif

  if((memcmp(au->data, &au_type1, 4)) && (memcmp(au->data, &au_type2, 4))){
    if((memcmp(au->data, &au_type1_i, 4)) && (memcmp(au->data, &au_type2_i, 4))){
#ifdef DBGAU
      {
	int i;
	printf("wrong\nFile\tMagicnumber1 (einfach)\tMagicnumber1 (umgedreht)\n");
	for(i=0;i<4;i++)
	  printf("%#X <-> \t%#X\t\t\t%#X\n", *(au->data+i), *(((char *)&au_type1)+i), *(((char *)&au_type1_i)+i));
	printf("\nFile\tMagicnumber2 (einfach)\tMagicnumber2 (umgedreht)\n");
	for(i=0;i<4;i++)
	  printf("%#X <-> \t%#X\t\t\t%#X\n", *(au->data+i), *(((char *)&au_type2)+i), *(((char *)&au_type2_i)+i));
	printf("\n");
      }
      err_nb=WMN;
#endif
      return(false);           /* Magicnumbers testen, sonst Ende */
    }
    else{
      byteorder = WRONG;
#ifdef DBGAU
      puts("OK\nau-Daten in umgekehrter Reihenfolge");
#endif
    }
  }
  else{
    byteorder = OK;
#ifdef DBGAU
    puts("OK\nau-Daten in einfacher Reihenfolge");
#endif
  }

  if(byteorder==WRONG){
    change_order(au->data, 4);    /* magicnumber */
    change_order(au->data+4, 4);  /* headersize */
    change_order(au->data+8, 4);  /* datalength */
    change_order(au->data+12, 4); /* encoding */
    change_order(au->data+16, 4); /* samplerate */
    change_order(au->data+20, 4); /* nb_channels */
  }

#ifdef DBGAU
  printf("checking encoding ... ");
#endif
    
  if(memcmp(au->data+12, &encoding, 4)){      /* encoding testen */
    err_nb=WEN;
    return(false);                         /* sonst Ende */
  }
#ifdef DBGAU
  printf("OK\nchecking samplerate ... ");
#endif

  if(memcmp(au->data+16, &samplerate, 4)){       /* samplerate testen */
#ifdef DBGAU
    {
      int i;
      printf("falsche Samplingrate: %d\n",*(au->data+16));
      for(i=0;i<4;i++)
	printf("%#X <-> %#X\n", *(au->data+16+i), *(((char *)&samplerate)+16+i));
      printf("\n");
    }
    err_nb=WSR;
#endif
    return(false);                         /* sonst Ende */
  }

#ifdef DBGAU
  printf("OK\nchecking #channels ... ");
#endif
    
  if(memcmp(au->data+20, &nb_channels, 4)){  /* Anzahl Kanaele testen */
    err_nb=WCH;
    return(false);                          /* sonst Ende */
  }

#ifdef DBGAU
  printf("OK\nreading headersize and datalength\n");
#endif

#ifdef DBGAU
  printf("memcpy(&headersize, au->data+4, 4);\n");
#endif
  memcpy(&headersize, au->data+4, 4);       /* Headergroesse auslesen */
#ifdef DBGAU
  printf("headersize (hex): %#X\n",headersize);
  printf("headersize (dez): %d\n",headersize);
#endif

  targetsize = headersize - 24;
#ifdef DBGAU
  printf("targetsize (hex): %#X\n",targetsize);
  printf("targetsize (dez): %d\n",targetsize);
#endif
  
#ifdef DBGAU
  printf("memcpy(&(au->datalength), au->data+8, 4);\n");
#endif
  memcpy(&(au->datalength), au->data+8, 4); /* Datalength auslesen */
#ifdef DBGAU
  printf("datalength (hex): %#X\n",au->datalength);
  printf("datalength (dez): %d\n",au->datalength);
#endif

#ifdef DBGAU
  printf("checking target-status ... ");
#endif

  if(targetsize==0){
    err_nb=NTG;
    return(false);
  }

  if(byteorder==WRONG)
    change_order(au->data+24, targetsize);

  if(memcmp(au->data+24, &target_a, (size_t)targetsize))              /* Sounddatei fuer Vokal a */
    if(memcmp(au->data+24, &target_e, (size_t)targetsize))            /* Sounddatei fuer Vokal e */
      if(memcmp(au->data+24, &target_i, (size_t)targetsize))          /* Sounddatei fuer Vokal i */
	if(memcmp(au->data+24, &target_o, (size_t)targetsize))        /* Sounddatei fuer Vokal o */
	  if(memcmp(au->data+24, &target_u, (size_t)targetsize))      /* Sounddatei fuer Vokal u */
	    if(memcmp(au->data+24, &target_k, (size_t)targetsize)){   /* Sounddatei ist kein Vokal */
#ifdef DBGAU
	      {
		int i;
		printf("wrong\n\"");
		for(i=0;i<targetsize;i++)
		  printf("%c", *(au->data+24+i));
		printf("\"\n");
	      }
	      err_nb=NTG;
#endif
	      return(false);                        /* Sounddatei ist nicht typisiert */
	    }
	    else{
#ifdef DBGAU
	      printf("OK\t kein Vokal\n");
#endif
	      au->target=K_V;
	    }
	  else{
#ifdef DBGAU
	    printf("OK\tu Vokal\n");
#endif
	    au->target=U_V;
	  }
	else{
#ifdef DBGAU
	  printf("OK\to Vokal\n");
#endif
	  au->target=O_V;
	}
      else{
#ifdef DBGAU
	printf("OK\ti Vokal\n");
#endif
	au->target=I_V;
      }
    else{
#ifdef DBGAU
      printf("OK\te Vokal\n");
#endif
      au->target=E_V;
    }
  else{
#ifdef DBGAU
    printf("OK\ta Vokal\n");
#endif
    au->target=A_V;
  }

#ifdef DBGAU
  printf("memmove(au->data, au->data+headersize, au->datalength);\n");
#endif
  memmove(au->data, au->data+headersize, (size_t)au->datalength); /* auf Daten redu. */

  return(true);
}

int get_pos_max(AU * au){
/* ermittelt Position mit groesster Amplitude */

  int pos_max=0;
  int i=0;

  for(i=0;i<au->datalength;i++){            /* In einer Schleife bis zum */
    if((au->data[i])>(au->data[pos_max]))   /* groessten Wert laufen */
      pos_max=i;                            /* und diesen speichern */
  }
#ifdef DBGAU
  printf("max_pos=%d\n",pos_max);
#endif
  return(pos_max);                          /* und zurueckgeben */
}

void cut_au(AU * au){
/* schneidet au-file auf AU_LEN Samples zurecht */

  const int auh=AU_LEN/2;
  const int auhm1=(AU_LEN/2)-1;
  const int first_pos=0;
  const int last_pos=au->datalength-1;

  int pos_max=0, start=0;

  pos_max=get_pos_max(au);         /* Position mit groesstem Pegel ermittlen */
  if(pos_max-auh>=first_pos){      /* start und ende ermitteln */
    if(pos_max+auhm1<=last_pos){
      start=pos_max-auh;
    }
    else{
      start=start-((pos_max+auhm1)-last_pos);
    }
  }
  else{
    start=first_pos;
  }
#ifdef DBGAU
  printf("start=%d\tende=%d\n",start,start+AU_LEN-1);
#endif
  au->datalength=AU_LEN;
  memmove(au->data, au->data+start, (size_t)au->datalength); /* auf Daten redu. */
}

bool check_file_length(AU * au){
/* Laenge checken und gegebenenfalls kuerzen*/

  if (au->datalength<8192){      /* wenn zu wenig Daten */
    err_nb=SDL;
    return(false);               /* dann Ende */
  }
  if (au->datalength>8192)       /* wenn zuviel Daten */
    cut_au(au);                  /* dann beschneiden */
  return(true);
}

void semi_invert(AU * au){
/* au-Rohdaten semiinvertieren */

  int i;
  
  for(i=0;i<au->datalength;i++){
    if(au->data[i]==64)
      au->data[i]=0;
    else
      au->data[i]=(au->data[i]>=0?128-au->data[i]:au->data[i]);
  }
}

void au_amp(AU * au){
/* Amplituden aus semiinvertierten au-Daten errechnen */

  int i;
  double factor;

  factor=pow(2047.0,1.0/127.0);
  for(i=0;i<au->datalength;i++)
    if(au->data[i]>0)
      au->decode[i]=pow(factor,(double)au->data[i]);
    else
      if(au->data[i]<0)
	au->decode[i]=-pow(factor,-((double)au->data[i]));
}

bool decode(AU * au){
/* au-Format dekodieren */
  semi_invert(au);
  if(!(check_file_length(au)))  /* Laenge checken und gegebenenfalls */
    return(false);              /* kuerzen, sonst Ende */
  au_amp(au);                   /* Amplituden berechnen und in au->decode */
  free(au->data);               /* speichern, au->data freigeben */

  return(true);
}

void fft_func(AU * au){
/* Fast-Fourier-Transformation durchfuehren */
  int i;

  for(i=0;i<AU_LEN;i++){            /* In einer Schleife */
    au->fft[2*i]=au->decode[i];     /* die Daten interleaved */
    au->fft[2*i+1]=0;               /* mit Nullen auffuellen */
  }
  fft(au->fft-1, AU_LEN, 1);        /* und los geht's ... */
}

void amp(int start, int stop, AU * au){
/* Amplituden von start bis stop berechnen */
  int i;

  for(i=start;i<=stop;i++){
    au->postfft[i]=sqrt(pow(au->fft[2*i],2.0)+pow(au->fft[2*i+1],2.0));
  }
}

void hamming(AU * au, double * input){
/* Werte fuer nninp mittels Hamming-Fenster berechnen*/
  int i=0, j=0, k=0;
  Hamming * ham;
  int winsize;
  double sum=0;

  ham=au->hamming;            /* Abkuerzung */
  
  for(i=0;i<ham->nb_hwin;i++){   /* Initale Fenstergroesse berechnen */
    sum+=pow(ham->fac,(double)i);
  }
  winsize=(int)floor((double)(ham->stop - ham->start+1)/((1 - ham->rednc)*(sum)));

  k=ham->start;                                    /* Von start bis ende */
  for(i=0;i<ham->nb_hwin;i++){                     /* die Werte ueber */
    for(j=k;((j<k+winsize)&&(j<=ham->stop));j++){  /* die Hammingfenster */
      input[i]+=au->postfft[k]/(double)winsize;    /* summieren und durch */
    }                                              /* die Anzahl der */
    k=j-(int)floor((ham->rednc*(double)winsize));  /* Hammingfenster */
    winsize=(int)floor((double)winsize*ham->fac);  /* dividieren */
#ifdef DBGNORM
    printf("Fenster %d berechnet: %f\n",i, input[i]);
#endif
  }
}

char get_trans_func(MN * mn){
/* liefert die Transferfunktion in der letzten Schicht */

  return(mn->a_net->layer[mn->a_net->nb_layers-1]->trans);
}

double get_min(int len, NNInp * nninp){
/* liefert Minimun des Inputs */

  int i;
  double min;

  min=nninp->input[0];
  for(i=1;i<len;i++){
    if(min>nninp->input[i])
      min=nninp->input[i];
  }
  return(min);
}

double get_max(int len, NNInp * nninp){
/* liefert Maximun des Inputs */

  int i;
  double max;

  max=nninp->input[0];
  for(i=1;i<len;i++){
    if(max<nninp->input[i])
      max=nninp->input[i];
  }
  return(max);
}

void norm(MN * mn, NNInp * nninp){
/* normiert die Input-Werte auf Wertebereich der Transferfunktion */

  int i;
  double min, mid, max;
  double * inp;

  min=get_min(mn->hamming->nb_hwin, nninp);
  max=get_max(mn->hamming->nb_hwin, nninp);
  mid=(min+max)/2;
#ifdef DBGNORM
  printf("min=%f\tmid=%f\tmax=%f\n", min, mid, max);
#endif
  inp=nninp->input;
  switch(get_trans_func(mn)){
  case LIN:    /* wie tanh */
  case STEP1:  /* wie tanh */
  case TANH:
    for(i=0;i<mn->hamming->nb_hwin;i++){
      inp[i]=(inp[i]-mid)/abs((int)(max-mid));
#ifdef DBGNORM
      printf("Fenster %d normiert: %f\n",i, inp[i]);
#endif
    }
    break;
  case STEP0:  /* wie logi */
  case LOGI:
    for(i=0;i<mn->hamming->nb_hwin;i++){
      inp[i]=(inp[i]-min)/abs((int)(max-min));
#ifdef DBGNORM
      printf("Fenster %d normiert: %f\n",i, inp[i]);
#endif
    }
    break;
  }
}

void make_target(MN * mn, AU * au, Target *target){
/* Soll-Werte in nninp eintragen */

  switch(get_trans_func(mn)){
  case STEP1:
  case TANH:
    switch(au->target){
    case A_V:
      target->a[0]= 1; target->ei[0]=-1; target->ou[0]=-1;
      target->a[1]=-1; target->ei[1]=-1; target->ou[1]=-1;
      target->a[2]=-1; target->ei[2]= 1; target->ou[2]= 1;
      target->total[0]= 1;
      target->total[1]=-1;
      target->total[2]=-1;
      target->total[3]=-1;
      target->total[4]=-1;
      break;
    case E_V:
      target->a[0]=-1; target->ei[0]= 1; target->ou[0]=-1;
      target->a[1]=-1; target->ei[1]=-1; target->ou[1]=-1;
      target->a[2]= 1; target->ei[2]=-1; target->ou[2]= 1;
      target->total[0]=-1;
      target->total[1]= 1;
      target->total[2]=-1;
      target->total[3]=-1;
      target->total[4]=-1;
      break;
    case I_V:
      target->a[0]=-1; target->ei[0]=-1; target->ou[0]=-1;
      target->a[1]=-1; target->ei[1]= 1; target->ou[1]=-1;
      target->a[2]= 1; target->ei[2]=-1; target->ou[2]= 1;
      target->total[0]=-1;
      target->total[1]=-1;
      target->total[2]= 1;
      target->total[3]=-1;
      target->total[4]=-1;
      break;
    case O_V:
      target->a[0]=-1; target->ei[0]=-1; target->ou[0]= 1;
      target->a[1]= 1; target->ei[1]=-1; target->ou[1]=-1;
      target->a[2]=-1; target->ei[2]= 1; target->ou[2]=-1;
      target->total[0]=-1;
      target->total[1]=-1;
      target->total[2]=-1;
      target->total[3]= 1;
      target->total[4]=-1;
      break;
    case U_V:
      target->a[0]=-1; target->ei[0]=-1; target->ou[0]=-1;
      target->a[1]= 1; target->ei[1]=-1; target->ou[1]= 1;
      target->a[2]=-1; target->ei[2]= 1; target->ou[2]=-1;
      target->total[0]=-1;
      target->total[1]=-1;
      target->total[2]=-1;
      target->total[3]=-1;
      target->total[4]= 1;
      break;
    case K_V:
    default:
      target->a[0]=-1; target->ei[0]=-1; target->ou[0]=-1;
      target->a[1]=-1; target->ei[1]=-1; target->ou[1]=-1;
      target->a[2]=-1; target->ei[2]= 1; target->ou[2]= 1;
      target->total[0]=-1;
      target->total[1]=-1;
      target->total[2]=-1;
      target->total[3]=-1;
      target->total[4]=-1;
      break;
    }
    break;
  case LOGI:
  case STEP0:
  case LIN:
    switch(au->target){
    case A_V:
      target->a[0]=1; target->ei[0]=0; target->ou[0]=0;
      target->a[1]=0; target->ei[1]=0; target->ou[1]=0;
      target->a[2]=0; target->ei[2]=1; target->ou[2]=1;
      target->total[0]=1;
      target->total[1]=0;
      target->total[2]=0;
      target->total[3]=0;
      target->total[4]=0;
      break;
    case E_V:
      target->a[0]=0; target->ei[0]=1; target->ou[0]=0;
      target->a[1]=0; target->ei[1]=0; target->ou[1]=0;
      target->a[2]=1; target->ei[2]=0; target->ou[2]=1;
      target->total[0]=0;
      target->total[1]=1;
      target->total[2]=0;
      target->total[3]=0;
      target->total[4]=0;
      break;
    case I_V:
      target->a[0]=0; target->ei[0]=0; target->ou[0]=0;
      target->a[1]=0; target->ei[1]=1; target->ou[1]=0;
      target->a[2]=1; target->ei[2]=0; target->ou[2]=1;
      target->total[0]=0;
      target->total[1]=0;
      target->total[2]=1;
      target->total[3]=0;
      target->total[4]=0;
      break;
    case O_V:
      target->a[0]=0; target->ei[0]=0; target->ou[0]=1;
      target->a[1]=1; target->ei[1]=0; target->ou[1]=0;
      target->a[2]=0; target->ei[2]=1; target->ou[2]=0;
      target->total[0]=0;
      target->total[1]=0;
      target->total[2]=0;
      target->total[3]=1;
      target->total[4]=0;
      break;
    case U_V:
      target->a[0]=0; target->ei[0]=0; target->ou[0]=0;
      target->a[1]=1; target->ei[1]=0; target->ou[1]=1;
      target->a[2]=0; target->ei[2]=1; target->ou[2]=0;
      target->total[0]=0;
      target->total[1]=0;
      target->total[2]=0;
      target->total[3]=0;
      target->total[4]=1;
      break;
    case K_V:
    default:
      target->a[0]=0; target->ei[0]=0; target->ou[0]=0;
      target->a[1]=0; target->ei[1]=0; target->ou[1]=0;
      target->a[2]=0; target->ei[2]=1; target->ou[2]=1;
      target->total[0]=0;
      target->total[1]=0;
      target->total[2]=0;
      target->total[3]=0;
      target->total[4]=0;
      break;
    }
    break;
  }
}
