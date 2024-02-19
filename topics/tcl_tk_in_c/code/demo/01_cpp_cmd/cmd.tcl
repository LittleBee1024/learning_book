# 执行自定义的“blob”命令
blob add Jack 100
set res1 [blob find Jack]
puts "\[  TCL\] Jack's age is ${res1}"
blob find Tim
blob remove Jack
blob find Jack
blob remove Jack
# 无效命令
blob clear

