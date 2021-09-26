#!/bin/sh
row=$1
column=$2
if [ $row -le 1 ]
then
echo "first argument must be greater than 0"
exit 0
elif [ $column -le 1 ]
then 
echo "second argument must be greater than 0"
exit 0
fi
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
