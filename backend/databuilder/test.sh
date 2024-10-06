#!/bin/bash
cd src
rm -f main
make all
cp ./main ../
cd ..
./main <GaiaUniverseModel_0000.raw
