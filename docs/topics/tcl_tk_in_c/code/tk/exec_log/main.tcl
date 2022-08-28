#!/usr/bin/wish

# 设置窗口标题
wm title . ExecLog

# frame用户组件布局，创建“.top”框架
frame .top -borderwidth 10
# 将“.top”框架水平平铺在顶部
pack .top -side top -fill x

# 在“.top”框架内放置按钮
# “Quit”按钮绑定退出动作
button .top.quit -text Quit -command exit
# “Run it”按钮绑定“Run”动作
set button_run [button .top.run -text "Run it" -command Run]
# 将按钮放置在“.top”框架的右侧
pack .top.quit .top.run -side right

# 在“.top”框架内放置输入框：label + entry
label .top.l -text Command: -padx 0
# 将entry组件输入的内容，绑定在变量“in_command”中
entry .top.cmd -width 20 -relief sunken -textvariable in_command
pack .top.l -side left
# "-expand true"配置可将输入框自动拉伸
pack .top.cmd -side left -fill x -expand true

# 将快捷键绑定在组件，并触发相应动作
bind .top.cmd <Return> Run
bind .top.cmd <Control-c> Stop
# 开始程序后，主动聚焦在.top.cmd上，否则需要先用鼠标聚焦
focus .top.cmd

# 创建“.text”框架，用于文字布局
frame .text
# 在文字布局中，创建text组件，包括水平和竖直方向的滚动条
set log [text .text.log -width 80 -height 10 \
   -borderwidth 2 -relief raised -setgrid true -state normal -wrap none \
   -yscrollcommand {.text.scrolly set} -xscrollcommand {.text.scrollx set}]
# 通过“text marks”将显示框配置成只读，“limit mark”是只读位置和可修改部分的边界，“insert mark”是光标的位置
$log mark set limit insert
$log mark gravity limit left
focus $log
# 创建滚动条
scrollbar .text.scrolly -orient vertical -command {.text.log yview}
scrollbar .text.scrollx -orient horizontal -command {.text.log xview}
pack .text.scrolly -side right -fill y
pack .text.scrollx -side bottom -fill x
pack .text.log -side left -fill both -expand true
pack .text -side top -fill both -expand true

# Run the program and arrange to read its input

proc Run {} {
   global in_command input log button_run
   # catch command sets input to a file descriptor if command successes,
   # or to error message if the command fails.
   if [catch {open "|$in_command |& cat"} input] {
      # print command error
      $log insert end $input\n
   } else {
      # set callback function to Log, called when pipeline generates output
      fileevent $input readable Log
      # print command line
      $log insert end ">$in_command\n"
      $button_run config -text Stop -command Stop
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
   global input button_run
   catch {close $input}
   $button_run config -text "Run it" -command Run
}
