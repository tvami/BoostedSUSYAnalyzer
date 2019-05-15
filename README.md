# BoostAnalyzer18

The event structure can be seen in include/eventBuffer.h with JEC corrected in include/eventBuffer_JEC.h

As usually, do

source setup.sh
make clean
make

You have to modify the Analysis_*.h for changing the analysis cuts

./Analyzer filelists/backgrounds/filelist_DY.txt DY.root

The condor scripts are in 
https://github.com/cghuh/RazorBoost-CMS-Connect
