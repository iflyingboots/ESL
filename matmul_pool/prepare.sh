#!/bin/sh

cd dsp; make
make
make send
cd ..
cd gpp
make
make send

