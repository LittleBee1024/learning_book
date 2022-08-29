#!/usr/local/bin/wish
# 创建一个可以执行TCL命令的窗口，主要涉及的章节有：
#  Chapter 36. The Text Widget
#  Chapter 29. Binding Commands to Events

# 带滚动条的文本输入框，传入frame名称和text的配置参数，返回text实例
proc Scrolled_Text { f args } {
   frame $f
   eval {text $f.text -wrap none \
      -xscrollcommand [list $f.xscroll set] \
      -yscrollcommand [list $f.yscroll set]} $args
   scrollbar $f.xscroll -orient horizontal -command [list $f.text xview]
   scrollbar $f.yscroll -orient vertical -command [list $f.text yview]
   grid $f.text $f.yscroll -sticky news
   grid $f.xscroll -sticky news
   grid rowconfigure $f 0 -weight 1
   grid columnconfigure $f 0 -weight 1
   return $f.text
}
# 创建Scrolled_Text实例，返回的是text组件
set t [Scrolled_Text .win -width 80 -height 10]
# 需要放置frame，而不是返回的$t
pack .win -fill both -expand true

# 为text输入框创建不同类型的标签，用于格式化，参见“Text Tags”章节
$t tag configure prompt -underline true
$t tag configure result -foreground purple
$t tag configure error -foreground red
$t tag configure output -foreground blue

# 创建数组eval，其中，以“prompt”为键值的内容为“tcl> ”
set eval(prompt) "tcl> "
# 在输入框的“insert”位置，插入$eval(prompt)内容，它的格式是标签prompt
$t insert insert $eval(prompt) prompt
# 设置limit标记为当前输入位置，limit和end之间的内容是要执行的TCL命令
$t mark set limit insert
# 设置limit标记的gravity属性为left，这样insert动作就不会改变limit位置
# 默认是right，limit的位置会随insert增加，参见“Mark Gravity”章节
$t mark gravity limit left
# 将光标聚焦在输入框
focus $t
# 设置变量“eval(text)”
set eval(text) $t

# 绑定动作和事件
# 绑定回车事件，触发TCL命令执行动作“EvalTypein”
bind $t <Return> {EvalTypein ; break}
# 绑定回退事件，用于删除字符
bind $t <BackSpace> {
   # %W代表组件的全名称，参见“The bind Command”章节
   # 打印所有标签：[%W tag names] -> sel prompt result error output
   # 判断sel标签在1.0和end范围内是否为空，参见“Text Operations”章节
   if {[%W tag nextrange sel 1.0 end] != ""} {
      # 删除选择的多个字符
      %W delete sel.first sel.last
   } elseif {[%W compare insert > limit]} {
      # 删除当前字符
      %W delete insert-1c
      # 修改显示位置，否则窗口不会滚动显示最新的结果
      %W see insert
   }
   # 不满足上述条件时，不删除任何字符
   break
}
# 绑定输入事件，控制不同位置输入的情况
bind $t <Key> {
   # 判断插入位置是否在活动窗口内(当前行)，参见“Text Indices”章节
   if [%W compare insert < limit] {
      # 插入位置不在当前行，设置插入位置到末尾，这样后面的输入都会添加在最后
      %W mark set insert end
   }
}

# 处理用户输入(TCL命令)，输入为limit标记和end标记间的内容
proc EvalTypein {} {
   global eval
   # 在回车位置换行符
   $eval(text) insert insert \n
   # limit和end之间的所有内容为输入命令，包括上面的换行符号
   set command [$eval(text) get limit end]
   # 判断是否为完整的TCL命令
   if [info complete $command] {
      # 更新limit位置为插入位，前面设置了所有的命令输出结果都会显示在limit位置的左侧
      $eval(text) mark set limit insert
      Eval $command
   }
}

# 执行命令并显示结果
proc Eval {command} {
   global eval
   # 调整插入位置
   $eval(text) mark set insert end
   # 利用“eval(slave)”执行TCL命令
   if [catch {$eval(slave) eval $command} result] {
      # 执行命令出错，用error标签格式化输出
      $eval(text) insert insert $result error
   } else {
      # 正常执行命令，用result标签格式化输出
      $eval(text) insert insert $result result
   }
   # 如果不是行起始位(例如错误命令时)，换行
   if {[$eval(text) compare insert != "insert linestart"]} {
      $eval(text) insert insert \n
   }
   # 输出prompt
   $eval(text) insert insert $eval(prompt) prompt
   $eval(text) see insert
   # 更新limit为当前位置
   $eval(text) mark set limit insert
   return
}

# 创建TCL子解释器
proc SlaveInit {slave} {
   interp create $slave
   # 通过PutsAlias重写puts命令，将结果输出到界面中，默认是输出到terminal窗口
   interp alias $slave puts {} PutsAlias $slave
   return $slave
}

# 定制化puts命令，将puts的结果输出到当前窗口
proc PutsAlias {slave args} {
   if {[llength $args] > 3} {
      error "invalid arguments"
   }
   set newline "\n"
   if {[string match "-nonewline" [lindex $args 0]]} {
      set newline ""
      set args [lreplace $args 0 0]
   }
   if {[llength $args] == 1} {
      set chan stdout
      set string [lindex $args 0]$newline
   } else {
      set chan [lindex $args 0]
      set string [lindex $args 1]$newline
   }
   if [regexp (stdout|stderr) $chan] {
      # 输出puts的结果
      global eval
      $eval(text) insert limit $string output
      $eval(text) see limit
   } else {
      puts -nonewline $chan $string
   }
}

# 创建TCL解释器，用于执行TCL命令
set eval(slave) [SlaveInit shell]
