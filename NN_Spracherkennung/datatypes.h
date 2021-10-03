#ifndef DATATYPES_H
#define DATATYPES_H

#define NB_MLPS 3            /* Anzahl der MLP's */
#define AU_LEN 8192          /* Laenge des .au-Daten-Array */
#define FFT_LEN AU_LEN*2     /* Laenge des FFT-Array */
#define P_FFT_LEN AU_LEN/2   /* Laenge des Post-FFT-Array */
#define A_N 0                /* A-Netz */
#define EI_N 1               /* EI-NETZ */
#define OU_N 2               /* OU-Netz */
#define M_N 3                /* Multinetz */
#define TRAINFL (1<<0)       /* Train Flag */
#define TESTFL (1<<1)        /* Test Flag */
#define TANH 0               /* tanh */
#define LOGI 1               /* Die 'logische' :-) Funktion */
#define STEP0 2              /* Die Treppenfunktion R->{0,1} */
#define STEP1 3              /* Die Treppenfunktion R->{-1,0,1} */
#define LIN 4                /* Die lineare Abbildung mit k=1 */
#define A_V (1<<0)           /* Die Vokale */
#define E_V (1<<1)           /*            */
#define I_V (1<<2)           /*            */
#define O_V (1<<3)           /*            */
#define U_V (1<<4)           /*            */
#define K_V (1<<5)           /* kein Vokal */

#define WRONG (1<<0)
#define OK (1<<1)

typedef enum {false=0, true=!(false)} bool;   /* bool'scher Datentyp */

typedef enum {batch, online} trainmode;  /* Trainigsmodus des Netzes */

typedef struct{
  double a[3],ou[3],ei[3];   /* Sollwerte fuer die drei NN */
  double total[5];           /* Sollwerte fuer das MLP plus Entscheider */
}Target;

typedef struct{
  int start,stop;       /* Anfangs- und Endindex des Datenarray fuer Hamming */
  double rednc;         /* Redundanz der Hammingfenster */
  double fac;           /* Vergroesserungsfaktor der Hammingfenster */
  int nb_hwin;          /* Anzahl der Hammingfenster */
}Hamming;

typedef struct{
  int datalength;            /* Anzahl der Samples im .au-file */
  char *data;                /* Sampledaten */
  double decode[AU_LEN];     /* Array fuer decodierte und beschnittene Daten */
  double fft[FFT_LEN];       /* Array fuer Quelle und Ergebnis der FFT */
  double postfft[P_FFT_LEN]; /* Array fuer Amplituden der FFT */
  Hamming *hamming;         /* Zeiger auf Hammingstruktur */
  char target;               /* Beschreibung (a,e,i,o,u, kein Vokal) */
}AU;

typedef struct{
  double *input;             /* Eingangswerte fuer das MLP */
  Target target;             /* Zugehoerige target-Werte */
}NNInp;

typedef struct{
  double *weight;            /* Gewichte des Neurons */
  double *d_weight;          /* Delta-Gewichte des Neurons */
  double bias;               /* Schwellen des Neurons */
  double d_bias;             /* Delta-Schwellen des Neurons */
  double y;                  /* Wert des Neurons nach Transferfunktion */
  double delta;              /* Delta (aus Backprop) des Neurons */
}Neuron;

typedef struct{
  int nb_neurons;        /* Anzahl der Neuronen in der Schicht */
  Neuron **neuron;       /* Zeiger auf Neuronen-Adressen */
  char trans;            /* Transferfunktion der Schicht */
  int nb_weights;        /* Anzahl der Gewichte pro Neuron in dieser Schicht */
}Layer;

typedef struct ListElem{
  struct ListElem *next;     /* Zeiger auf naechstes Element */
  double data;               /* Datum des Elementes */
}ListElem;

typedef struct{
  int train_epochs;          /* Anzahl der Trainingsepochen */
  ListElem *firsttrain;      /* Zeiger auf erstes Element */
  int test_epochs;           /* Anzahl der durchlaufenen Testepochen */
  ListElem *firsttest;       /* Zeiger auf erstes Element */
}Stats;

typedef struct{
  int nb_layers;             /* Anzahl der Layer */
  Layer **layer;             /* Zeiger auf Layer-Adressen */
  double output[3];          /* Ausgabe des Netzes */
  Stats stats;               /* Statistische Werte */
}MLP;

typedef struct{
  MLP *a_net,*ei_net,*ou_net;   /* Drei MultiLayerNetworks */
  double output[5];          /* Ausgabe des MultiNetzes */
  Hamming *hamming;          /* Zeiger auf Hammingstruktur */
  Stats stats;               /* Statistische Werte */
  double eps;                /* Lernschrittweite */
  int epoch;                 /* Anzahl der Epochen */
  int descr_size;            /* Groesse der Description */
  char *descr;               /* Zeiger auf Description */
}MN;

typedef struct{
  int nb_files;             /* Anzahl der Dateien in einem Set */
  char *descr;              /* Beschreibung des Audiosets */
  char **files;             /* Zeiger auf Strings mit Namen der Dateien */
}AudioSet;

#endif
