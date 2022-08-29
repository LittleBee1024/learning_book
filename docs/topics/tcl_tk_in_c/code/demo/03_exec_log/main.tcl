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
scrollbar .text.scrolly -orient vertical -command {.text.log yview}
scrollbar .text.scrollx -orient horizontal -command {.text.log xview}
pack .text.scrolly -side right -fill y
pack .text.scrollx -side bottom -fill x
pack .text.log -side left -fill both -expand true
pack .text -side top -fill both -expand true

# 执行命令
proc Run {} {
   global in_command input log button_run
   # 通过管道执行命令
   if [catch {open "|$in_command"} input] {
      # 发生错误
      $log insert end $input\n
      # 修改显示位置，否则窗口不会滚动显示最新的结果
      $log see insert
   } else {
      # $input是一个文件描述符，设置其可读回调函数Log，用于将执行的结果输出，此步骤为异步
      fileevent $input readable Log
      # 打印执行的命令，由于此时命令还没执行完成，下面的话会打印在命令结果之前
      $log insert end ">$in_command\n"
      # 将按钮文字显示为Stop，表明正则执行命令
      $button_run config -text Running -command Close
   }
}

# 从结果文件描述符$input中读出命令的结果，并打印
# 由于此动作被设置为可读回调函数，只要有结果没有被读出，会一直调用
proc Log {} {
   global input log
   if [eof $input] {
      # 调用结束动作，改变按钮显示内容
      Close
   } else {
      # 从文件描述符中读取一行
      gets $input line
      # 打印一行内容
      $log insert end $line\n
      # 修改显示位置，否则窗口不会滚动显示最新的结果
      $log see insert
   }
}

# 关闭结果文件描述符$input，并恢复按钮显示
proc Close {} {
   global input button_run
   catch {close $input}
   $button_run config -text "Run it" -command Run
}
