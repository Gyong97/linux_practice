#!/bin/sh
row=$1
column=$2
for i in $(seq 1 $row)
do
for j in $(seq 1 $column)
do
res=`expr $i \* $j`
printf "$i*$j=$res "
done
echo
done
exit 0
