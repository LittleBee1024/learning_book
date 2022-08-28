#!/usr/local/bin/wish
#  Browser for the Tcl and Tk examples in the book.

# browse(dir) is the directory containing all the tcl files
# Please edit to match your system configuration.

set browse(dir) ./src

wm minsize . 30 5
wm title . "Tcl Example Browser"

# Create a row of buttons along the top

set f [frame .menubar]
pack $f -fill x
button $f.quit -text Quit -command exit
button $f.next -text Next -command Next
button $f.prev -text Previous -command Previous

# The Run and Reset buttons use EvalEcho that
# is defined by the Tcl shell in Example 24–4 on page 389

button $f.load -text Run -command Run
button $f.reset -text Reset -command Reset
pack $f.quit $f.reset $f.load $f.next $f.prev -side right

# A label identifies the current example

label $f.label -textvariable browse(current)
pack $f.label -side right -fill x -expand true

# Create the menubutton and menu
menubutton $f.ex -text Examples -menu $f.ex.m
pack $f.ex -side left
set m [menu $f.ex.m]

# Create the text to display the example
proc Scrolled_Text { f args } {
   frame $f
   eval {text $f.text -wrap none \
      -xscrollcommand [list $f.xscroll set] \
      -yscrollcommand [list $f.yscroll set]} $args
   scrollbar $f.xscroll -orient horizontal \
      -command [list $f.text xview]
   scrollbar $f.yscroll -orient vertical \
      -command [list $f.text yview]
   grid $f.text $f.yscroll -sticky news
   grid $f.xscroll -sticky news
   grid rowconfigure $f 0 -weight 1
   grid columnconfigure $f 0 -weight 1
   return $f.text
}

set browse(text) [Scrolled_Text .body -width 80 -height 10 -setgrid true]
pack .body -fill both -expand true

# Look through the example files for their ID number. 
# For example, "# Example 1-2", x = "# Example 1-2", chap="1", ex="2"
foreach f [lsort -dictionary [glob [file join $browse(dir) *]]] {
   if [catch {open $f} in] {
      puts stderr "Cannot open $f: $in"
      continue
   }
   while {[gets $in line] >= 0} {
      if [regexp {^# Example ([0-9]+)-([0-9]+)} $line x chap ex] {
         lappend examples($chap) $ex
         lappend browse(list) $f
         # Read example title
         gets $in line
         set title($chap-$ex) [string trim $line "# "]
         set file($chap-$ex) $f
         close $in
         break
      }
   }
}

# Create two levels of cascaded menus.
# The first level divides up the chapters into chunks.
# The second level has an entry for each example.

option add *Menu.tearOff 0
set limit 8
set c 0; set i 0
foreach chap [lsort -integer [array names examples]] {
   $m add cascade -label "Chapter $chap" -menu $m.sub$i
   set sub [menu $m.sub$i]
   incr i
   foreach ex [lsort -integer $examples($chap)] {
      $sub add command -label "$chap-$ex $title($chap-$ex)" \
         -command [list Browse $file($chap-$ex)]
   }
}

# Display a specified file. The label is updated to
# reflect what is displayed, and the text is left
# in a read-only mode after the example is inserted.

proc Browse { file } {
   global browse
   set browse(current) [file tail $file]
   set browse(curix) [lsearch $browse(list) $file]
   set t $browse(text)
   $t config -state normal
   $t delete 1.0 end
   if [catch {open $file} in] {
      $t insert end $in
   } else {
      $t insert end [read $in]
      close $in
   }
   $t config -state disabled
}

# Browse the next and previous files in the list

set browse(curix) -1
proc Next {} {
   global browse
   if {$browse(curix) < [llength $browse(list)] - 1} {
      incr browse(curix)
   }
   Browse [lindex $browse(list) $browse(curix)]
}
proc Previous {} {
   global browse
   if {$browse(curix) > 0} {
      incr browse(curix) -1
   }
   Browse [lindex $browse(list) $browse(curix)]
}

proc EvalEcho {command} {
   global eval
   $eval(text) mark set insert end
   if [catch {$eval(slave) eval $command} result] {
      $eval(text) insert insert $result error
   } else {
      $eval(text) insert insert $result result
   }
   if {[$eval(text) compare insert != "insert linestart"]} {
      $eval(text) insert insert \n
   }
   $eval(text) insert insert $eval(prompt) prompt
   $eval(text) see insert
   $eval(text) mark set limit insert
   return
}

# Run the example in the shell

proc Run {} {
   global browse
   EvalEcho [list source \
      [file join $browse(dir) $browse(current)]]
}

# Reset the slave in the eval server

proc Reset {} {
   EvalEcho reset
}

