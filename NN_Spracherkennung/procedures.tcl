proc fileDialog {w operation} {
    #   Type names		Extension(s)	Mac File Type(s)
    #
    #---------------------------------------------------------
    set types {
	{"Netz-Dateien"         {.mnet}         }
	{"All files"		*}
    }
    if {$operation == "open"} {
	set file [tk_getOpenFile -filetypes $types -parent $w \
		      -defaultextension .mnet]
	if [string compare $file ""] {
	    c_load_net $file
	} else {
	    return -1
	}
    } else {
	set file [tk_getSaveFile -filetypes $types -parent $w \
		      -initialfile Untitled -defaultextension .mnet]
	if [string compare $file ""] {
	    c_save_net $file
	} else {
	    return -1
	}
    }
}

proc ScrolledListbox { parent args } {
  eval { listbox $parent.list \
    -yscrollcommand [list $parent.sy set] \
    -xscrollcommand [list $parent.pad.sx set]} $args
  scrollbar $parent.sy -orient vertical -command [list $parent.list yview]
  frame $parent.pad
  scrollbar $parent.pad.sx -orient horizontal -command [list $parent.list xview]
  set pad [expr [$parent.sy cget -width] + 2* \
    ([$parent.sy cget -bd] + \
    [$parent.sy cget -highlightthickness])]
  pack $parent.pad -side bottom -fill x
  pack $parent.pad.sx -side bottom -fill x
  pack $parent.sy -side right -fill y
  pack $parent.list -side left -fill both -expand true
  return $parent.list
}

proc load_net {w} {
    global mn_exists
    global mn_changed
    global netz_neub
    global netz_parameterb
    global netz_speichernb

    if {$mn_changed == "yes"} {
	set answer [tk_messageBox -message "Netz ist noch nicht gespeichert! Moechten Sie es vor dem Laden eines neuen Netzes speichern?" -type yesno -icon question]
	if {$answer == "yes"} {
	    set result [save_net .]
	    if {$result == "-1"} {
		return
	    }
	    set mn_changed "no"
	}
    }

    set result [fileDialog $w "open"]
    if {$result != "-1"} {
	set mn_exists "yes"
# 	$netz_neub config -state disabled
	$netz_parameterb config -state normal
	$netz_speichernb config -state normal
	return $result
    }
}

proc save_net { w } {
    global mn_changed
    global mn_exists

    if { $mn_exists == "yes" } {
	set result [fileDialog $w "save"]
	if {$result != "-1"} {
	    set mn_changed "no"
	}
    }
}

proc set_new_net {} {
    global mn_exists
    global mn_changed
    global netz_neub
    global netz_parameterb
    global netz_speichernb
    
    if {$mn_exists == "no"} {
	new_net
    } else {
	
	if { $mn_changed == "yes" } {
	    set answer [tk_messageBox -message "Netz ist noch nicht gespeichert! Moechten Sie es vor dem Laden eines neuen Netzes speichern?" -type yesno -icon question]
	    if {$answer == "yes"} {
		set result [save_net .]
		if {$result == "-1"} {
		    puts stdout "Speichern fehlgeschlagen"
		    return
		}
		set mn_changed "no"
	    }
	}
	new_net
    }
}

proc set_net { w } {
    global mn_exists
    global mn_changed
    
#     if {$mn_exists == "yes" && $mn_changed == "yes"} {
# 	set answer [tk_messageBox -message "Das Aendern der Netzparameter initialisiert das Netz neu! Moechten Sie es vor dem Initialisieren speichern?" -type yesno -icon question]
# 	if {$answer == "yes"} {
# 	    set result [save_net .]
# 	    if {$result == "-1"} {
# 		puts stdout "Speichern fehlgeschlagen"
# 		return
# 	    }
# 	    set mn_changed "no"
# 	}
# 	new_net
# 	set mn_changed "yes"
#     }
#     if {$mn_exists == "yes" && $mn_changed == "no"} {
# 	set answer [tk_messageBox -message "Das Aendern der Netzparameter initialisiert das Netz neu! Weiter?" -type yesno -icon question]
# 	if {$answer == "yes"} {
# 	    new_net
# 	    set mn_changed "yes"
# 	}
#     }
    show_param $w

}

proc rem_train {} {
}

proc to_train {} {
}

proc to_test {} {
}

proc rem_test {} {
}

proc add_train_file {} {
}

proc rem_train_file {} {
}

proc add_train_set {} {
}

proc rem_train_set {} {
}

proc clear_train_list {} {
}

proc mix_files {} {
}

proc auto90 {} {
}

proc auto80 {} {
}

proc auto70 {} {
}

proc auto60 {} {
}

proc add_test_file {} {
}

proc rem_test_file {} {
}

proc add_test_set {} {
}

proc rem_test_set {} {
}

proc clear_test_list {} {
}

proc audioedit {} {
}

proc view_au {} {
}

proc view_hamm {} {
}

proc view_net {} {
}

proc view_fft {} {
}

proc view_error {} {
}

proc nn_traintest {} {
}

proc nn_livetest {} {
}

proc nn_train {} {
    c_nn_train
}

proc nn_test {} {
}

proc ende {} {
    global mn_changed
    if {$mn_changed == "yes"} {
	set answer [tk_messageBox -message "Netz ist noch nicht gespeichert! \
Moechten Sie es vor dem Beenden des Programmes speichern?" -type yesno -icon question]
	if {$answer == "yes"} {
	    set result [save_net .]
	    if {$result == "-1"} {
		return
	    }
	}
    }
    c_ende
    {exit}
}

proc grab_up { par win } {
    destroy $win
    grab set $par
}

proc generate_net { win } {
    global nb_layers0
    global nb_neurons0_0
    global nb_neurons0_1
    global nb_neurons0_2
    global nb_neurons0_3
    global nb_neurons0_4
    global nb_neurons0_5
    global nb_neurons0_6
    global nb_neurons0_7
    global nb_neurons0_8
    global nb_neurons0_9
    global nb_neurons0_10
    global nb_neurons0_11
    global nb_neurons0_12
    global nb_neurons0_13
    global nb_neurons0_14
    global nb_neurons0_15
    global nb_neurons0_16
    global nb_neurons0_17
    global nb_neurons0_18
    global nb_neurons0_19
    global trans0_0
    global trans0_1
    global trans0_2
    global trans0_3
    global trans0_4
    global trans0_5
    global trans0_6
    global trans0_7
    global trans0_8
    global trans0_9
    global trans0_10
    global trans0_11
    global trans0_12
    global trans0_13
    global trans0_14
    global trans0_15
    global trans0_16
    global trans0_17
    global trans0_18
    global trans0_19
    global trans0_20
    global nb_neurons1_0
    global nb_neurons1_1
    global nb_neurons1_2
    global nb_neurons1_3
    global nb_neurons1_4
    global nb_neurons1_5
    global nb_neurons1_6
    global nb_neurons1_7
    global nb_neurons1_8
    global nb_neurons1_9
    global nb_neurons1_10
    global nb_neurons1_11
    global nb_neurons1_12
    global nb_neurons1_13
    global nb_neurons1_14
    global nb_neurons1_15
    global nb_neurons1_16
    global nb_neurons1_17
    global nb_neurons1_18
    global nb_neurons1_19
    global trans1_0
    global trans1_1
    global trans1_2
    global trans1_3
    global trans1_4
    global trans1_5
    global trans1_6
    global trans1_7
    global trans1_8
    global trans1_9
    global trans1_10
    global trans1_11
    global trans1_12
    global trans1_13
    global trans1_14
    global trans1_15
    global trans1_16
    global trans1_17
    global trans1_18
    global trans1_19
    global trans1_20
    global nb_neurons2_0
    global nb_neurons2_1
    global nb_neurons2_2
    global nb_neurons2_3
    global nb_neurons2_4
    global nb_neurons2_5
    global nb_neurons2_6
    global nb_neurons2_7
    global nb_neurons2_8
    global nb_neurons2_9
    global nb_neurons2_10
    global nb_neurons2_11
    global nb_neurons2_12
    global nb_neurons2_13
    global nb_neurons2_14
    global nb_neurons2_15
    global nb_neurons2_16
    global nb_neurons2_17
    global nb_neurons2_18
    global nb_neurons2_19
    global trans2_0
    global trans2_1
    global trans2_2
    global trans2_3
    global trans2_4
    global trans2_5
    global trans2_6
    global trans2_7
    global trans2_8
    global trans2_9
    global trans2_10
    global trans2_11
    global trans2_12
    global trans2_13
    global trans2_14
    global trans2_15
    global trans2_16
    global trans2_17
    global trans2_18
    global trans2_19
    global trans2_20

#     puts stdout $nb_neurons0_0
#     puts stdout $nb_neurons0_1
#     puts stdout $nb_neurons0_2
#     puts stdout $nb_neurons0_3
#     puts stdout $nb_neurons0_4
#     puts stdout $nb_neurons0_5
#     puts stdout $nb_neurons0_6
#     puts stdout $nb_neurons0_7
#     puts stdout $nb_neurons0_8
#     puts stdout $nb_neurons0_9
#     puts stdout $nb_neurons0_10
#     puts stdout $nb_neurons0_11
#     puts stdout $nb_neurons0_12
#     puts stdout $nb_neurons0_13
#     puts stdout $nb_neurons0_14
#     puts stdout $nb_neurons0_15
#     puts stdout $nb_neurons0_16
#     puts stdout $nb_neurons0_17
#     puts stdout $nb_neurons0_18
#     puts stdout $nb_neurons0_19
#     puts stdout $nb_neurons1_0
#     puts stdout $nb_neurons1_1
#     puts stdout $nb_neurons1_2
#     puts stdout $nb_neurons1_3
#     puts stdout $nb_neurons1_4
#     puts stdout $nb_neurons1_5
#     puts stdout $nb_neurons1_6
#     puts stdout $nb_neurons1_7
#     puts stdout $nb_neurons1_8
#     puts stdout $nb_neurons1_9
#     puts stdout $nb_neurons1_10
#     puts stdout $nb_neurons1_11
#     puts stdout $nb_neurons1_12
#     puts stdout $nb_neurons1_13
#     puts stdout $nb_neurons1_14
#     puts stdout $nb_neurons1_15
#     puts stdout $nb_neurons1_16
#     puts stdout $nb_neurons1_17
#     puts stdout $nb_neurons1_18
#     puts stdout $nb_neurons1_19
#     puts stdout $nb_neurons2_0
#     puts stdout $nb_neurons2_1
#     puts stdout $nb_neurons2_2
#     puts stdout $nb_neurons2_3
#     puts stdout $nb_neurons2_4
#     puts stdout $nb_neurons2_5
#     puts stdout $nb_neurons2_6
#     puts stdout $nb_neurons2_7
#     puts stdout $nb_neurons2_8
#     puts stdout $nb_neurons2_9
#     puts stdout $nb_neurons2_10
#     puts stdout $nb_neurons2_11
#     puts stdout $nb_neurons2_12
#     puts stdout $nb_neurons2_13
#     puts stdout $nb_neurons2_14
#     puts stdout $nb_neurons2_15
#     puts stdout $nb_neurons2_16
#     puts stdout $nb_neurons2_17
#     puts stdout $nb_neurons2_18
#     puts stdout $nb_neurons2_19

#     puts stdout $trans0_0
#     puts stdout $trans0_1
#     puts stdout $trans0_2
#     puts stdout $trans0_3
#     puts stdout $trans0_4
#     puts stdout $trans0_5
#     puts stdout $trans0_6
#     puts stdout $trans0_7
#     puts stdout $trans0_8
#     puts stdout $trans0_9
#     puts stdout $trans0_10
#     puts stdout $trans0_11
#     puts stdout $trans0_12
#     puts stdout $trans0_13
#     puts stdout $trans0_14
#     puts stdout $trans0_15
#     puts stdout $trans0_16
#     puts stdout $trans0_17
#     puts stdout $trans0_18
#     puts stdout $trans0_19
#     puts stdout $trans0_20
#     puts stdout $trans1_0
#     puts stdout $trans1_1
#     puts stdout $trans1_2
#     puts stdout $trans1_3
#     puts stdout $trans1_4
#     puts stdout $trans1_5
#     puts stdout $trans1_6
#     puts stdout $trans1_7
#     puts stdout $trans1_8
#     puts stdout $trans1_9
#     puts stdout $trans1_10
#     puts stdout $trans1_11
#     puts stdout $trans1_12
#     puts stdout $trans1_13
#     puts stdout $trans1_14
#     puts stdout $trans1_15
#     puts stdout $trans1_16
#     puts stdout $trans1_17
#     puts stdout $trans1_18
#     puts stdout $trans1_19
#     puts stdout $trans1_20
#     puts stdout $trans2_0
#     puts stdout $trans2_1
#     puts stdout $trans2_2
#     puts stdout $trans2_3
#     puts stdout $trans2_4
#     puts stdout $trans2_5
#     puts stdout $trans2_6
#     puts stdout $trans2_7
#     puts stdout $trans2_8
#     puts stdout $trans2_9
#     puts stdout $trans2_10
#     puts stdout $trans2_11
#     puts stdout $trans2_12
#     puts stdout $trans2_13
#     puts stdout $trans2_14
#     puts stdout $trans2_15
#     puts stdout $trans2_16
#     puts stdout $trans2_17
#     puts stdout $trans2_18
#     puts stdout $trans2_19
#     puts stdout $trans2_20

    global nb_layers0
    global nb_layers1
    global nb_layers2
    global descr
    global descr_size
    global nb_hwin
    global eps
    global start
    global stop
    global rednc
    global fac

    set descr_size [string len $descr]

#     puts stdout "Werte in TCL"
#     puts stdout "nb_layers0"
#     puts stdout $nb_layers0
#     puts stdout "nb_layers1"
#     puts stdout $nb_layers1
#     puts stdout "nb_layers2"
#     puts stdout $nb_layers2
#     puts stdout "descr"
#     puts stdout $descr
#     puts stdout "descr_size"
#     puts stdout $descr_size
#     puts stdout "nb_hwin"
#     puts stdout $nb_hwin
#     puts stdout "eps"
#     puts stdout $eps
#     puts stdout "start"
#     puts stdout $start
#     puts stdout "stop"
#     puts stdout $stop
#     puts stdout "rednc"
#     puts stdout $rednc
#     puts stdout "fac"
#     puts stdout $fac
#     puts stdout "Rufe c_generate_net auf..."

    global mn_exists
    global mn_changed
    global netz_speichernb
    global netz_parameterb

    c_generate_net
    set mn_exists "yes"
    set mn_changed "yes"
    $netz_speichernb config -state normal
    $netz_parameterb config -state normal
    destroy $win
}
