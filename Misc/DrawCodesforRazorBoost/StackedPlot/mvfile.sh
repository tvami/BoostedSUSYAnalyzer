echo "start the move the files"
mkdir Histogram/1D;
mkdir Histogram/1D/P;
mkdir Histogram/1D/topana;
mkdir Histogram/1D/topana/sideband;
mkdir Histogram/1D/nminusone;
mkdir Histogram/1D/Q;
mkdir Histogram/1D/Qsideband;
mkdir Histogram/1D/S;
mkdir Histogram/1D/Ssideband;
mkdir Histogram/1D/T;
mkdir Histogram/1D/W;
mkdir Histogram/1D/Z;
mkdir Histogram/1D/G;
mkdir Histogram/1D/L;
#mkdir Histogram/2D/Q;
#mkdir Histogram/2D/Qsideband;
#mkdir Histogram/2D/S;
#mkdir Histogram/2D/Ssideband;
#mkdir Histogram/2D/T;
#mkdir Histogram/2D/W;
#mkdir Histogram/2D/Z;
#mkdir Histogram/2D/G;
mv Histogram/njet.* Histogram/1D/P/.
mv Histogram/nAK8jet.* Histogram/1D/P/.
mv Histogram/nbTight.* Histogram/1D/P/.
mv Histogram/nbMedium.* Histogram/1D/P/.
mv Histogram/nbLoose.* Histogram/1D/P/.
mv Histogram/nw.* Histogram/1D/P/.
mv Histogram/nEleVeto.* Histogram/1D/P/.
mv Histogram/nEleTight.* Histogram/1D/P/.
mv Histogram/nEleMedium.* Histogram/1D/P/.
mv Histogram/nEleLoose.* Histogram/1D/P/.
mv Histogram/nMuTight.* Histogram/1D/P/.
mv Histogram/nMuMedium.* Histogram/1D/P/.
mv Histogram/nMuSoft.* Histogram/1D/P/.
mv Histogram/nTauVeto.* Histogram/1D/P/.
mv Histogram/nIsoTrack.* Histogram/1D/P/.
mv Histogram/nPhoMedium.* Histogram/1D/P/.
mv Histogram/ht_AK4.* Histogram/1D/P/.
mv Histogram/R2_MR.* Histogram/1D/P/.
mv Histogram/MET.* Histogram/1D/P/.
mv Histogram/jet1_pt.* Histogram/1D/P/.
mv Histogram/h_softDropMass.* Histogram/1D/Ssideband/.
 
mv Histogram/*no* Histogram/1D/nminusone/;
#mv Histogram/*topana*s* Histogram/1D/topana/sideband/; 
#mv Histogram/*topana*q* Histogram/1D/topana/sideband/; 
#mv Histogram/*topana* Histogram/1D/topana/; 
mv Histogram/*_Q* Histogram/1D/Q/; 
mv Histogram/*_q* Histogram/1D/Qsideband/;
mv Histogram/*_T* Histogram/1D/T/; 
mv Histogram/*_W* Histogram/1D/W/; 
mv Histogram/*_s* Histogram/1D/Ssideband/;
mv Histogram/*_S* Histogram/1D/S/; 
mv Histogram/*_Z* Histogram/1D/Z/; 
mv Histogram/*_G* Histogram/1D/G/; 
mv Histogram/*_L* Histogram/1D/L/; 
#mv Histogram/2D/*_Q_* Histogram/2D/Q/; 
#mv Histogram/2D/*_q_* Histogram/2D/Qsideband/;
#mv Histogram/2D/*_T_* Histogram/2D/T/; 
#mv Histogram/2D/*_W_* Histogram/2D/W/; 
#mv Histogram/2D/*_s_* Histogram/2D/Ssideband/;
#mv Histogram/2D/*_S_* Histogram/2D/S/; 
#mv Histogram/2D/*_Z_* Histogram/2D/Z/; 
#mv Histogram/2D/*_G_* Histogram/2D/G/; 

cp index.php Histogram/.
cp index.php Histogram/1D/.
cp index.php Histogram/1D/P/.
cp index.php Histogram/1D/L/.
cp index.php Histogram/1D/topana/.
cp index.php Histogram/1D/topana/sideband/.
cp index.php Histogram/1D/nminusone/.
cp index.php Histogram/1D/Q/.
cp index.php Histogram/1D/Qsideband/.
cp index.php Histogram/1D/S/.
cp index.php Histogram/1D/Ssideband/.
cp index.php Histogram/1D/T/.
cp index.php Histogram/1D/W/.
cp index.php Histogram/1D/Z/.
cp index.php Histogram/1D/G/.

cp .htaccess Histogram/.
cp .htaccess Histogram/1D/.
cp .htaccess Histogram/1D/P/.
cp .htaccess Histogram/1D/L/.
cp .htaccess Histogram/1D/topana/.
cp .htaccess Histogram/1D/topana/sideband/.
cp .htaccess Histogram/1D/nminusone/.
cp .htaccess Histogram/1D/Q/.
cp .htaccess Histogram/1D/Qsideband/.
cp .htaccess Histogram/1D/S/.
cp .htaccess Histogram/1D/Ssideband/.
cp .htaccess Histogram/1D/T/.
cp .htaccess Histogram/1D/W/.
cp .htaccess Histogram/1D/Z/.
cp .htaccess Histogram/1D/G/.
mv  Histogram/1D/* Histogram/.
rm -r Histogram/1D/
rm  Histogram/nisrjets.png
rm  Histogram/npvLowHigh_data.png
rm  Histogram/softDropMass.png
cp -r Histogram /eos/user/t/tvami/www/projects/SUSY/2018Data/.
echo "Please rename: mv /eos/user/t/tvami/www/projects/SUSY/2018Data/Histogram/ /eos/user/t/tvami/www/projects/SUSY/2018Data/1905"
echo "Please rename: mv Histogram 1905"
