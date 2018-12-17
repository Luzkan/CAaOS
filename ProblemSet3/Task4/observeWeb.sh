#http://bartor.net/random

howto()
{
printf "|-|-| Monitoring website script. Usage:\n$0 <url> <check interval> \n"
}

if [  $# -lt 2 ]
then
howto
exit 1
fi

website=$1
interval=$2
siteFiles=/tmp/sites
mkdir $siteFiles


printf "|-|-| Monitoring: $website\n|-|-| Interval: $2\n"
i=1
lynx -source $website > $siteFiles/siteArchived$i
sleep $interval


while true
do

printf "|-| Redownloading for check.\n"
lynx -source $website > $siteFiles/siteCurrent


check=$(diff $siteFiles/siteArchived$i $siteFiles/siteCurrent)
if [ -n "$check" ] 
then
printf "|-|-| Site changed. Archived version: siteArchive$i\n"
i=$((i+1))
mv $siteFiles/siteCurrent $siteFiles/siteArchived$i
fi


sleep $interval
done
