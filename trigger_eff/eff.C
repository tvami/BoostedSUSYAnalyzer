#include <iostream>
#include <string>
#include <sstream>

#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TRandom3.h"
#include "TMath.h"
#include "TColor.h"
#include "TEfficiency.h"
#include "TGraphAsymmErrors.h"

using namespace std;

Double_t* getVariableBinEdges(int num_entries, Double_t* tmp_array)
{
  Double_t* my_array = new Double_t[num_entries];
  for (int i = 0; i != num_entries; ++i) {
    my_array[i] = tmp_array[i];
 
    //cout << "bin edge " << i << " : " << my_array[i] << endl;
  }
   return my_array;
}

void fillefferr2D(TH2D* h, TH2D* hp, TH2D* heu, TH2D* hel)
{
  double total, passed, cen, eup, elow;
  for (int i=1; i<h->GetXaxis()->GetNbins()+1; i++) {
    for (int j=1; j<h->GetXaxis()->GetNbins()+1; j++) {
      passed = hp->GetBinContent(i, j);
      total = h->GetBinContent(i, j);
      if (total == 0) continue;
      cen = (double)passed / (double)total;
      eup = TEfficiency::ClopperPearson(total, passed, 0.687, true);
      elow = TEfficiency::ClopperPearson(total, passed, 0.687, false);
      heu->SetBinContent(i, j, eup-cen);
      hel->SetBinContent(i, j, cen-elow);
    }
  }
}


void calceff(TString ntuplePtl = "SingleMuon")
{

  TH1::SetDefaultSumw2();
  TFile* file1 = TFile::Open("./SingleMuon2018_v190311.root");

  TH1D *h_HT_pre  = (TH1D*)file1->Get("h_HT_TriggerEff_0");
  TH1D *h_HT_pre_pass = (TH1D*)file1->Get("h_HT_TriggerEff_1");
  
  //TH1D *h_HT_j1pt_pre_1D = ChangeBinning(h_HT_j1pt_pre);
  //TH1D *h_HT_j1pt_pre_pass_1D = ChangeBinning(h_HT_j1pt_pre_pass);

  TGraphAsymmErrors* h_HT_pre_eff_HT = new TGraphAsymmErrors(h_HT_pre_pass, h_HT_pre, "cl=0.683 b(1,1) mode");
  h_HT_pre_eff_HT->SetName("h_HT_pre_eff_HT");
  h_HT_pre_eff_HT->SetTitle("SingleMuon2018_v190311");
  h_HT_pre_eff_HT->GetXaxis()->SetTitle("H_{T} (GeV)");
  h_HT_pre_eff_HT->GetYaxis()->SetTitle("Trigger efficiency");

  //TFile* output = new TFile(ntuplePtl+"_TriggerEfficiency.root", "recreate");
  //h_HT_pre_eff_HT->Write();

  TCanvas* c1 = new TCanvas("ctemp","",750,750);
  h_HT_pre_eff_HT->Draw();
  c1->SaveAs("HLTeff_"+ntuplePtl+".png");

}

void eff(){
  calceff("SingleMuon");
}

