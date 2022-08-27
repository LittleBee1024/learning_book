#!/usr/bin/wish

# execlog - run a program with exec and log the output
# Set window title
wm title . ExecLog

# Create a frame for buttons and entry.

frame .top -borderwidth 10
# The -fill x packing option makes the frame fill out to the whole width of the main window
pack .top -side top -fill x

# Create the command buttons.

button .top.quit -text Quit -command exit
# store .top.run button in "but" var
set but [button .top.run -text "Run it" -command Run]
# pack start from right
pack .top.quit .top.run -side right

# Create a labeled entry for the command

label .top.l -text Command: -padx 0
entry .top.cmd -width 20 -relief sunken -textvariable command
pack .top.l -side left
# "-expand true" expands its packing space and fill up that extra area with its display
pack .top.cmd -side left -fill x -expand true

# Set up key binding equivalents to the buttons

bind .top.cmd <Return> Run
bind .top.cmd <Control-c> Stop
# 开始程序后，主动聚焦在.top.cmd上，否则需要先用鼠标聚焦
focus .top.cmd

# Create a text widget to log the output

frame .t
set log [text .t.log -width 80 -height 10 \
   -borderwidth 2 -relief raised -setgrid true -state normal -wrap none \
   -yscrollcommand {.t.scrolly set} -xscrollcommand {.t.scrollx set}]
scrollbar .t.scrolly -orient vertical -command {.t.log yview}
scrollbar .t.scrollx -orient horizontal -command {.t.log xview}
pack .t.scrolly -side right -fill y
pack .t.scrollx -side bottom -fill x
pack .t.log -side left -fill both -expand true
pack .t -side top -fill both -expand true

# Run the program and arrange to read its input

proc Run {} {
   global command input log but
   # catch command sets input to a file descriptor if command successes,
   # or to error message if the command fails.
   if [catch {open "|$command |& cat"} input] {
      # print command error
      $log insert end $input\n
   } else {
      # set callback function to Log, called when pipeline generates output
      fileevent $input readable Log
      # print command line
      $log insert end ">$command\n"
      $but config -text Stop -command Stop
   }
}

# Read and log output from the program

proc Log {} {
   global input log
   if [eof $input] {
      Stop
   } else {
      # the $input is a file descriptor
      gets $input line
      # print command results
      $log insert end $line\n
   }
}

# Stop the program and fix up the button

proc Stop {} {
   global input but
   catch {close $input}
   $but config -text "Run it" -command Run
}
