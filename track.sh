#!/bin/bash
#ST sleep timer.
ST=1800
PROC=$1
mkdir -p /tmp/pmap/$PROC
for ((i=1;1;i++))
do
echo "${i}. ***********************************************" >> /tmp/mem.log
date >> /tmp/mem.log
top -n 1 -b o %MEM >> /tmp/mem.log;
#kill -12 $(pidof $PROC)
date >> /tmp/pmap/$PROC/${i}_pmap.log
pmap -XX $(pidof $PROC) >> /tmp/pmap/$PROC/${i}_pmap.log
echo "${i}. *********************************************** END" >> /tmp/mem.log
#for ((j=0;$j<$ST;j++)) do sleep 1; echo -en "$(date) \t - wait $(($ST - $j)) \r"; done
sleep $ST
printf "\n\n\n"
done

