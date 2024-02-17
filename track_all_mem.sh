#!/bin/bash
#ST sleep timer.
ST=1800
for ((i=1;1;i++))
do
echo "${i}. ***********************************************" >> /tmp/mem.log;
date >> /tmp/mem.log;
echo -en "\n" >> /tmp/mem.log;
free >> /tmp/mem.log;
echo -en "\n" >> /tmp/mem.log;
top -n 1 -b o %MEM >> /tmp/mem.log;
#for ((j=0;$j<$ST;j++)) do sleep 1; echo -en "$(date) \t - wait $(($ST - $j)) \r"; done
echo "${i}. *********************************************** END" >> /tmp/mem.log;
sleep $ST
echo -en "\n\n\n" >> /tmp/mem.log;
done

