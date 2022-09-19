#!/bin/sh

TIMES=10
rm -rf run.log
for i in `seq 1 $TIMES`; do
   echo "--- Run #$i---" | tee -a run.log
   ./main | tee -a run.log
done
