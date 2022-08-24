blob add Jack 100
set res1 [blob find Jack]
puts "\[  TCL\] Jack's age is ${res1}"
blob find Tim
blob remove Jack
blob find Jack
blob remove Jack
# invalid command, report error
blob clear

