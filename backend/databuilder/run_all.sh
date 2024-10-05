#!/bin/bash
DATA_DIR="/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/test/*"

cd src
rm -f phase1
make all
cp ./phase1 ../
cd ..

for file in $DATA_DIR; do 
    if [ -f "$file" ]; then 
        ./phase1 <$file &>>.log/phase1
    fi 
done
