#!/bin/bash
ST=300
PROC=$1
for ((i=1;1;i++))
do
echo "$i. ***********************************************";
date; ./ram.py -p $(pidof $PROC); top -n 1 -b -p $(pidof $PROC);
echo "$i. *********************************************** END";
for ((j=0;$j<$ST;j++)) do sleep 1; echo -en "$(date) \t - wait $(($ST - $j)) \r"; done
printf "\n\n\n"
done

