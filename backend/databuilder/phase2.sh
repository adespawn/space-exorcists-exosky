#!/bin/bash
cd src
make all
node phase_2.js ../layers/l1 ../layers/l2 &>>../.log/phase_2_js
