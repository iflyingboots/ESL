#!/bin/sh

cd dsp
make
cp Debug/pool_notify.out ../
cd ..
cd gpp
make
cp Debug/klt ../
cp Release/klt ../klt_release