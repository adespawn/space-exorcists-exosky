#!/bin/bash
DATA_DIR="/media/adespawn/01524459-1aff-4407-a4a6-68c6a130a898/data/test/*"

for file in $DATA_DIR; do 
    if [ -f "$file" ]; then 
        ./main <$file &>>.log
    fi 
done
