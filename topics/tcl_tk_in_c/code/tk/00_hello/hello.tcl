#!/usr/bin/wish

# Creates an instance of a button
# The name of the buttong is .hello.
# The label on the buttong is Hello
button .hello -text "Hello, world!" -command {puts stdout "Hello, World!"}

# pack command maps the button .hello onto the screen
pack .hello -padx 20 -pady 10

