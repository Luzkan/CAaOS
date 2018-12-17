for i in /proc/[0-9]*
do
name=$(awk 'FNR == 1 {print $2}' $i/status)
state=$(awk 'FNR == 3 {print $2}' $i/status)
pid=$(awk 'FNR == 6 {print $2}' $i/status)
#pidAlt=$(echo $i | sed -E 's/^[^,]*= *//') 
fileDesc=$(lsof -p $pid | wc -l) 

printf "\033[0;31m[PID: $pid] \t\033[0;m| \033[0;32m[Name: $name] \t\033[0;m| \033[0;36m[#File Descriptors: $fileDesc] \t\033[0;m| \033[0;33m[State: $state]\n"

done
