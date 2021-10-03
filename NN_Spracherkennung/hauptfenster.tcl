#!/usr/bin/wish
#!/usr/local/bin/wish

# Globale tcl-Variablen
# mn_changed: Netz veraendert und noch nicht gespeichert (yes/no)
set mn_changed "no"
set mn_exists "no"
set nb_layers0 3
set nb_layers1 3
set nb_layers2 3
set nb_hwin 3
set descr "Bitte geben Sie hier eine Beschreibung ein..."
set descr_size [string len $descr]
set eps 0.25
set start 1
set stop 4096
set rednc 25
set fac "1.500000"

set nb_neurons0_0 5
set nb_neurons0_1 5
set nb_neurons0_2 5
set nb_neurons0_3 5
set nb_neurons0_4 5
set nb_neurons0_5 5
set nb_neurons0_6 5
set nb_neurons0_7 5
set nb_neurons0_8 5
set nb_neurons0_9 5
set nb_neurons0_10 5
set nb_neurons0_11 5
set nb_neurons0_12 5
set nb_neurons0_13 5
set nb_neurons0_14 5
set nb_neurons0_15 5
set nb_neurons0_16 5
set nb_neurons0_17 5
set nb_neurons0_18 5
set nb_neurons0_19 5
set nb_neurons1_0 5
set nb_neurons1_1 5
set nb_neurons1_2 5
set nb_neurons1_3 5
set nb_neurons1_4 5
set nb_neurons1_5 5
set nb_neurons1_6 5
set nb_neurons1_7 5
set nb_neurons1_8 5
set nb_neurons1_9 5
set nb_neurons1_10 5
set nb_neurons1_11 5
set nb_neurons1_12 5
set nb_neurons1_13 5
set nb_neurons1_14 5
set nb_neurons1_15 5
set nb_neurons1_16 5
set nb_neurons1_17 5
set nb_neurons1_18 5
set nb_neurons1_19 5
set nb_neurons2_0 5
set nb_neurons2_1 5
set nb_neurons2_2 5
set nb_neurons2_3 5
set nb_neurons2_4 5
set nb_neurons2_5 5
set nb_neurons2_6 5
set nb_neurons2_7 5
set nb_neurons2_8 5
set nb_neurons2_9 5
set nb_neurons2_10 5
set nb_neurons2_11 5
set nb_neurons2_12 5
set nb_neurons2_13 5
set nb_neurons2_14 5
set nb_neurons2_15 5
set nb_neurons2_16 5
set nb_neurons2_17 5
set nb_neurons2_18 5
set nb_neurons2_19 5

set trans0_0 0
set trans0_1 0
set trans0_2 0
set trans0_3 0
set trans0_4 0
set trans0_5 0
set trans0_6 0
set trans0_7 0
set trans0_8 0
set trans0_9 0
set trans0_10 0
set trans0_11 0
set trans0_12 0
set trans0_13 0
set trans0_14 0
set trans0_15 0
set trans0_16 0
set trans0_17 0
set trans0_18 0
set trans0_19 0
set trans0_20 0
set trans1_0 0
set trans1_1 0
set trans1_2 0
set trans1_3 0
set trans1_4 0
set trans1_5 0
set trans1_6 0
set trans1_7 0
set trans1_8 0
set trans1_9 0
set trans1_10 0
set trans1_11 0
set trans1_12 0
set trans1_13 0
set trans1_14 0
set trans1_15 0
set trans1_16 0
set trans1_17 0
set trans1_18 0
set trans1_19 0
set trans1_20 0
set trans2_0 0
set trans2_1 0
set trans2_2 0
set trans2_3 0
set trans2_4 0
set trans2_5 0
set trans2_6 0
set trans2_7 0
set trans2_8 0
set trans2_9 0
set trans2_10 0
set trans2_11 0
set trans2_12 0
set trans2_13 0
set trans2_14 0
set trans2_15 0
set trans2_16 0
set trans2_17 0
set trans2_18 0
set trans2_19 0
set trans2_20 0

# set netz_neub ""
# set netz_parameterb ""

source procedures.tcl
source unterfenster.tcl

# foreach i { 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 } {
#     set nb_neurons0_$i 5
#     set nb_neurons1_$i 5
#     set nb_neurons2_$i 5
#     set trans0_$i 0
#     set trans1_$i 0
#     set trans2_$i 0
# }

wm title . "Vokalerkennung"
wm iconname . "Vokalerkennung"
wm minsize . "20" "8"

#Toplevelfenster horizontal in zwei Frames einteilen
set lframe [frame .lframe]
set rframe [frame .rframe]
pack $lframe -side left -fill both -expand true
pack $rframe -side left -fill both -expand true

#Linkes Toplevelframe vertikal in drei Frames einteilen
set netzwerkf [frame $lframe.netzwerkf -borderwidth 1]
pack $netzwerkf -side top -fill both -expand true
set grafikf [frame $lframe.grafikf -borderwidth 1]
pack $grafikf -side top -fill both
set kommandof [frame $lframe.kommandof -borderwidth 1]
pack $kommandof -side top -fill both

#Rechtes Toplevelframe vertikal in zwei Frames einteilen
set adateif [frame $rframe.adateif -borderwidth 1]
pack $adateif -side top -fill both -expand true
set audioeditf [frame $rframe.audiorecf]
pack $audioeditf -side top -fill both

#Frame oben links vertikal in drei Frames einteilen
set unetzwerkf [frame $netzwerkf.unetzwerkf]
set mnetzwerkf [frame $netzwerkf.mnetzwerkf]
set bnetzwerkf [frame $netzwerkf.bnetzwerkf]
pack $unetzwerkf -side top -fill both -expand true
pack $mnetzwerkf -side top -fill x
pack $bnetzwerkf -side top -fill x

ScrolledListbox $unetzwerkf -width 20 -height 8 -setgrid true
$unetzwerkf.list insert end "Netz1" "3 Schichten" "1. Schicht: 3 Neuronen" "2. Schicht: 10 Neuronen" "3. Schicht: 5 Neuronen" "untrainiert"

set netz_ladenb [button $mnetzwerkf.netz_ladenb -text "Laden" -command "load_net ." -width 7]
set netz_speichernb [button $mnetzwerkf.netz_speichernb -text "Speichern" -command "save_net ." -width 7 -state disabled]
set netz_neub [button $bnetzwerkf.netz_neub -text "Neu" -command "set_new_net" -width 7]
set netz_parameterb [button $bnetzwerkf.netz_parameterb -text "Parameter" -command "set_net ." -width 7 -state disabled]
pack $netz_ladenb -side left -fill x -expand true
pack $netz_speichernb -side left -fill x -expand true
pack $netz_neub -side left -fill x -expand true
pack $netz_parameterb -side left -fill x -expand true

#Frame oben rechts vertikal in zwei Frames einteilen
set uadateif [frame $adateif.uadateif -borderwidth 1]
set badateif [frame $adateif.badateif -borderwidth 1]
pack $uadateif -side top -fill both -expand true
pack $badateif -side top -fill both

#Frame oben rechts oben horizontal in fuenf Frames einteilen
set 1uadateif [frame $uadateif.1uadateif]
set 2uadateif [frame $uadateif.2uadateif]
set 3uadateif [frame $uadateif.3uadateif]
set 4uadateif [frame $uadateif.4uadateif]
set 5uadateif [frame $uadateif.5uadateif]
pack $1uadateif -side left -fill both -expand true
pack $2uadateif -side left -fill both
pack $3uadateif -side left -fill both -expand true
pack $4uadateif -side left -fill both
pack $5uadateif -side left -fill both -expand true

set u2uadateif [frame $2uadateif.u2uadateif]
set b2uadateif [frame $2uadateif.b2uadateif]
set u4uadateif [frame $4uadateif.u4uadateif]
set b4uadateif [frame $4uadateif.b4uadateif]
pack $u2uadateif -fill both -expand true
pack $b2uadateif -fill both -expand true
pack $u4uadateif -fill both -expand true
pack $b4uadateif -fill both -expand true

# Listboxes und Buttons fuer Audiodateien erstellen
ScrolledListbox $1uadateif -width 5 -height 5 -setgrid true

set von_trainb [button $u2uadateif.von_trainb -text " > " -command "rem_train" -padx 0]
set zu_trainb [button $b2uadateif.zu_trainb -text " < " -command "to_train" -padx 0]
pack $von_trainb -side bottom
pack $zu_trainb -side top

ScrolledListbox $3uadateif -width 5 -height 5 -setgrid true

set zu_testb [button $u4uadateif.zu_testb -text " > " -command "to_test" -padx 0]
set von_testb [button $b4uadateif.von_testb -text " < " -command "rem_test" -padx 0]
pack $zu_testb -side bottom
pack $von_testb -side top

ScrolledListbox $5uadateif -width 5 -height 5 -setgrid true

#Frame oben rechts unten horizontal in drei Frames aufteilen
set lbadateif [frame $badateif.lbadateif]
set mbadateif [frame $badateif.mbadateif]
set rbadateif [frame $badateif.rbadateif]
pack $lbadateif -side left -fill x -expand true
pack $mbadateif -side left -fill both -expand true
pack $rbadateif -side left -fill x -expand true

# Dateibuttons erstellen
set train_datei_hinzub [button $lbadateif.train_datei_hinzub -text "Datei hinzufuegen" -command "add_train_file" -width 17]
pack $train_datei_hinzub -side top -fill x -expand true
set train_datei_entfb [button $lbadateif.datei_entfb -text "Datei entfernen" -command "rem_train_file" -width 17]
pack $train_datei_entfb -side top -fill x -expand true
set train_set_hinzub [button $lbadateif.train_set_hinzub -text "Set hinzufuegen" -command "add_train_set" -width 17]
pack $train_set_hinzub -side top -fill x -expand true
set train_set_entfb [button $lbadateif.train_set_entfb -text "Set entfernen" -command "rem_train_set" -width 17]
pack $train_set_entfb -side top -fill x -expand true
set train_liste_loeschenb [button $lbadateif.train_liste_loeschenb -text "Liste loeschen" -command "clear_train_list" -width 17]
pack $train_liste_loeschenb -side top -fill x -expand true

set datei_mixb [button $mbadateif.datei_mixb -text "Mix" -command "mix_files" -width 17]
set verteilmb [menubutton $mbadateif.mb -text Verteilen -menu $mbadateif.mb.menu -relief ridge]
set m [menu $verteilmb.menu -tearoff false]
$m add command -label "90/10" -command "auto90"
$m add command -label "80/20" -command "auto80"
$m add command -label "70/30" -command "auto70"
$m add command -label "60/40" -command "auto60"
pack $datei_mixb -side top -fill x
pack $verteilmb -side top -fill x

set test_datei_hinzub [button $rbadateif.test_datei_hinzub -text "Datei hinzufuegen" -command "add_test_file" -width 17]
pack $test_datei_hinzub -side top -fill x -expand true
set test_datei_entfb [button $rbadateif.test_datei_entfb -text "Datei entfernen" -command "rem_test_file" -width 17]
pack $test_datei_entfb -side top -fill x -expand true
set test_set_hinzub [button $rbadateif.test_set_hinzub -text "Set hinzufuegen" -command "add_test_set" -width 17]
pack $test_set_hinzub -side top -fill x -expand true
set test_set_entfb [button $rbadateif.test_set_entfb -text "Set entfernen" -command "rem_test_set" -width 17]
pack $test_set_entfb -side top -fill x -expand true
set test_liste_loeschenb [button $rbadateif.test_liste_loeschenb -text "Liste loeschen" -command "clear_test_list" -width 17]
pack $test_liste_loeschenb -side top -fill x -expand true

#Audioeditframe teilen fuer Ende-Button
set laudioeditf [frame $audioeditf.laudioeditf]
set raudioeditf [frame $audioeditf.raudioeditf -borderwidth 3]
pack $laudioeditf -side left -fill x -expand true
pack $raudioeditf -side left

# Audiobearbeitungsframe
set audioeditb [button $laudioeditf.audioeditb -text "Audiobearbeitung" -command "audioedit" -padx 50]
pack $audioeditb

set endeb [button $raudioeditf.endeb -text "Ende" -command ende -width 5 -background grey]
pack $endeb

# grafik-Frame teilen
set ugrafikf [frame $grafikf.ugrafikf]
set mgrafikf [frame $grafikf.mgrafikf]
set bgrafikf [frame $grafikf.bgrafikf]
pack $ugrafikf -side top -fill x -expand true
pack $mgrafikf -side top -fill x -expand true
pack $bgrafikf -side top -fill x -expand true

set grafikl [label $ugrafikf.grafikl -text "Grafiken"]
pack $grafikl -side left

set audiodateib [button $mgrafikf.audiodateib -text "Sample" -command "view_au" -width 6]
set hamb [button $mgrafikf.hamb -text "Hamming" -command "view_hamm" -width 6]
set gewschwb [button $bgrafikf.gewschwb -text "Netz" -command "view_net" -width 3]
set fftb [button $bgrafikf.fftb -text "FFT" -command "view_fft" -width 3]
set fehlerkurveb [button $bgrafikf.fehlerkurveb -text "Fehler" -command "view_error" -width 6]
pack $audiodateib -side left -fill x -expand true
pack $hamb -side left -fill x -expand true
pack $gewschwb -side left -fill x -expand true
pack $fftb -side left -fill x -expand true
pack $fehlerkurveb -side left -fill x -expand true


#Kommando-Frame teilen
set ukommandof [frame $kommandof.ukommandof]
set mkommandof [frame $kommandof.mkommandof]
set bkommandof [frame $kommandof.bkommandof]
pack $ukommandof -side top -fill x -expand true
pack $mkommandof -side top -fill x -expand true
pack $bkommandof -side top -fill x -expand true

set netzl [label $ukommandof.netzl -text "Netz"]
pack $netzl -side left

set nn_traintestb [button $mkommandof.nn_traintestb -text "train & test" -command "nn_traintest" -width 8]
pack $nn_traintestb -side left -fill x -expand true

set nn_livetestb [button $mkommandof.nn_livetestb -text "Live-Test" -command "nn_livetest" -width 8]
pack $nn_livetestb -side left -fill x -expand true

set nn_trainb [button $bkommandof.trainb -text "train" -command "nn_train" -width 4]
pack $nn_trainb -side left -fill x -expand true

set nn_testb [button $bkommandof.nn_testb -text "test" -command "nn_test" -width 4]
pack $nn_testb -side left -fill x -expand true

