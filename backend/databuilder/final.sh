#!/bin/bash
# THE SCRIPT TO CREATE THE DATABASE.
# RUN WITH EXTREME CAUTION

# ./run_all.sh &>> .log/phase1_sh

# echo "PHASE 1 COMPLETE"

# node phase1_info.js

cd src
node phase_2.js ../layers/l1 ../layers/l2 &>> ../.log/phase2_node

echo "PHASE 2 L1 COMPLETE"

node phase_2.js ../layers/l2 ../layers/l3 &>> ../.log/phase2_node

echo "PHASE 2 L2 COMPLETE"

node phase_2.js ../layers/l3 ../layers/l4 &>> ../.log/phase2_node

echo "PHASE 2 L3 COMPLETE"

node phase_2.js ../layers/l4 ../layers/l5 &>> ../.log/phase2_node

echo "PHASE 2 L4 COMPLETE"

node phase_3.js ../layers/l1 ../layers/l2 ../layers/l3 ../layers/l4 ../layers/l5 &>>../.log/phase3_node

echo "PHASE 3 COMPLETE"

node phase_4.js ../layers/l5 ../layers/l4 ../layers/l3 ../layers/l2 &>>../.log/phase4_node

echo "PHASE 4 COMPLETE"
echo "DONE!!!"
