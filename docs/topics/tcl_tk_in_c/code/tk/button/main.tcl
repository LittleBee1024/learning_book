#!/usr/bin/wish

# 将按钮实例存于“$hello_button”，后面对按钮的操作都可以通过“$hello_button”完成
set hello_button [button .hello -text "Hello, world!" -command {puts stdout "Hello, World!"}]
# 配置按钮背景色
$hello_button config -background green
# 将按钮在屏幕上显示
pack $hello_button -padx 20 -pady 10

# 获取按钮的属性
set color [$hello_button cget -background]
puts "hello buttton background: $color"

# 通过函数打印组件的所有属性
proc Widget_Attributes {w {out stdout}} {
   puts $out [format "%-20s %-10s %s" Attribute Default Value]
   foreach item [$w configure] {
      puts $out [format "%-20s %-10s %s" [lindex $item 0] [lindex $item 3] [lindex $item 4]]
   }
}
Widget_Attributes $hello_button

