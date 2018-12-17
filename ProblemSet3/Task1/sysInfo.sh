let i=0
while true; do
clear
let i++

#gathering values for internet usage calculations

received1=$(awk 'FNR == 4 {print $2}' /proc/net/dev)
send1=$(awk 'FNR == 4 {print $10}' /proc/net/dev)
sleep 1s
received2=$(awk 'FNR == 4 {print $2}' /proc/net/dev)
send2=$(awk 'FNR == 4 {print $10}' /proc/net/dev)


# Difference of values in one second
let "oldavg=(($received2+$send2)-($received1+$send1))"
# Adding up all the values we checked so far
let "newavg=($newavg+$oldavg)"
# Averaging them over all iterations of while loop
let "totalavg=($newavg/i)"


printf "\n|-|-|-|-|-| System Info:\n"
printf "|-| \033[0;31mConnection:\033[0;m\n"

if [ $oldavg -gt 1000000 ]; then
printf "|-|\t\033[0;33m%0.1f MB/s \033[0;m(current)\n" $(echo "$((oldavg/100000))" | bc)
elif [ $oldavg -gt 1000 ]; then
printf "|-|\t\033[0;33m%0.1f KB/s \033[0;m(current)\n" $(echo "$((oldavg/1000))" | bc)
else
printf "|-|\t\033[0;33m%i B/s \033[0;m(current)\n" "$oldavg"
fi

if [ $totalavg -gt 1000000 ]; then
printf "|-|\t\033[0;33m%0.1f MB/s \033[0;m(average)\n" $(echo "$((totalavg/100000))" | bc)
elif [ $totalavg -gt 1000 ]; then
printf "|-|\t\033[0;33m%0.1f KB/s \033[0;m(average)\n" $(echo "$((totalavg/1000))" | bc)
else
printf "|-|\t\033[0;33m%i B/s \033[0;m(average)\n" "$totalavg"
fi


#simple uptime read

cat /proc/uptime | awk -F'( )+' '{printf("|-| \033[0;31mUptime:\033[0;m\n|-|\t\033[0;33m%d\033[0;m days \033[0;33m%02d\033[0;m hours \033[0;33m%02d\033[0;m minutes \033[0;33m%02d\033[0;m seconds\n",($1/60/60/24),($1/60/60%24),($1/60%60),($1%60))}'

#simple loadavg read

cat /proc/loadavg | awk '{printf("|-| \033[0;31mAvg Load:\033[0;m\n|-|\t\033[0;33m%d\033[0;m over last minute\n", ($1*100))}'

#not simple (but works) battery ready

cat /sys/class/power_supply/BAT0/uevent | sed -E 's/^[^,]*= *//' | tr '\n' ' ' | awk -F'( )+' '{printf("|-| \033[0;31mBattery:\033[0;m\n|-|\tusing \033[0;33m%dW\033[0;m / stored energy \033[0;33m%dWh\033[0;m / around \033[0;33m%0.2f\033[0;m hours left\n", ($8/100000), ($11/100000), ($11/($8+1)))}'

#================================================================================ Lots of Stuff for Graph

mmax="▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉▉"
amax="▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║      "
cmax="▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║     ║      "
mmid="▉▉▉▉▉║▉▉▉▉▉║▉▉▉▉▉║     ║     ║      "
cmin="▉▉▉▉▉║▉▉▉▉▉║     ║     ║     ║      "
amin="▉▉▉▉▉║     ║     ║     ║     ║      "
mmin="     ║     ║     ║     ║     ║      "

newA=$(if [ $oldavg -gt 600000 ]; then
printf "$mmax"
elif [ $oldavg -gt 500000 ]; then
printf "$mmax"
elif [ $oldavg -gt 400000 ]; then
printf "$mmax"
elif [ $oldavg -gt 300000 ]; then
printf "$mmid"
elif [ $oldavg -gt 200000 ]; then
printf "$cmin"
elif [ $oldavg -gt 100000 ]; then
printf "$amin"
else
printf "$mmin"
fi) 


printf "|-|\n"
printf "|-| ╔══════╦═════╦═════╦═════╦═════╦═════╦══════╗\n"
printf "|-| ║ t/MB ║ 0.1 ║ 0.2 ║ 0.3 ║ 0.4 ║ 0.5 ║ 0.6+ ║\n"
printf "|-| ╠══════╬═════╬═════╬═════╬═════╬═════╬══════╣\n"
printf "|-| ║  1s  ║$newA║\n"
printf "|-| ║  2s  ║$newB║\n"
printf "|-| ║  3s  ║$newC║\n"
printf "|-| ║  4s  ║$newD║\n"
printf "|-| ║  5s  ║$newE║\n"
printf "|-| ║  6s  ║$newF║\n"
printf "|-| ║  7s  ║$newG║\n"
printf "|-| ║  8s  ║$newH║\n"
printf "|-| ╚══════╩═════╩═════╩═════╩═════╩═════╩══════╝\n"

newH=$newG
newG=$newF
newF=$newE
newE=$newD
newD=$newC
newC=$newB
newB=$newA


done
