#!/usr/local/bin/wish
# 浏览“./src”目录中所有的TCL文件，并可通过一个TCL解释器执行，涉及的章节有：
#  Chapter 30. Buttons and Menus

# 依赖“02_tcl_shell”中的TCL解释器，用于执行目录中的TCL文件
source ../02_tcl_shell/main.tcl
set browse(dir) ./src
wm minsize . 30 5
wm title . "Tcl Example Browser"

# 创建菜单栏
set f [frame .menubar]
pack $f -fill x
button $f.quit -text Quit -command exit
button $f.next -text Next -command Next
button $f.prev -text Previous -command Previous
button $f.load -text Run -command Run
button $f.reset -text Reset -command Reset
pack $f.quit $f.reset $f.load $f.next $f.prev -side right

# 创建标签，用于显示当前打开的TCL文件名
label $f.label -textvariable browse(current)
pack $f.label -side right -fill x -expand true

# 创建多级菜单
menubutton $f.ex -text Examples -menu $f.ex.m
pack $f.ex -side left
set m [menu $f.ex.m]

# 创建文件内容显示窗口
set browse(text) [Scrolled_Text .body -width 80 -height 10 -setgrid true]
pack .body -fill both -expand true

# 遍历文件夹，并记录文件名和标题等
foreach f [lsort -dictionary [glob [file join $browse(dir) *]]] {
   if [catch {open $f} in] {
      puts stderr "Cannot open $f: $in"
      continue
   }
   while {[gets $in line] >= 0} {
      if [regexp {^# Example ([0-9]+)-([0-9]+)} $line \
             x chap ex] {
         lappend examples($chap) $ex
         lappend browse(list) $f
         # 从文件的第一行注释中找到菜单标题
         gets $in line
         set title($chap-$ex) [string trim $line "# "]
         set file($chap-$ex) $f
         close $in
         break
      }
   }
}

# 关闭子菜单的弹出按钮
option add *Menu.tearOff 0
set limit 8
set c 0; set i 0
# 根据上面遍历的结果，创建出多级子菜单
foreach chap [lsort -integer [array names examples]] {
   if {$i == 0} {
      $m add cascade -label "Chapter $chap..." \
         -menu $m.$c
      set sub1 [menu $m.$c]
      incr c
   }
   set i [expr ($i +1) % $limit]
   $sub1 add cascade -label "Chapter $chap" -menu $sub1.sub$i
   set sub2 [menu $sub1.sub$i]
   foreach ex [lsort -integer $examples($chap)] {
      $sub2 add command -label "$chap-$ex $title($chap-$ex)" \
         -command [list Browse $file($chap-$ex)]
   }
}

# 显示文件内容
proc Browse { file } {
   global browse
   set browse(current) [file tail $file]
   set browse(curix) [lsearch $browse(list) $file]
   set t $browse(text)
   # 恢复为可写
   $t config -state normal
   $t delete 1.0 end
   if [catch {open $file} in] {
      $t insert end $in
   } else {
      $t insert end [read $in]
      close $in
   }
   # 更改为只读
   $t config -state disabled
}

# 记录当前显示文件的标号
set browse(curix) -1
# 显示下一个文件
proc Next {} {
   global browse
   if {$browse(curix) < [llength $browse(list)] - 1} {
      incr browse(curix)
   }
   Browse [lindex $browse(list) $browse(curix)]
}
# 显示上一个文件
proc Previous {} {
   global browse
   if {$browse(curix) > 0} {
      incr browse(curix) -1
   }
   Browse [lindex $browse(list) $browse(curix)]
}

# 利用“source <file>.tcl”命令，运行TCL文件
proc Run {} {
   global browse
   EvalEcho [list source [file join $browse(dir) $browse(current)]]
}

# 重置TCL解释器，并清空记录
proc Reset {} {
   EvalEcho reset
}

# 在TCL解释器中执行TCL命令，并打印结果
proc EvalEcho {command} {
   global eval
   $eval(text) mark set insert end
   $eval(text) insert insert $command\n
   $eval(text) mark set limit insert
   Eval $command
}

