# Chapter 23 The Pack Geometry Manager
# Chapter 32-1 Frames, Labelframes, and Toplevel Windows

# 普通布局
. config -bg black
frame .one -width 40 -height 40 -bg white
labelframe .two -labelanchor n -text split1 -width 200 -height 20 -bg grey50
pack .one .two -side top


# 存在子布局
frame .three -bg white
foreach b {alpha beta gamma} {
   button .three.$b -text $b
   pack .three.$b -side left
}
labelframe .four -text split2 -width 100 -height 20 -bg grey50
pack .three .four -side top


# 存在三层布局
frame .five -bg white
foreach b {alpha beta} {
   button .five.$b -text $b
   pack .five.$b -side left
}
# Create a frame for two more buttons
frame .five.right
foreach b {delta epsilon} {
   button .five.right.$b -text $b
   pack .five.right.$b -side bottom
}
labelframe .six -text split3 -width 200 -height 20 -bg grey50
pack .five.right -side right
pack .five .six -side top

# fill填充
frame .menubar -bg white
labelframe .body -text split4 -height 20 -bg grey50
# Create buttons at either end of the menubar
foreach b {alpha beta} {
   button .menubar.$b -text $b
}
pack .menubar.alpha -side left
pack .menubar.beta -side right
# Let the menu bar fill along the top
pack .menubar -side top -fill x
pack .body -fill x

