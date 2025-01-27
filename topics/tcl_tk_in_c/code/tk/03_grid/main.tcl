# Chapter 26 The Grid Geometry Manager

foreach color {red orange yellow green blue purple} {
   label .l1$color -text $color -bg white
   frame .f1$color -background $color -width 100 -height 5
   # 放置label和frame
   grid .l1$color .f1$color
   # 调整对齐方式：west
   grid .l1$color -sticky w
   # north south
   grid .f1$color -sticky ns
}

label .lsplit -text --------
frame .fsplit -width 100 -height 20
grid .lsplit .fsplit

foreach color {red orange yellow green blue purple} {
   label .l2$color -text $color -bg white
   frame .f2$color -background $color -width 100 -height 5
   grid .l2$color .f2$color
   # 外部和内部边框
   grid .l2$color -column 1 -sticky w -padx 10 -ipady 5
   grid .f2$color -column 0 -sticky ns -pady 5
}
