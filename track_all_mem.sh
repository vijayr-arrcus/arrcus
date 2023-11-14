#!/bin/bash
#ST sleep timer.
ST=300
for ((i=1;1;i++))
do
echo "${i}. ***********************************************";
date; top -n 1 -b o %MEM;
echo "${i}. *********************************************** END";
#for ((j=0;$j<$ST;j++)) do sleep 1; echo -en "$(date) \t - wait $(($ST - $j)) \r"; done
sleep $ST
printf "\n\n\n"
done

