#!/bin/bash
#ST sleep timer.
ST=300
PROC=$1
mkdir -p /tmp/pmap/$PROC
for ((i=1;1;i++))
do
echo "${i}. ***********************************************";
date; top -n 1 -b -p $(pidof $PROC);
#kill -10 $(pidof $PROC)
date >> /tmp/pmap/$PROC/${i}_pmap.log
pmap -XX $(pidof $PROC) >> /tmp/pmap/$PROC/${i}_pmap.log
echo "${i}. *********************************************** END";
#for ((j=0;$j<$ST;j++)) do sleep 1; echo -en "$(date) \t - wait $(($ST - $j)) \r"; done
sleep $ST
printf "\n\n\n"
done

