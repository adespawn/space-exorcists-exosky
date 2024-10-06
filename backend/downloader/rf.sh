node ./data.js /tmp/$1 >/tmp/_xd_$1
./formater </tmp/_xd_$1 >/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/hackaton/$2
rm -f /tmp/_xd_$1
rm -f /tmp/$1
rm -f /tmp/$1.gz
exit
