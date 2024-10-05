#!/bin/bash
cd src
make all
cp ./main ../
cd ..
./main <GaiaUniverseModel_0000.raw
