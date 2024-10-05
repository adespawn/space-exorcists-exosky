# touch xd
echo $1 >>xd
./phase2 $1 $2
rm "$1$2.raw"
