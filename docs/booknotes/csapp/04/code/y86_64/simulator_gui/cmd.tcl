##########################################################################
#            TCL Processes which will be called from C code              #
##########################################################################

# signal_register_update
proc setReg {id val highlight} {
   global lastId normalBg specialBg
   if {$lastId >= 0} {
      .r.reg$lastId config -bg $normalBg
      set lastId -1
   }
   if {$id < 0 || $id >= 15} {
      error "Invalid Register ($id)"
   }
   .r.reg$id config -text [format %16x $val]
   if {$highlight} {
      uplevel .r.reg$id config -bg $specialBg
      set lastId $id
   }
}

# signal_register_clear: Clear all registers
proc clearReg {} {
   global lastId normalBg
   if {$lastId >= 0} {
      .r.reg$lastId config -bg $normalBg
      set lastId -1
   }
   for {set i 0} {$i < 8} {incr i 1} {
      .r.reg$i config -text ""
   }
}

# create_memory_display
proc createMem {nminAddr nmemCnt} {
   global minAddr memCnt haveMem codeFont dpyFont normalBg
   set minAddr $nminAddr
   set memCnt $nmemCnt

   if { $haveMem } { destroy .m.e }

   # Create Memory Structure
   frame .m.e
   set haveMem 1
   pack .m.e -in .m -side top -anchor w
   # Now fill it with values
   for {set i 0} {$i < $memCnt} {incr i 16} {
      set addr [expr $minAddr + $i]

      frame .m.e.r$i
      pack .m.e.r$i -side bottom -in .m.e
      label .m.e.r$i.lab -width 6 -font $dpyFont -text [format "0x%.3x-"  [expr $addr / 16]]
      pack .m.e.r$i.lab -in .m.e.r$i -side left

      for {set j 0} {$j < 16} {incr j 8} {
         set a [expr $addr + $j]
         label .m.e.v$a -width 16 -font $dpyFont -relief ridge \
            -bg $normalBg
         pack .m.e.v$a -in .m.e.r$i -side left
      }
   }
}

# create_memory_display
proc setMem {Addr Val} {
   global minAddr memCnt
   if {$Addr < $minAddr || $Addr > [expr $minAddr + $memCnt]} {
      error "Memory address $Addr out of range"
   }
   .m.e.v$Addr config -text [format %16x $Val]
}

# show_cc: Set all 3 condition codes
proc setCC {zv cv ov} {
   .cc.cc0 config -text [format %d $zv]
   .cc.cc1 config -text [format %d $cv]
   .cc.cc2 config -text [format %d $ov]
}

# report_line: Add a line of code to the display
proc addCodeLine {line addr op text} {
   global codeRowCount
   # Create new line in display
   global codeFont
   frame .c.tr.$addr
   pack .c.tr.$addr -in .c.tr -side top -anchor w
   label .c.tr.$addr.a -width 6 -text [format "0x%x" $addr] -font $codeFont
   label .c.tr.$addr.i -width 20 -text $op -font $codeFont
   label .c.tr.$addr.s -width 2 -text "" -font $codeFont -bg white
   label .c.tr.$addr.t -text $text -font $codeFont
   pack .c.tr.$addr.a .c.tr.$addr.i .c.tr.$addr.s \
      .c.tr.$addr.t -in .c.tr.$addr -side left
}

# report_state: update status line for specified proc register
proc updateStage {name txts} {
   set Name [string toupper $name]
   global pwins
   set wins [lindex $pwins($Name) 0]
   setDisp $wins $txts
}
