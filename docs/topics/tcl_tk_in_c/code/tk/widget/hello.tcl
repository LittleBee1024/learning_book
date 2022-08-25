#!/usr/bin/wish

# store widget instance .hello into a variable "hello_button"
set hello_button [button .hello -text "Hello, world!" -command {puts stdout "Hello, World!"}]

# Use $hello_button as a command to oeprate on the button, so that the script doesn't
# depends on the widget pathname
$hello_button config -background green

pack $hello_button -padx 20 -pady 10

# get widget attribute
set color [$hello_button cget -background]
puts "hello buttton background: $color"

# print out all the information about a widget
proc Widget_Attributes {w {out stdout}} {
   puts $out [format "%-20s %-10s %s" Attribute Default Value]
   foreach item [$w configure] {
      puts $out [format "%-20s %-10s %s" [lindex $item 0] [lindex $item 3] [lindex $item 4]]
   }
}
Widget_Attributes $hello_button

