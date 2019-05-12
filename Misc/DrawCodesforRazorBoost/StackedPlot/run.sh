cmsenv
rm -r HistogramLin;
rm -r Histogram;
mkdir Histogram;
mv StackedPlotLin.C StackedPlot.C
make clean
rm -f StackedPlot
make -Bj
source remove.sh;
./StackedPlot;
#./StackedPlot_2D;
source mvfile.sh;
mv Histogram HistogramLin
mv /eos/user/t/tvami/www/projects/SUSY/2018Data/Histogram /eos/user/t/tvami/www/projects/SUSY/2018Data/HistogramLin
mv StackedPlot.C StackedPlotLin.C

mkdir Histogram;
mv StackedPlotLog.C StackedPlot.C
make clean
rm -f StackedPlot
make -Bj
source remove.sh;
./StackedPlot;
#./StackedPlot_2D;
source mvfile.sh;
mv StackedPlot.C StackedPlotLog.C
