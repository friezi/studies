proc new_net {} {
    set new_netw .new_netw
    toplevel $new_netw -borderwidth 1
    grab set $new_netw
    wm title $new_netw "Netz-Parameter"
    wm grid $new_netw 1 1 10 10
    set x [winfo x .]
    set y [winfo y .]
    set newx [expr $x + "25"]
    set newy [expr $y + "25"]
    wm geometry $new_netw +$newx+$newy

    set labelf [frame $new_netw.labelf -borderwidth 1]
    set netf [frame $new_netw.netf -borderwidth 1]
    set hammf [frame $new_netw.hammf -borderwidth 1]
    set okf [frame $new_netw.okf -borderwidth 1]
    pack $labelf -side top -fill x
    pack $netf -side top -fill x
    pack $hammf -side top -fill x
    pack $okf -side top -fill x

    global descr
    global descr_size
    set descl [label $labelf.descl -text "Netzbeschreibung:"]
    pack $descl -side left
    set desce [entry $labelf.desce -textvariable descr]
    pack $desce -side left -fill x -expand true
    

    set unetf [frame $netf.unetf]
    set bnetf [frame $netf.bnetf]
    pack $unetf -side top -fill x -expand true
    pack $bnetf -side top -fill x -expand true

    set a_netf [frame $unetf.a_netf]
    set ei_netf [frame $unetf.ei_netf]
    set ou_netf [frame $unetf.ou_netf]
    pack $a_netf -side left -fill x -expand true
    pack $ei_netf -side left -fill x -expand true
    pack $ou_netf -side left -fill x -expand true

    global nb_layers0
    global nb_layers1
    global nb_layers2
    set a_nb_layerss [scale $a_netf.a_nb_layerss -from 1 -to 20 \
	-variable nb_layers0 -orient horizontal -label "Zwischenschichten"]
    set ei_nb_layerss [scale $ei_netf.ei_nb_layerss -from 1 -to 20 \
	-variable nb_layers1 -orient horizontal -label "Zwischenschichten"]
    set ou_nb_layerss [scale $ou_netf.a_nb_layerss -from 1 -to 20 \
	-variable nb_layers2 -orient horizontal -label "Zwischenschichten"]
    pack $a_nb_layerss -side top -fill x -expand true
    pack $ei_nb_layerss -side top -fill x -expand true
    pack $ou_nb_layerss -side top -fill x -expand true

    set net_ab [button $a_netf.net_ab -text "A E/I O/U Netz" -width 11 -command "new_net_a $new_netw new"]
    set net_eib [button $ei_netf.net_eib -text "E/I Netz" -width 11 -command "new_net_ei $new_netw new"]
    set net_oub [button $ou_netf.net_oub -text "O/U Netz" -width 11 -command "new_net_ou $new_netw new"]
    pack $net_ab -side top -fill x -expand true
    pack $net_eib -side top -fill x -expand true
    pack $net_oub -side top -fill x -expand true

    set thammf [frame $hammf.thammf]
    set bhammf [frame $hammf.bhammf]
    pack $thammf -side top -fill x
    pack $bhammf -side top -fill x

    set lthammf [frame $thammf.lthammf]
    set rthammf [frame $thammf.rthammf]
    pack $lthammf -side left -fill x -expand true
    pack $rthammf -side left -fill x

    set urthammf [frame $rthammf.urthammf]
    set brthammf [frame $rthammf.brthammf]
    pack $urthammf -side top -fill x -expand true
    pack $brthammf -side top -fill x -expand true

    set 1rthammf [frame $brthammf.1rthammf]
    set 2rthammf [frame $brthammf.2rthammf]
    set 3rthammf [frame $brthammf.3rthammf]
    pack $1rthammf -side top -fill x -expand true
    pack $2rthammf -side top -fill x -expand true
    pack $3rthammf -side top -fill x -expand true

    global eps
    set epss [scale $bnetf.epss -from 0.001 -to 0.5 \
	-variable eps -orient horizontal -label "Lernschrittweite" -resolution 0.001]
    pack $epss -side top -fill x -expand true

    global nb_hwin
    set nb_hwins [scale $lthammf.nb_hwins -from 1 -to 20 \
	-variable nb_hwin -orient horizontal -label "Anzahl Hammingfenster"]
    pack $nb_hwins -side top -fill x -expand true

    global rednc
    set redncs [scale $lthammf.redncs -from 0 -to 95 \
	-variable rednc -orient horizontal -label "Redundanz (%)"]
    pack $redncs -side top -fill x -expand true

    set facl [label $urthammf.facl -text "Vergroesserungsfaktor" -width 19]
    pack $facl -side top

    global fac
    set facb0 [radiobutton $1rthammf.facb0 -text "7/6" -variable fac -value "1.166667"]
    set facb1 [radiobutton $1rthammf.facb1 -text "6/5" -variable fac -value "1.200000"]
    set facb2 [radiobutton $1rthammf.facb2 -text "5/4" -variable fac -value "1.250000"]
    set facb3 [radiobutton $2rthammf.facb3 -text "4/3" -variable fac -value "1.333333"]
    set facb4 [radiobutton $2rthammf.facb4 -text "7/5" -variable fac -value "1.400000"]
    set facb5 [radiobutton $2rthammf.facb5 -text "3/2" -variable fac -value "1.500000"]
    set facb6 [radiobutton $3rthammf.facb6 -text "5/3" -variable fac -value "1.666667"]
    set facb7 [radiobutton $3rthammf.facb7 -text "7/4" -variable fac -value "1.750000"]
    set facb8 [radiobutton $3rthammf.facb8 -text "5/2" -variable fac -value "2.500000"]
    pack $facb0 -side left
    pack $facb1 -side left
    pack $facb2 -side left
    pack $facb3 -side left
    pack $facb4 -side left
    pack $facb5 -side left
    pack $facb6 -side left
    pack $facb7 -side left
    pack $facb8 -side left

    global start
    set starts [scale $bhammf.starts -from 1 -to 4096 -variable start -length 600 \
 		       -orient horizontal -label "Startindex"]

    global stop
    set stops [scale $bhammf.stops -from 1 -to 4096 -variable stop -length 600 \
 		       -orient horizontal -label "Stopindex"]

    pack $starts -side top -fill x
    pack $stops -side top -fill x

    set okb [button $okf.okb -text "Ok" -command "generate_net $new_netw"]
    pack $okb -side left
    set cancelb [button $okf.cancelb -text "Abbrechen" -command "destroy $new_netw"]
    pack $cancelb -side right

}

proc show_param { par } {
    set new_netw .new_netw
    toplevel $new_netw -borderwidth 1
    grab set $new_netw
    wm title $new_netw "Netz-Parameter"
    wm grid $new_netw 1 1 10 10
    set x [winfo x .]
    set y [winfo y .]
    set newx [expr $x + "25"]
    set newy [expr $y + "25"]
    wm geometry $new_netw +$newx+$newy

    set labelf [frame $new_netw.labelf -borderwidth 1]
    set netf [frame $new_netw.netf -borderwidth 1]
    set hammf [frame $new_netw.hammf -borderwidth 1]
    set okf [frame $new_netw.okf -borderwidth 1]
    pack $labelf -side top -fill x
    pack $netf -side top -fill x
    pack $hammf -side top -fill x
    pack $okf -side top -fill x

    global descr
    global descr_size
    set descl [label $labelf.descl -text "Netzbeschreibung:"]
    pack $descl -side left
    set desce [entry $labelf.desce -textvariable descr]
    pack $desce -side left -fill x -expand true
    $desce config -state disabled
    

    set unetf [frame $netf.unetf]
    set bnetf [frame $netf.bnetf]
    pack $unetf -side top -fill x -expand true
    pack $bnetf -side top -fill x -expand true

    set a_netf [frame $unetf.a_netf]
    set ei_netf [frame $unetf.ei_netf]
    set ou_netf [frame $unetf.ou_netf]
    pack $a_netf -side left -fill x -expand true
    pack $ei_netf -side left -fill x -expand true
    pack $ou_netf -side left -fill x -expand true

    global nb_layers0
    global nb_layers1
    global nb_layers2
    set a_nb_layerss [scale $a_netf.a_nb_layerss -from 1 -to 20 \
	-variable nb_layers0 -orient horizontal -label "Zwischenschichten"]
    set ei_nb_layerss [scale $ei_netf.ei_nb_layerss -from 1 -to 20 \
	-variable nb_layers1 -orient horizontal -label "Zwischenschichten"]
    set ou_nb_layerss [scale $ou_netf.a_nb_layerss -from 1 -to 20 \
	-variable nb_layers2 -orient horizontal -label "Zwischenschichten"]
    pack $a_nb_layerss -side top -fill x -expand true
    $a_nb_layerss config -state disabled
    pack $ei_nb_layerss -side top -fill x -expand true
    $ei_nb_layerss config -state disabled
    pack $ou_nb_layerss -side top -fill x -expand true
    $ou_nb_layerss config -state disabled

    set net_ab [button $a_netf.net_ab -text "A E/I O/U Netz" -width 11 -command "new_net_a $new_netw show"]
    set net_eib [button $ei_netf.net_eib -text "E/I Netz" -width 11 -command "new_net_ei $new_netw show"]
    set net_oub [button $ou_netf.net_oub -text "O/U Netz" -width 11 -command "new_net_ou $new_netw show"]
    pack $net_ab -side top -fill x -expand true
    pack $net_eib -side top -fill x -expand true
    pack $net_oub -side top -fill x -expand true

    set thammf [frame $hammf.thammf]
    set bhammf [frame $hammf.bhammf]
    pack $thammf -side top -fill x
    pack $bhammf -side top -fill x

    set lthammf [frame $thammf.lthammf]
    set rthammf [frame $thammf.rthammf]
    pack $lthammf -side left -fill x -expand true
    pack $rthammf -side left -fill x

    set urthammf [frame $rthammf.urthammf]
    set brthammf [frame $rthammf.brthammf]
    pack $urthammf -side top -fill x -expand true
    pack $brthammf -side top -fill x -expand true

    set 1rthammf [frame $brthammf.1rthammf]
    set 2rthammf [frame $brthammf.2rthammf]
    set 3rthammf [frame $brthammf.3rthammf]
    pack $1rthammf -side top -fill x -expand true
    pack $2rthammf -side top -fill x -expand true
    pack $3rthammf -side top -fill x -expand true

    global eps
    set epss [scale $bnetf.epss -from 0.001 -to 0.5 \
	-variable eps -orient horizontal -label "Lernschrittweite" -resolution 0.001]
    pack $epss -side top -fill x -expand true
    $epss config -state disabled

    global nb_hwin
    set nb_hwins [scale $lthammf.nb_hwins -from 1 -to 20 \
	-variable nb_hwin -orient horizontal -label "Anzahl Hammingfenster"]
    pack $nb_hwins -side top -fill x -expand true
    $nb_hwins config -state disabled

    global rednc
    set redncs [scale $lthammf.redncs -from 0 -to 95 \
	-variable rednc -orient horizontal -label "Redundanz (%)"]
    pack $redncs -side top -fill x -expand true
    $redncs config -state disabled

    set facl [label $urthammf.facl -text "Vergroesserungsfaktor" -width 19]
    pack $facl -side top

    global fac
    set facb0 [radiobutton $1rthammf.facb0 -text "7/6" -variable fac -value "1.166667"]
    set facb1 [radiobutton $1rthammf.facb1 -text "6/5" -variable fac -value "1.200000"]
    set facb2 [radiobutton $1rthammf.facb2 -text "5/4" -variable fac -value "1.250000"]
    set facb3 [radiobutton $2rthammf.facb3 -text "4/3" -variable fac -value "1.333333"]
    set facb4 [radiobutton $2rthammf.facb4 -text "7/5" -variable fac -value "1.400000"]
    set facb5 [radiobutton $2rthammf.facb5 -text "3/2" -variable fac -value "1.500000"]
    set facb6 [radiobutton $3rthammf.facb6 -text "5/3" -variable fac -value "1.666667"]
    set facb7 [radiobutton $3rthammf.facb7 -text "7/4" -variable fac -value "1.750000"]
    set facb8 [radiobutton $3rthammf.facb8 -text "5/2" -variable fac -value "2.500000"]
    pack $facb0 -side left
    pack $facb1 -side left
    pack $facb2 -side left
    pack $facb3 -side left
    pack $facb4 -side left
    pack $facb5 -side left
    pack $facb6 -side left
    pack $facb7 -side left
    pack $facb8 -side left
    $facb0 config -state disabled
    $facb1 config -state disabled
    $facb2 config -state disabled
    $facb3 config -state disabled
    $facb4 config -state disabled
    $facb5 config -state disabled
    $facb6 config -state disabled
    $facb7 config -state disabled
    $facb8 config -state disabled

    global start
    set starts [scale $bhammf.starts -from 1 -to 4096 -variable start -length 600 \
 		       -orient horizontal -label "Startindex"]

    global stop
    set stops [scale $bhammf.stops -from 1 -to 4096 -variable stop -length 600 \
 		       -orient horizontal -label "Stopindex"]

    pack $starts -side top -fill x
    pack $stops -side top -fill x
    $starts config -state disabled
    $stops config -state disabled

    set okb [button $okf.okb -text "Ok" -command "grab_up $par $new_netw"]
    pack $okb -side right
#     set cancelb [button $okf.cancelb -text "Abbrechen" -command "destroy $new_netw"]
#     pack $cancelb -side right

}

proc new_net_a { par flag } {
    set new_net_aw $par.new_net_aw
    toplevel $new_net_aw
    grab set $new_net_aw
    wm title $new_net_aw "Netz A E/I O/U Parameter"
    wm iconname $new_net_aw "Netz A E/I O/U Parameter"
    set x [winfo x $par]
    set y [winfo y $par]
    set newx [expr $x + "25"]
    set newy [expr $y + "25"]
    wm geometry $new_net_aw +$newx+$newy

    set neuronsf [frame $new_net_aw.neuronsf]
    set funcf [frame $new_net_aw.funcf]
    set okf [frame $new_net_aw.okf]
    pack $funcf -side top
    pack $neuronsf -side top
    pack $okf -side top

    global nb_layers0
    global nb_neurons0_0 5
    global nb_neurons0_1 5
    global nb_neurons0_2 5
    global nb_neurons0_3 5
    global nb_neurons0_4 5
    global nb_neurons0_5 5
    global nb_neurons0_6 5
    global nb_neurons0_7 5
    global nb_neurons0_8 5
    global nb_neurons0_9 5
    global nb_neurons0_10 5
    global nb_neurons0_11 5
    global nb_neurons0_12 5
    global nb_neurons0_13 5
    global nb_neurons0_14 5
    global nb_neurons0_15 5
    global nb_neurons0_16 5
    global nb_neurons0_17 5
    global nb_neurons0_18 5
    global nb_neurons0_19 5
    global trans0_0 0
    global trans0_1 0
    global trans0_2 0
    global trans0_3 0
    global trans0_4 0
    global trans0_5 0
    global trans0_6 0
    global trans0_7 0
    global trans0_8 0
    global trans0_9 0
    global trans0_10 0
    global trans0_11 0
    global trans0_12 0
    global trans0_13 0
    global trans0_14 0
    global trans0_15 0
    global trans0_16 0
    global trans0_17 0
    global trans0_18 0
    global trans0_19 0
    global trans0_20 0

    for {set i 0} {$i<$nb_layers0} {incr i} {
	set n_frame [frame $neuronsf.nb_neurons0_$i]
	pack $n_frame -side left

	set {nb_neuronss0_$i} [scale $n_frame.$i -from 1 -to 20 -length 100 \
				   -orient vertical -variable nb_neurons0_$i]
	pack ${nb_neuronss0_$i} -side top
	if { $flag == "show" } { ${nb_neuronss0_$i} config -state disabled }

	set {funcb_$i} [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
	set {m0_$i} [menu ${funcb_$i}.menu -tearoff false]
	${m0_$i} add radio -label "tanh" -variable trans0_$i -value 0
	${m0_$i} add radio -label "logi" -variable trans0_$i -value 1
	${m0_$i} add separator
	${m0_$i} add radio -label "step0" -variable trans0_$i -value 2
	${m0_$i} add radio -label "step1" -variable trans0_$i -value 3
	${m0_$i} add radio -label "lin" -variable trans0_$i -value 4
	pack ${funcb_$i} -side top
	if { $flag == "show" } {
	    ${m0_$i} entryconfigure 0 -state disabled
	    ${m0_$i} entryconfigure 1 -state disabled
	    ${m0_$i} entryconfigure 3 -state disabled
	    ${m0_$i} entryconfigure 4 -state disabled
	    ${m0_$i} entryconfigure 5 -state disabled
	}
    }
    set n_frame [frame $neuronsf.nb_neurons_$nb_layers0]
    pack $n_frame -side left

    set descl [label $n_frame.descl -text "Output-Schicht"]
    pack $descl -side top
    set {funcb_$i} [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
    set {m0_$i} [menu ${funcb_$i}.menu -tearoff false]
    ${m0_$i} add radio -label "tanh" -variable trans0_$i -value 0
    ${m0_$i} add radio -label "logi" -variable trans0_$i -value 1
    ${m0_$i} add separator
    ${m0_$i} add radio -label "step0" -variable trans0_$i -value 2
    ${m0_$i} add radio -label "step1" -variable trans0_$i -value 3
    ${m0_$i} add radio -label "lin" -variable trans0_$i -value 4
    pack ${funcb_$i} -side top
    if { $flag == "show" } {
	${m0_$i} entryconfigure 0 -state disabled
	${m0_$i} entryconfigure 1 -state disabled
	${m0_$i} entryconfigure 3 -state disabled
	${m0_$i} entryconfigure 4 -state disabled
	${m0_$i} entryconfigure 5 -state disabled
    }

#     set okb [button $okf.okb -text "Ok"]
#     pack $okb -side left
    set cancelb [button $okf.cancelb -text "Zurueck" -command "grab_up $par $new_net_aw"]
    pack $cancelb -side right
}

proc new_net_ei { par flag } {
    set new_net_eiw $par.new_net_eiw
    toplevel $new_net_eiw
    grab set $new_net_eiw
    wm title $new_net_eiw "Netz E/I Parameter"
    wm iconname $new_net_eiw "Netz E/I Parameter"
    set x [winfo x $par]
    set y [winfo y $par]
    set newx [expr $x + "25"]
    set newy [expr $y + "25"]
    wm geometry $new_net_eiw +$newx+$newy

    set neuronsf [frame $new_net_eiw.neuronsf]
    set okf [frame $new_net_eiw.okf]
    pack $neuronsf -side top
    pack $okf -side top

    global nb_layers1
    global nb_neurons1_0 5
    global nb_neurons1_1 5
    global nb_neurons1_2 5
    global nb_neurons1_3 5
    global nb_neurons1_4 5
    global nb_neurons1_5 5
    global nb_neurons1_6 5
    global nb_neurons1_7 5
    global nb_neurons1_8 5
    global nb_neurons1_9 5
    global nb_neurons1_10 5
    global nb_neurons1_11 5
    global nb_neurons1_12 5
    global nb_neurons1_13 5
    global nb_neurons1_14 5
    global nb_neurons1_15 5
    global nb_neurons1_16 5
    global nb_neurons1_17 5
    global nb_neurons1_18 5
    global nb_neurons1_19 5
    global trans1_0 0
    global trans1_1 0
    global trans1_2 0
    global trans1_3 0
    global trans1_4 0
    global trans1_5 0
    global trans1_6 0
    global trans1_7 0
    global trans1_8 0
    global trans1_9 0
    global trans1_10 0
    global trans1_11 0
    global trans1_12 0
    global trans1_13 0
    global trans1_14 0
    global trans1_15 0
    global trans1_16 0
    global trans1_17 0
    global trans1_18 0
    global trans1_19 0
    global trans1_20 0

    for {set i 0} {$i<$nb_layers1} {incr i} {
	set n_frame [frame $neuronsf.nb_neurons1_$i]
	pack $n_frame -side left

	set {nb_neuronss1_$i} [scale $n_frame.$i -from 1 -to 20 -length 100 \
				   -orient vertical -variable nb_neurons1_$i]
	pack ${nb_neuronss1_$i} -side top
	if { $flag == "show" } { ${nb_neuronss1_$i} config -state disabled }

# 	scale $n_frame.$i -from 1 -to 20 -length 100 \
# 		-orient vertical -variable nb_neurons1_$i
# 	pack $n_frame.$i -side top

	set funcb [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
	set {m1_$i} [menu $funcb.menu -tearoff false]
	${m1_$i} add radio -label "tanh" -variable trans1_$i -value 0
	${m1_$i} add radio -label "logi" -variable trans1_$i -value 1
	${m1_$i} add separator
	${m1_$i} add radio -label "step0" -variable trans1_$i -value 2
	${m1_$i} add radio -label "step1" -variable trans1_$i -value 3
	${m1_$i} add radio -label "lin" -variable trans1_$i -value 4
	pack $funcb -side top
	if { $flag == "show" } {
	    ${m1_$i} entryconfigure 0 -state disabled
	    ${m1_$i} entryconfigure 1 -state disabled
	    ${m1_$i} entryconfigure 3 -state disabled
	    ${m1_$i} entryconfigure 4 -state disabled
	    ${m1_$i} entryconfigure 5 -state disabled
	}	
    }
    set n_frame [frame $neuronsf.nb_neurons_$nb_layers1]
    pack $n_frame -side left

    set descl [label $n_frame.descl -text "Output-Schicht"]
    pack $descl -side top
    set funcb [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
    set {m1_$i} [menu $funcb.menu -tearoff false]
    ${m1_$i} add radio -label "tanh" -variable trans0_$i -value 0
    ${m1_$i} add radio -label "logi" -variable trans0_$i -value 1
    ${m1_$i} add separator
    ${m1_$i} add radio -label "step0" -variable trans0_$i -value 2
    ${m1_$i} add radio -label "step1" -variable trans0_$i -value 3
    ${m1_$i} add radio -label "lin" -variable trans0_$i -value 4
    pack $funcb -side top
    if { $flag == "show" } {
	${m1_$i} entryconfigure 0 -state disabled
	${m1_$i} entryconfigure 1 -state disabled
	${m1_$i} entryconfigure 3 -state disabled
	${m1_$i} entryconfigure 4 -state disabled
	${m1_$i} entryconfigure 5 -state disabled
    }

#     set okb [button $okf.okb -text "Ok"]
#     pack $okb -side left
    set cancelb [button $okf.cancelb -text "Zurueck" -command "grab_up $par $new_net_eiw"]
    pack $cancelb -side right
}

proc new_net_ou { par flag } {
    set new_net_ouw $par.new_net_ouw
    toplevel $new_net_ouw
    grab set $new_net_ouw
    wm title $new_net_ouw "Netz O/U Parameter"
    wm iconname $new_net_ouw "Netz O/U Parameter"
    set x [winfo x $par]
    set y [winfo y $par]
    set newx [expr $x + "25"]
    set newy [expr $y + "25"]
    wm geometry $new_net_ouw +$newx+$newy

    set neuronsf [frame $new_net_ouw.neuronsf]
    set okf [frame $new_net_ouw.okf]
    pack $neuronsf -side top
    pack $okf -side top

    global nb_layers2
    global nb_neurons2_0 5
    global nb_neurons2_1 5
    global nb_neurons2_2 5
    global nb_neurons2_3 5
    global nb_neurons2_4 5
    global nb_neurons2_5 5
    global nb_neurons2_6 5
    global nb_neurons2_7 5
    global nb_neurons2_8 5
    global nb_neurons2_9 5
    global nb_neurons2_10 5
    global nb_neurons2_11 5
    global nb_neurons2_12 5
    global nb_neurons2_13 5
    global nb_neurons2_14 5
    global nb_neurons2_15 5
    global nb_neurons2_16 5
    global nb_neurons2_17 5
    global nb_neurons2_18 5
    global nb_neurons2_19 5
    global trans2_0 0
    global trans2_1 0
    global trans2_2 0
    global trans2_3 0
    global trans2_4 0
    global trans2_5 0
    global trans2_6 0
    global trans2_7 0
    global trans2_8 0
    global trans2_9 0
    global trans2_10 0
    global trans2_11 0
    global trans2_12 0
    global trans2_13 0
    global trans2_14 0
    global trans2_15 0
    global trans2_16 0
    global trans2_17 0
    global trans2_18 0
    global trans2_19 0
    global trans2_20 0

    for {set i 0} {$i<$nb_layers2} {incr i} {
	set n_frame [frame $neuronsf.nb_neurons2_$i]
	pack $n_frame -side left

	set {nb_neuronss2_$i} [scale $n_frame.$i -from 1 -to 20 -length 100 \
				   -orient vertical -variable nb_neurons2_$i]
	pack ${nb_neuronss2_$i} -side top
	if { $flag == "show" } { ${nb_neuronss2_$i} config -state disabled }

# 	scale $n_frame.$i -from 1 -to 20 -length 100 \
# 		-orient vertical -variable nb_neurons2_$i
# 	pack $n_frame.$i -side top

	set funcb [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
	set {m2_$i} [menu $funcb.menu -tearoff false]
	${m2_$i} add radio -label "tanh" -variable trans2_$i -value 0
	${m2_$i} add radio -label "logi" -variable trans2_$i -value 1
	${m2_$i} add separator
	${m2_$i} add radio -label "step0" -variable trans2_$i -value 2
	${m2_$i} add radio -label "step1" -variable trans2_$i -value 3
	${m2_$i} add radio -label "lin" -variable trans2_$i -value 4
	pack $funcb -side top
	if { $flag == "show" } {
	    ${m2_$i} entryconfigure 0 -state disabled
	    ${m2_$i} entryconfigure 1 -state disabled
	    ${m2_$i} entryconfigure 3 -state disabled
	    ${m2_$i} entryconfigure 4 -state disabled
	    ${m2_$i} entryconfigure 5 -state disabled
	}	
	
    }
    set n_frame [frame $neuronsf.nb_neurons_$nb_layers2]
    pack $n_frame -side left

    set descl [label $n_frame.descl -text "Output-Schicht"]
    pack $descl -side top
    set funcb [menubutton $n_frame.mb -text "f" -menu $n_frame.mb.menu -relief ridge -indicatoron true]
    set {m2_$i} [menu $funcb.menu -tearoff false]
    ${m2_$i} add radio -label "tanh" -variable trans0_$i -value 0
    ${m2_$i} add radio -label "logi" -variable trans0_$i -value 1
    ${m2_$i} add separator
    ${m2_$i} add radio -label "step0" -variable trans0_$i -value 2
    ${m2_$i} add radio -label "step1" -variable trans0_$i -value 3
    ${m2_$i} add radio -label "lin" -variable trans0_$i -value 4
    pack $funcb -side top
    if { $flag == "show" } {
	${m2_$i} entryconfigure 0 -state disabled
	${m2_$i} entryconfigure 1 -state disabled
	${m2_$i} entryconfigure 3 -state disabled
	${m2_$i} entryconfigure 4 -state disabled
	${m2_$i} entryconfigure 5 -state disabled
    }

#     set okb [button $okf.okb -text "Ok"]
#     pack $okb -side left
    set cancelb [button $okf.cancelb -text "Zurueck" -command "grab_up $par $new_net_ouw"]
    pack $cancelb -side right
}

