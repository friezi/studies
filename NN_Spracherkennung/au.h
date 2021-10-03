#ifndef AU_H
#define AU_H

char * change_order(char *i, int len);
/* kehrt die Reihenfolge der Bytes um */

bool check_au_type(AU * au);
/* checken, ob wirklich au-file */

int get_pos_max(AU * au);
/* ermittelt Position mit groesster Amplitude */

void cut_au(AU * au);
/* schneidet aus au-file auf 8192 Samples zurecht */

bool check_file_length(AU * au);
/* Laenge checken und gegebenenfalls */

void semi_invert(AU * au);
/* au-Rohdaten semiinvertieren */

void au_amp(AU * au);
/* Amplituden aus semiinvertierten au-Daten errechnen */

bool decode(AU * au);
/* au-Format dekodieren */

void fft_func(AU * au);
/* Fast-Fourier-Transformation durchfuehren */

void amp(int start, int stop, AU * au);
/* Amplituden von start bis stop berechnen */

void hamming(AU * au, double * input);
/* Werte fuer nninp mittels Hamming-Fenster berechnen*/

char get_trans_func(MN * mn);
/* liefert die Transferfunktion in der letzten Schicht */

double get_min(int len, NNInp * nninp);
/* liefert Minimun des Inputs */

double get_max(int len, NNInp * nninp);
/* liefert Maximun des Inputs */

void norm(MN * mn, NNInp * nninp);
/* normiert die Input-Werte auf Wertebereich der Transferfunktion */

void make_target(MN * mn, AU * au, Target *target);
/* Soll-Werte in nninp eintragen */

#endif
