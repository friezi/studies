CC = gcc -Wall -ansi -pedantic -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align  -Waggregate-return -Wstrict-prototypes

CFLAGS = -I/usr/tcl/include -c -O3

# Moegliche Debug-Einstellungen
#
# -DDBGAUVV	Audiovorverarbeitung (in au_management.c)
# -DDBGAU	Funktionen auf au-Daten
# -DDBGNORM     Hamming und Normierung der Inputs
# -DDBGMNG      Netzmanagement
# -DDBGUP       Update
# -DDBGBACK     Backpropagation
# -DDBGPOSTP    Postprocessing
# -DDBGPROP     Propagation
# -DDBGSWB	set_weights_bias
# -DDBGSMN	Multinetz speichern
# -DDBGLMN	Multinetz laden
# -DDBGMMLP	MLP erschaffen
# -DDBGAUIO	au-Datei Lese-/Schreibfunktionen
# -DDBGGN	generate_net

# fuer Linux
# LDLIBS = -ltk8.0 -ltcl8.0 -lm

LDLIBS = -R/usr/tcl/lib -L/usr/tcl/lib -ltk -ltcl -lm  

all: mnet

mnet: net-mgmnt.o net.o fft.o au_management.o au.o fileio.o tcl_main.o tcl_interface.o
	$(CC) $(LDLIBS) net-mgmnt.o net.o fft.o au_management.o au.o fileio.o tcl_main.o tcl_interface.o -o mnet

net-mgmnt.o: net-mgmnt.c Makefile
	$(CC) $(CFLAGS) net-mgmnt.c -o net-mgmnt.o

net.o: net.c Makefile
	$(CC) $(CFLAGS) net.c -o net.o

fft.o: fft.c Makefile
	$(CC) $(CFLAGS) fft.c -o fft.o

au_management.o: au_management.c Makefile
	$(CC) $(CFLAGS) au_management.c -o au_management.o

au.o: au.c Makefile
	$(CC) $(CFLAGS) au.c -o au.o

tcl_main.o: tcl_main.c Makefile
	$(CC) $(CFLAGS) tcl_main.c -o tcl_main.o

tcl_interface.o: tcl_interface.c Makefile
	$(CC) $(CFLAGS) tcl_interface.c -o tcl_interface.o

#main.o: main.c Makefile
#	$(CC) $(CFLAGS) main.c -o main.o

fileio.o: fileio.c Makefile
	$(CC) $(CFLAGS) fileio.c -o fileio.o

clean:
	rm -f *.o *~

notcl: mnet2

mnet2: net-mgmnt.o net.o fft.o au_management.o au.o fileio.o main.o
	$(CC) $(LDLIBS) net-mgmnt.o net.o fft.o au_management.o au.o fileio.o main.o -o mnet2

main.o: main.c Makefile
	$(CC) $(CFLAGS) main.c -o main.o
