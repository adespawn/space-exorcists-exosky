touch ${1}tmp
for file in $1*; do
    if [[ $file == *.top ]]; then 
        echo $file
        cat $file >> "${1}tmp"
    fi 
done
./phase3 < "${1}tmp" >$2
rm "${1}tmp"
