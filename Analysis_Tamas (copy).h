#include "TLorentzVector.h"
#include "TMath.h"
#include "include/AnalysisBase.h"
#include <fstream>
#include <map>

using namespace std;
//_______________________________________________________
//                  Calculate variables

void
Analysis::calculate_variables(eventBuffer& data, const unsigned int& syst_index)
{
}

//_______________________________________________________
//                Define Skimming cuts
//   (Not needed, unless you want to skim the ntuple)

bool
Analysis::pass_skimming(eventBuffer& data)
{
  int NJetAK8 = 0; 
  for (size_t i=0; i<data.FatJet.size(); ++i) {
    // pt cut intentionally removed to accept all jets for systematics
    if ( (data.FatJet[i].jetId == 2 || data.FatJet[i].jetId == 6) &&
         std::abs(data.FatJet[i].eta)  <  JET_AK8_ETA_CUT ) {
      NJetAK8++;
    }    
  }
  if (!(NJetAK8>=1)) return 0;
  //if (!(R2>=0.04)) return 0;
  return 1;
}

bool
Analysis::pass_duplicate(eventBuffer& data)
{
  bool isMET = TString(sample).Contains("MET");
  if(isMET){
    ifstream f1;
    f1.open("inclue/Duplicate.txt");
    int RunDuplicate, LumiDuplicate;
    ulong EventDuplicate;
    map <pair<int, int>, int> duplicate_map;
    while(1){
      if (!f1.good()) break;
      duplicate_map.insert(make_pair(make_pair(RunDuplicate,LumiDuplicate),EventDuplicate));
    }
    auto it4 = duplicate_map.find(make_pair(data.run,data.luminosityBlock));
    if(it4 != duplicate_map.end()){
      if(data.event == duplicate_map[make_pair(data.run,data.luminosityBlock)]) return 0;
    }
    f1.close();
  } 
  return 1;
}

//_______________________________________________________
//          Define Analysis event selection cuts
//     Can define all sorts of Signal/Control regions

void
Analysis::define_selections(const eventBuffer& d)
{
  analysis_cuts.clear();

  // Define here cuts that are include in all Signal/Control regions
  // MET Filters, etc. are already applied in AnalysisBase.h, See baseline_cuts

  // cut0: signal mass region
/*
bool isT2tt = TString(sample).Contains("T2tt");
  if(isT2tt){ 
  baseline_cuts.push_back({ .name="signal_mass_selection",   .func = [&d]{ 
            //return d.evt.SUSY_Gluino_Mass==mGluino[num] && d.evt.SUSY_LSP_Mass==mLSP[num];
            //return d.evt.SUSY_Gluino_Mass==2000 && d.evt.SUSY_LSP_Mass==300;
            return d.evt.SUSY_Stop_Mass==1100 && d.evt.SUSY_LSP_Mass==0;
	    } });}
bool isT5tttt = TString(sample).Contains("T5tttt");
  if(isT5tttt){ 
  baseline_cuts.push_back({ .name="signal_mass_selection",   .func = [&d]{ 
            //return d.evt.SUSY_Gluino_Mass==mGluino[num] && d.evt.SUSY_LSP_Mass==mLSP[num];
            return d.evt.SUSY_Gluino_Mass==1400 && d.evt.SUSY_LSP_Mass==300;
            //return d.evt.SUSY_Stop_Mass==800 && d.evt.SUSY_LSP_Mass==100;
	    } });}
bool isT1tttt = TString(sample).Contains("T1tttt");
  if(isT1tttt){ 
  baseline_cuts.push_back({ .name="signal_mass_selection",   .func = [&d]{ 
            return d.evt.SUSY_Gluino_Mass==1400 && d.evt.SUSY_LSP_Mass==300;
	    } });}
bool isT5ttcc = TString(sample).Contains("T5ttcc");
  if(isT5ttcc){ 
  baseline_cuts.push_back({ .name="signal_mass_selection",   .func = [&d]{ 
            return d.evt.SUSY_Gluino_Mass==1700 && d.evt.SUSY_LSP_Mass==300;
	    } });}
*/

  bool isMET   = TString(sample).Contains("MET");
  bool isJetHT = TString(sample).Contains("JetHT");
  // preselection
  analysis_cuts['P'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['P'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); // Separate cut, so one can exclude (N-1)
  analysis_cuts['P'].push_back({ .name="MR_R2",    .func = [&d]  { return MR>=800 && R2>=0.08;     }});
  
  if(isJetHT) analysis_cuts['P'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['P'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['P'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});

#if TOP == 0
  // S: Signal region
  analysis_cuts['S'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['S'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); // Separate cut, so one can exclude (N-1)
  analysis_cuts['S'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['S'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['S'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['S'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['S'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['S'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
  //analysis_cuts['S'].push_back({ .name="0IsoTrk",    .func = [&d]  { return d.evt.NIsoTrk==0;                 }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['S'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['S'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['S'].push_back({ .name="1b",         .func = []    { return nMediumBTag>=1;                   }});
  analysis_cuts['S'].push_back({ .name="1W",         .func = []    { return nTightWTag>=1;                    }});
  //analysis_cuts['S'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['S'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});

  // S': DPhi Control region of Signal region
  analysis_cuts['s'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['s'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); // Separate cut, so one can exclude (N-1)
  analysis_cuts['s'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['s'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['s'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['s'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['s'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['s'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
  //analysis_cuts['s'].push_back({ .name="0IsoTrk",    .func = [&d]  { return d.evt.NIsoTrk==0;                 }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['s'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['s'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['s'].push_back({ .name="1b",         .func = []    { return nMediumBTag>=1;                   }});
  analysis_cuts['s'].push_back({ .name="1W",         .func = []    { return nTightWTag>=1;                    }});
  //analysis_cuts['s'].push_back({ .name="InvmDPhi",   .func = []    { return minDeltaPhi<0.5;                  }});
  analysis_cuts['s'].push_back({ .name="InvmDPhi",   .func = []    { return dPhiRazor>=2.8;                  }});

  // Q: QCD enriched control sample
  analysis_cuts['Q'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['Q'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['Q'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['Q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['Q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['Q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['Q'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['Q'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['Q'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['Q'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['Q'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['Q'].push_back({ .name="1aW",        .func = []    { return nTightWAntiTag>=1;                }});
  //analysis_cuts['Q'].push_back({ .name="InvmDPhi",.func = []    { return minDeltaPhi<0.3;                  }});
  analysis_cuts['Q'].push_back({ .name="InvmDPhi",.func = []    { return dPhiRazor>=2.8;                  }});

  // Q': Dphi Control region of QCD enriched sample
  analysis_cuts['q'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['q'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['q'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['q'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['q'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['q'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['q'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['q'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['q'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['q'].push_back({ .name="1aW",        .func = []    { return nTightWAntiTag>=1;                }});
  //analysis_cuts['q'].push_back({ .name="mDPhi",.func = []    { return minDeltaPhi>=0.5;                  }});
  analysis_cuts['q'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});

  // T: Top enriched control sample
  analysis_cuts['T'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['T'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['T'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['T'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['T'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['T'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['T'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['T'].push_back({ .name="1b",         .func = []    { return nMediumBTag>=1;                   }});
  analysis_cuts['T'].push_back({ .name="1W",         .func = []    { return nTightWTag>=1;                    }});
  //analysis_cuts['T'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['T'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});
  analysis_cuts['T'].push_back({ .name="MT",         .func = []    { return MT_vetolep<100;                   }});

  // W: W enriched control sample
  analysis_cuts['W'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['W'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['W'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  
  if(isJetHT) analysis_cuts['W'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['W'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['W'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['W'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['W'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['W'].push_back({ .name="1mW",        .func = []    { return nWMassTag>=1;                     }});
  //analysis_cuts['W'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['W'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});
  analysis_cuts['W'].push_back({ .name="MT",         .func = []    { return MT_vetolep>=30 && MT_vetolep<100; }});

  // L: 1-lepton invisible control sample with veto lepton
  analysis_cuts['L'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }});
  analysis_cuts['L'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['L'].push_back({ .name="MR",         .func = [&d]  { return MR>=800;                    }});
  analysis_cuts['L'].push_back({ .name="R2",         .func = [&d]  { return R2_1vl>=0.08;                     }});
  
  if(isJetHT) analysis_cuts['L'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['L'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['L'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['L'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['L'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['L'].push_back({ .name="1mW",        .func = []    { return nWMassTag>=1;                     }});
  //analysis_cuts['L'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi_1vl>=0.5;                 }});
  analysis_cuts['L'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                    }});
  analysis_cuts['L'].push_back({ .name="MT",         .func = []    { return MT_vetolep>=30 && MT_vetolep<100; }});

  // Z: Z->ll enriched control sample
  analysis_cuts['Z'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); // Similar to pt>200, one AK8 jet has pt>200
  analysis_cuts['Z'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); // Separate cut, so one can exclude (N-1)
  analysis_cuts['Z'].push_back({ .name="MR_R2ll",    .func = [&d]  { return MR>=800 && R2_ll>=0.08;     }});
  
  if(isJetHT) analysis_cuts['Z'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['Z'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['Z'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['Z'].push_back({ .name="2Lep",       .func = []    { return (nEleSelect==2&&nMuVeto==0)||(nMuSelect==2&&nEleVeto==0); }});
  analysis_cuts['Z'].push_back({ .name="OppCharge",  .func = [&d]  { 
				   if (nEleSelect==2) return (d.Electron[iEleSelect[0]].charge + d.Electron[iEleSelect[1]].charge)==0;
				   else if (nMuSelect==2) return (d.Muon[iMuSelect[0]].charge  + d.Muon[iMuSelect[1]].charge)==0;
				   return false;
				 }});
  analysis_cuts['Z'].push_back({ .name="1mW",        .func = []    { return nWMassTag>=1;                     }});
  analysis_cuts['Z'].push_back({ .name="mDPhi",    .func = []    { return dPhiRazor<2.8;              }});
  //analysis_cuts['Z'].push_back({ .name="mDPhi",    .func = []    { return minDeltaPhi_ll>=0.5;              }});
  analysis_cuts['Z'].push_back({ .name="Mll",        .func = []    { return std::abs(M_ll-91.2)<10;           }});

  // G: Photon enriched sample
  analysis_cuts['G'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }});
  analysis_cuts['G'].push_back({ .name="1Pho",       .func = []    { return nPhotonSelect==1;                 }     });
  analysis_cuts['G'].push_back({ .name="NJet",       .func = []    { return nJetNoPho>=3;                     }});
  analysis_cuts['G'].push_back({ .name="MR",         .func = [&d]  { return MR_pho>=800;                    }});
  analysis_cuts['G'].push_back({ .name="R2",         .func = [&d]  { return R2_pho>=0.08;                     }});
  
  if(isJetHT) analysis_cuts['G'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['G'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['G'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['G'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['G'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['G'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['G'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['G'].push_back({ .name="1mW",        .func = []    { return nWMassTag>=1;                     }});
  //analysis_cuts['G'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi_pho>=0.5;             }});
  analysis_cuts['G'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazorNoPho<2.8;                    }});

  // z: Z->ll enriched control sample (with full tag)
  analysis_cuts['z'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['z'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['z'].push_back({ .name="MR",         .func = [&d]  { return MR>=800;                    }}); 
  analysis_cuts['z'].push_back({ .name="R2ll",       .func = [&d]  { return R2_ll>=0.08;                      }}); 
  analysis_cuts['z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  
  analysis_cuts['z'].push_back({ .name="2Lep",       .func = []    { return (nEleSelect==2&&nMuVeto==0)||(nMuSelect==2&&nEleVeto==0); }});
  analysis_cuts['z'].push_back({ .name="OppCharge",  .func = [&d]  { 
           if (nEleSelect==2) return (d.Electron[iEleSelect[0]].charge + d.Electron[iEleSelect[1]].charge)==0;
           else if (nMuSelect==2) return (d.Muon[iMuSelect[0]].charge + d.Muon[iMuSelect[1]].charge)==0;
           return false;
         }}); 
  analysis_cuts['z'].push_back({ .name="1W",         .func = []    { return nTightWTag>=1;                    }}); 
//analysis_cuts['z'].push_back({ .name="1mW",        .func = []    { return nWMassTag>=1;                     }});
  analysis_cuts['z'].push_back({ .name="mDPhi",    .func = []    { return dPhiRazor<2.8;                    }}); 
  analysis_cuts['z'].push_back({ .name="Mll",        .func = []    { return std::abs(M_ll-91.2)<10;           }}); 

#else

  // t: Boosted Top Signal region
  analysis_cuts['S'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['S'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['S'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['S'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['S'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['S'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['S'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['S'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['S'].push_back({ .name="1Top",       .func = []    { return nHadTopTag>=1;                    }});
  analysis_cuts['S'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});

  // a: Boosted Top' Signal region
  analysis_cuts['s'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['s'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['s'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['s'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['s'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['s'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['s'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['s'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['s'].push_back({ .name="1Top",       .func = []    { return nHadTopTag>=1;                    }});
  //analysis_cuts['s'].push_back({ .name="InvmDPhi",      .func = []    { return minDeltaPhi<0.5;                 }});
  analysis_cuts['s'].push_back({ .name="InvmDPhi",      .func = []    { return dPhiRazor>=2.8;                 }});

  // t: Boosted Top Q region
  analysis_cuts['Q'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['Q'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['Q'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['Q'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['Q'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['Q'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['Q'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['Q'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['Q'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['Q'].push_back({ .name="1aTop",       .func = []   { return nHadTop0BAntiTag>=1;                    }});
  //analysis_cuts['Q'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi<0.3;                 }});
  analysis_cuts['Q'].push_back({ .name="InvmDPhi",      .func = []    { return dPhiRazor>=2.8;                 }});

  // t: Boosted Top Q' region
  analysis_cuts['q'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['q'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['q'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['q'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['q'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['q'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['q'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['q'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['q'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['q'].push_back({ .name="1aTop",       .func = []   { return nHadTop0BAntiTag>=1;                    }});
  //analysis_cuts['q'].push_back({ .name="InvmDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['q'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});

  // t: Boosted Top T region
  analysis_cuts['T'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['T'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['T'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['T'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['T'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['T'].push_back({ .name="1Top",       .func = []   { return nHadTopTag>=1;                    }});
  //analysis_cuts['T'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['T'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});
  analysis_cuts['T'].push_back({ .name="MT",         .func = []    { return     MT_vetolep<100;               }});

  // t: Boosted Top W region
  analysis_cuts['W'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['W'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['W'].push_back({ .name="MR_R2",      .func = [&d]  { return MR>=800 && R2>=0.08;  }});
  //analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['W'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['W'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['W'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['W'].push_back({ .name="1mTop",       .func = []   { return nHadTop0BMassTag>=1;                    }});
  //analysis_cuts['W'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi>=0.5;                 }});
  analysis_cuts['W'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                 }});
  analysis_cuts['W'].push_back({ .name="MT",         .func = []    { return MT_vetolep>=30 && MT_vetolep<100; }});

  // L: 1-lepton invisible control sample with veto lepton
  analysis_cuts['L'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }});
  analysis_cuts['L'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['L'].push_back({ .name="MR",         .func = [&d]  { return MR>=800;                    }});
  analysis_cuts['L'].push_back({ .name="R2",         .func = [&d]  { return R2_1vl>=0.08;                     }});
  //analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['L'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['L'].push_back({ .name="1Lep",       .func = []    { return nLepVeto==1;                      }});
  analysis_cuts['L'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }});
  analysis_cuts['L'].push_back({ .name="1mTop",      .func = []    { return nHadTop0BMassTag>=1;              }});
  analysis_cuts['L'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazor<2.8;                    }});
  analysis_cuts['L'].push_back({ .name="MT",         .func = []    { return MT_vetolep>=30 && MT_vetolep<100; }});

  // Z: Z->ll enriched control sample
  analysis_cuts['Z'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                      }}); 
  analysis_cuts['Z'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }});
  analysis_cuts['Z'].push_back({ .name="MR_R2ll",    .func = [&d]  { return MR>=800 && R2_ll>=0.08;     }});
  //analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['Z'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['Z'].push_back({ .name="2Lep",       .func = []    { return (nEleSelect==2&&nMuVeto==0)||(nMuSelect==2&&nEleVeto==0); }});
  analysis_cuts['Z'].push_back({ .name="OppCharge",  .func = [&d]  { 
				   if (nEleSelect==2) return (d.Electron[iEleSelect[0]].charge + d.Electron[iEleSelect[1]].charge)==0;
				   else if (nMuSelect==2) return (d.Muon[iMuSelect[0]].charge + d.Muon[iMuSelect[1]].charge)==0;
				   return false;
				 }});
  analysis_cuts['Z'].push_back({ .name="1mTop",        .func = []    { return nHadTopMassTag>=1;                     }});
  analysis_cuts['Z'].push_back({ .name="mDPhi",    .func = []    { return dPhiRazor<2.8;              }});
  analysis_cuts['Z'].push_back({ .name="Mll",        .func = []    { return std::abs(M_ll-91.2)<10;           }});
  // G: Photon enriched sample
  analysis_cuts['G'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }});
  analysis_cuts['G'].push_back({ .name="1Pho",       .func = []    { return nPhotonSelect==1;                 }});
  analysis_cuts['G'].push_back({ .name="NJet",       .func = []    { return nJetNoPho>=3;                     }});
  analysis_cuts['G'].push_back({ .name="MR",         .func = [&d]  { return MR_pho>=800;                      }});
  analysis_cuts['G'].push_back({ .name="R2",         .func = [&d]  { return R2_pho>=0.08;                     }});
  //analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 : 1; }});
  //analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  //analysis_cuts['G'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['G'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }});
  analysis_cuts['G'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }});
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['G'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }});  
#else  
  analysis_cuts['G'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif 
  analysis_cuts['G'].push_back({ .name="1mTop",      .func = []    { return nHadTopMassTag>=1;                }});
//analysis_cuts['G'].push_back({ .name="mDPhi",      .func = []    { return minDeltaPhi_pho>=0.5;             }});
  analysis_cuts['G'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazorNoPho<2.8;               }});

#endif

  // G-1: Photon enriched sample (G without boosted object)
  analysis_cuts['g'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['g'].push_back({ .name="1Pho",       .func = []    { return nPhotonSelect==1;                 }}); 
  analysis_cuts['g'].push_back({ .name="NJet",       .func = []    { return nJetNoPho>=3;                     }}); 
  analysis_cuts['g'].push_back({ .name="MR",         .func = [&d]  { return MR_pho>=800;                      }}); 
  analysis_cuts['g'].push_back({ .name="R2",         .func = [&d]  { return R2_pho>=0.08;                     }}); 
  analysis_cuts['g'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['g'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }}); 
  //analysis_cuts['g'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['g'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }}); 
  analysis_cuts['g'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }}); 
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['g'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }}); 
#else
  analysis_cuts['g'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }}); 
#endif
  analysis_cuts['g'].push_back({ .name="mDPhi",      .func = []    { return dPhiRazorNoPho<2.8;               }}); 

  // w: Inclusive Razor-like W enriched region
  analysis_cuts['w'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['w'].push_back({ .name="MR",         .func = [&d]  { return MR>=300;                    }}); 
  analysis_cuts['w'].push_back({ .name="R2",         .func = [&d]  { return R2>=0.15;                   }}); 
  analysis_cuts['w'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['w'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }}); 
  //analysis_cuts['w'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['w'].push_back({ .name="1Lep",       .func = []    { return nLepSelect==1;                    }}); 
  analysis_cuts['w'].push_back({ .name="MET",        .func = [&d]  { return d.MET_pt>=30;                  }}); 
  analysis_cuts['w'].push_back({ .name="MT",         .func = []    { return MT>=30 && MT<100;                 }}); 
  analysis_cuts['w'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }}); 

  // F: Fake rate measurement region
  analysis_cuts['F'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['F'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); 
  analysis_cuts['F'].push_back({ .name="MR",         .func = [&d]  { return MR>=800;                    }}); 
  if(isJetHT) analysis_cuts['F'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['F'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['F'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['F'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }}); 
  analysis_cuts['F'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }}); 
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['F'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }}); 
#else
  analysis_cuts['F'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }}); 
#endif
  analysis_cuts['F'].push_back({ .name="0b",         .func = []    { return nLooseBTag==0;                    }}); 
//analysis_cuts['F'].push_back({ .name="InvmDPhi0p3",.func = []    { return minDeltaPhi<0.3;                  }});
  analysis_cuts['F'].push_back({ .name="InvmDPhi0p3",.func = []    { return dPhiRazor>=2.8;                   }}); 

  // F: F+1b
  analysis_cuts['f'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['f'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); 
  analysis_cuts['f'].push_back({ .name="MR",         .func = [&d]  { return MR>=800;                    }}); 
  if(isJetHT) analysis_cuts['f'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }});
  else if(isMET) analysis_cuts['f'].push_back({ .name="HLT",.func = [this,&d] { return isData ? !(d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1) && d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  else      analysis_cuts['f'].push_back({ .name="HLT",.func = [this,&d] { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  analysis_cuts['f'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }}); 
  analysis_cuts['f'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }}); 
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['f'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }}); 
#else
  analysis_cuts['f'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }}); 
#endif
  analysis_cuts['f'].push_back({ .name="1b",         .func = []    { return nMediumBTag>=1;                   }}); 
//analysis_cuts['f'].push_back({ .name="InvmDPhi0p3",.func = []    { return minDeltaPhi<0.3;                  }});
  analysis_cuts['f'].push_back({ .name="InvmDPhi0p3",.func = []    { return dPhiRazor>=2.8;                   }}); 

  // H: HT>=1TeV+1b
  analysis_cuts['H'].push_back({ .name="1JetAK8",    .func = []    { return nJetAK8>=1;                       }}); 
  analysis_cuts['H'].push_back({ .name="NJet",       .func = []    { return nJet>=3;                          }}); 
  analysis_cuts['H'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 : 1; }});
  //analysis_cuts['H'].push_back({ .name="HLT",   .func = [this,&d]  { return isData ? d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1 : 1; }}); 
  //analysis_cuts['H'].push_back({ .name="HLT",   .func = [this,&d]  { return d.HLT_PFHT1050==1 || d.HLT_AK8PFJet400_TrimMass30==1 || d.HLT_PFHT500_PFMET100_PFMHT100_IDTight==1 || d.HLT_AK8PFJet450==1 || d.HLT_AK8PFJet400_TrimMass30==1; }});
  analysis_cuts['H'].push_back({ .name="0Ele",       .func = []    { return nEleVeto==0;                      }}); 
  analysis_cuts['H'].push_back({ .name="0Mu",        .func = []    { return nMuVeto==0;                       }}); 
#if USE_ISO_TRK_VETO > 0
  analysis_cuts['H'].push_back({ .name="0Tau",       .func = [&d]  { return d.evt.NIsoTrk==0;                 }}); 
#else
  analysis_cuts['H'].push_back({ .name="0Tau",       .func = []    { return nTauVeto==0;                      }});
#endif
  analysis_cuts['H'].push_back({ .name="1b",         .func = []    { return nMediumBTag>=1;                   }});
  analysis_cuts['H'].push_back({ .name="HT",         .func = []    { return AK4_Ht>=1000;                     }});
}

//____________________________________________________
//          Analysis Specific Scale factors
//    (Defined for each search region separately)

void
Analysis::apply_scale_factors(eventBuffer& data, const unsigned int& s, const std::vector<std::vector<double> >& nSigmaSFs)
{
//   bool isFastSim = TString(sample).Contains("FastSim");
//   size_t i = 0;
// 
//   // Don't forget to specify the total number of sigmas you use in settings_*.h !
// 
//   // Electron SFs (5 sigmas - reco, fullsim id/iso, fastsim)
//   double sf_ele_veto, sf_ele_loose, sf_ele_medium;
//   std::tie(sf_ele_veto, sf_ele_loose, sf_ele_medium) = calc_ele_sf(data, nSigmaSFs[i][s], nSigmaSFs[i+1][s], nSigmaSFs[i+2][s], nSigmaSFs[i+3][s], isFastSim);
//   i+=4;
// 
//   // Muon SFs (3 sigmas - tracking, fullsim, fastsim)
//   double sf_muon_veto, sf_muon_loose, sf_muon_medium;
//   std::tie(sf_muon_veto, sf_muon_loose, sf_muon_medium) =  calc_muon_sf(data, nSigmaSFs[i][s], nSigmaSFs[i+1][s], nSigmaSFs[i+2][s], isFastSim);
//   i+=3;
// 
//   // b tagging SFs (2 sigma - fullsim, fastsim)
//   std::pair<double, double> sf_btag = calc_b_tagging_sf(data, nSigmaSFs[i][s], nSigmaSFs[i+1][s], isFastSim);
//   double sf_btag_loose = sf_btag.first, sf_btag_medium = sf_btag.second;
//   i+=2;
// 
// #if TOP == 0
//   // W tagging SF  (4 sigma - fullsim, fastsim, mistag, mistag fastsim)
//   double sf_w = calc_w_tagging_sf(data, nSigmaSFs[i][s], nSigmaSFs[i+1][s], nSigmaSFs[i+2][s], nSigmaSFs[i+3][s], isFastSim);
//   // fake W tagging SFs (2 sigma)
//   double sf_fake_mW = calc_fake_w_mass_tagging_sf(data, nSigmaSFs[i+4][s]);
//   double sf_fake_aW = calc_fake_w_anti_tagging_sf(data, nSigmaSFs[i+5][s]);
// #endif
//   i+=6;
// 
// #if TOP != 0
//   // top tagging SF (4 sigma - fullsim, fastsim, mistag, mistag fastsim)
//   double sf_top = calc_top_tagging_sf(data, nSigmaSFs[i][s], nSigmaSFs[i+1][s], nSigmaSFs[i+2][s], nSigmaSFs[i+3][s], isFastSim);
//   // fake top tagging SFs (3 sigmas)
//   double sf_fake_0b_mTop = calc_fake_top_0b_mass_tagging_sf(data, nSigmaSFs[i+4][s]);
//   double sf_fake_MTop = calc_fake_top_mass_tagging_sf(data, nSigmaSFs[i+5][s]);
//   double sf_fake_aTop = calc_fake_top_anti_tagging_sf(data, nSigmaSFs[i+6][s]);
// #endif
//   i+=7;

  double sf_ele_veto=1,  sf_ele_medium=1;
  double sf_muon_veto=1, sf_muon_medium=1;
  double sf_btag_loose = 1, sf_btag_medium = 1;
  double sf_fake_mW=1, sf_fake_aW=1, sf_w=1;
  double sf_top=1, sf_fake_0b_mTop=1, sf_fake_MTop=1, sf_fake_aTop=1;

  // Select scale factors to use
  for (auto& sf : scale_factors) sf.second.clear();

#if TOP == 0

  scale_factors['S'].push_back(sf_ele_veto);
  scale_factors['S'].push_back(sf_muon_veto);
  scale_factors['S'].push_back(sf_btag_medium);
  scale_factors['S'].push_back(sf_w);

  scale_factors['s'] = scale_factors['S'];

  scale_factors['Q'].push_back(sf_ele_veto);
  scale_factors['Q'].push_back(sf_muon_veto);
  scale_factors['Q'].push_back(sf_btag_loose);
  scale_factors['Q'].push_back(sf_fake_aW);

  scale_factors['q'] = scale_factors['Q'];

  scale_factors['T'].push_back(sf_ele_veto);
  scale_factors['T'].push_back(sf_muon_veto);
  scale_factors['T'].push_back(sf_btag_medium);
  scale_factors['T'].push_back(sf_w);

  scale_factors['W'].push_back(sf_ele_veto);
  scale_factors['W'].push_back(sf_muon_veto);
  scale_factors['W'].push_back(sf_btag_loose);
  scale_factors['W'].push_back(sf_fake_mW);

  scale_factors['L'] = scale_factors['W'];

  scale_factors['Z'].push_back(sf_ele_medium);
  scale_factors['Z'].push_back(sf_muon_medium);
  scale_factors['Z'].push_back(sf_fake_mW);

  scale_factors['G'].push_back(sf_ele_veto);
  scale_factors['G'].push_back(sf_muon_veto);
  scale_factors['G'].push_back(sf_fake_mW);

  scale_factors['z'].push_back(sf_ele_medium);
  scale_factors['z'].push_back(sf_muon_medium);
  scale_factors['z'].push_back(sf_w);

#else

  // Top Analysis
  scale_factors['S'].push_back(sf_ele_veto);
  scale_factors['S'].push_back(sf_muon_veto);
  scale_factors['S'].push_back(sf_top);

  scale_factors['s'] = scale_factors['S'];

  scale_factors['Q'].push_back(sf_ele_veto);
  scale_factors['Q'].push_back(sf_muon_veto);
  scale_factors['Q'].push_back(sf_fake_aTop);
  scale_factors['Q'].push_back(sf_btag_loose);

  scale_factors['q'] = scale_factors['Q'];

  scale_factors['T'].push_back(sf_ele_veto);
  scale_factors['T'].push_back(sf_muon_veto);
  scale_factors['T'].push_back(sf_top);

  scale_factors['W'].push_back(sf_ele_veto);
  scale_factors['W'].push_back(sf_muon_veto);
  scale_factors['W'].push_back(sf_fake_0b_mTop);
  scale_factors['W'].push_back(sf_btag_loose);

  scale_factors['G'].push_back(sf_ele_veto);
  scale_factors['G'].push_back(sf_muon_veto);
  scale_factors['G'].push_back(sf_fake_MTop);

  scale_factors['Z'].push_back(sf_ele_medium);
  scale_factors['Z'].push_back(sf_muon_medium);
  scale_factors['Z'].push_back(sf_fake_MTop);

  scale_factors['L'] = scale_factors['W'];

#endif

  scale_factors['F'].push_back(sf_ele_veto);
  scale_factors['F'].push_back(sf_muon_veto);
  scale_factors['F'].push_back(sf_btag_loose);
  scale_factors['f'].push_back(sf_ele_veto);
  scale_factors['f'].push_back(sf_muon_veto);
  scale_factors['f'].push_back(sf_btag_medium);

}

//_______________________________________________________
//                 Signal Region
//     Must define it, because we blind it in data!

bool
Analysis::signal_selection(const eventBuffer& data) {
  //return apply_all_cuts('S');
  return 0;
}

//_______________________________________________________
//                 List of Histograms

TH1D* h_dR_min;
TH1D* h_rel_pT;
TH2D* h_rel_pT_dR_min;
TH1D* h_dR_min_tag;
TH1D* h_rel_pT_tag;
TH2D* h_rel_pT_dR_min_tag;
TH1D* h_dR_min_untag;
TH1D* h_rel_pT_untag;
TH2D* h_rel_pT_dR_min_untag;

TH1D* h_MR_S_1Lep;
TH1D* h_R2_S_1Lep;
TH2D* h_R2_MR_S_1Lep;
TH1D* h_MR_T_1Lep;
TH1D* h_R2_T_1Lep;
TH2D* h_R2_MR_T_1Lep;
TH1D* h_MR_W_1Lep;
TH1D* h_R2_W_1Lep;
TH2D* h_R2_MR_W_1Lep;


TH1D* h_ht_AK4;
TH1D* h_njet;
TH1D* h_nAK8jet;
TH1D* h_nbTight;
TH1D* h_nbMedium;
TH1D* h_nbLoose ;
TH1D* h_nw;
TH1D* h_nEleVeto;
TH1D* h_nEleTight;
TH1D* h_nEleMedium;
TH1D* h_nEleLoose;
TH1D* h_nMuTight;
TH1D* h_nMuMedium;
TH1D* h_nMuSoft;
TH1D* h_nTauVeto;
TH1D* h_nIsoTrack;
TH1D* h_nPhoMedium;

TH1D* h_ele_pt_T;
TH1D* h_ele_pt_W;
TH1D* h_ele_pt_L;
TH1D* h_mu_pt_T;
TH1D* h_mu_pt_W;
TH1D* h_mu_pt_L;

TH1D* h_ht_AK4_S;
TH1D* h_jet1_pt_S;
TH1D* h_MR_S;
TH1D* h_R2_S;
TH1D* h_tau21_S;
TH1D* h_tau32_S;
TH1D* h_MET_S;
TH1D* h_AK8_jet1_pt_S;
TH1D* h_AK8_jet1_eta_S;
TH1D* h_MET_phi_S;
TH1D* h_AK8_jet1_pt_S_nj35;
TH1D* h_AK8_jet1_eta_S_nj35;
TH1D* h_MET_phi_S_nj35;
TH1D* h_AK8_jet1_pt_S_nj6;
TH1D* h_AK8_jet1_eta_S_nj6;
TH1D* h_MET_phi_S_nj6;

TH1D* h_ht_AK4_S_nj35;
TH1D* h_jet1_pt_S_nj35;
TH1D* h_MR_S_nj35;
TH1D* h_R2_S_nj35;
TH1D* h_tau21_S_nj35;
TH1D* h_tau32_S_nj35;
TH1D* h_MET_S_nj35;

TH1D* h_ht_AK4_S_nj6;
TH1D* h_jet1_pt_S_nj6;
TH1D* h_MR_S_nj6;
TH1D* h_R2_S_nj6;
TH1D* h_tau21_S_nj6;
TH1D* h_tau32_S_nj6;
TH1D* h_MET_S_nj6;

TH1D* h_ht_AK4_s;
TH1D* h_jet1_pt_s;
TH1D* h_MR_s;
TH1D* h_R2_s;
TH1D* h_tau21_s;
TH1D* h_tau32_s;
TH1D* h_MET_s;
TH1D* h_AK8_jet1_pt_s;
TH1D* h_AK8_jet1_eta_s;
TH1D* h_MET_phi_s;
TH1D* h_AK8_jet1_pt_s_nj35;
TH1D* h_AK8_jet1_eta_s_nj35;
TH1D* h_MET_phi_s_nj35;
TH1D* h_AK8_jet1_pt_s_nj6;
TH1D* h_AK8_jet1_eta_s_nj6;
TH1D* h_MET_phi_s_nj6;
TH1D* h_mDPhi_nomDPhi_s;
TH1D* h_mDPhi_nomDPhi_s_nj35;
TH1D* h_mDPhi_nomDPhi_s_nj6;
#if TOP == 0
TH1D* h_nW_nonW_s;
TH1D* h_nW_nonW_s_nj35;
TH1D* h_nW_nonW_s_nj6;

TH1D* h_nb_nonb_s;
TH1D* h_nb_nonb_s_nj35;
TH1D* h_nb_nonb_s_nj6;
#else
TH1D* h_nTop_nonTop_s;
TH1D* h_nTop_nonTop_s_nj35;
TH1D* h_nTop_nonTop_s_nj6;
#endif

#if TOP == 0
TH1D* h_nW_nonW_q;
TH1D* h_nW_nonW_q_nj35;
TH1D* h_nW_nonW_q_nj6;
#else
TH1D* h_nTop_nonTop_q;
TH1D* h_nTop_nonTop_q_nj35;
TH1D* h_nTop_nonTop_q_nj6;
#endif
TH1D* h_nb_nonb_q;
TH1D* h_nb_nonb_q_nj35;
TH1D* h_nb_nonb_q_nj6;

TH1D* h_ht_AK4_s_nj35;
TH1D* h_jet1_pt_s_nj35;
TH1D* h_MR_s_nj35;
TH1D* h_R2_s_nj35;
TH1D* h_tau21_s_nj35;
TH1D* h_tau32_s_nj35;
TH1D* h_MET_s_nj35;

TH1D* h_ht_AK4_s_nj6;
TH1D* h_jet1_pt_s_nj6;
TH1D* h_MR_s_nj6;
TH1D* h_R2_s_nj6;
TH1D* h_tau21_s_nj6;
TH1D* h_tau32_s_nj6;
TH1D* h_MET_s_nj6;

TH1D* h_ht_AK4_G;
TH1D* h_jet1_pt_G;
TH1D* h_MR_G;
TH1D* h_R2_G;
TH1D* h_tau21_G;
TH1D* h_tau32_G;
TH1D* h_MET_G;
TH1D* h_AK8_jet1_pt_G;
TH1D* h_AK8_jet1_eta_G;
TH1D* h_MET_phi_G;
TH1D* h_AK8_jet1_pt_G_nj35;
TH1D* h_AK8_jet1_eta_G_nj35;
TH1D* h_MET_phi_G_nj35;
TH1D* h_AK8_jet1_pt_G_nj6;
TH1D* h_AK8_jet1_eta_G_nj6;
TH1D* h_MET_phi_G_nj6;

TH1D* h_ht_AK4_G_nj35;
TH1D* h_jet1_pt_G_nj35;
TH1D* h_MR_G_nj35;
TH1D* h_R2_G_nj35;
TH1D* h_tau21_G_nj35;
TH1D* h_tau32_G_nj35;
TH1D* h_MET_G_nj35;

TH1D* h_ht_AK4_G_nj6;
TH1D* h_jet1_pt_G_nj6;
TH1D* h_MR_G_nj6;
TH1D* h_R2_G_nj6;
TH1D* h_tau21_G_nj6;
TH1D* h_tau32_G_nj6;
TH1D* h_MET_G_nj6;

TH1D* h_ht_AK4_no0Lep_G;
TH1D* h_jet1_pt_no0Lep_G;
TH1D* h_MR_no0Lep_G;
TH1D* h_R2_no0Lep_G;
TH1D* h_MET_no0Lep_G;
TH1D* h_nLep_no0Lep_G;

TH1D* h_ht_AK4_no1Pho_G;
TH1D* h_jet1_pt_no1Pho_G;
TH1D* h_MR_no1Pho_G;
TH1D* h_R2_no1Pho_G;
TH1D* h_MET_no1Pho_G;
TH1D* h_nPho_no1Pho_G;

TH1D* h_ht_AK4_nomDPhi_G;
TH1D* h_jet1_pt_nomDPhi_G;
TH1D* h_MR_nomDPhi_G;
TH1D* h_R2_nomDPhi_G;
TH1D* h_MET_nomDPhi_G;
TH1D* h_mDPhi_nomDPhi_G;

TH1D* h_ht_AK4_no0Lep_G_nj35;
TH1D* h_jet1_pt_no0Lep_G_nj35;
TH1D* h_MR_no0Lep_G_nj35;
TH1D* h_R2_no0Lep_G_nj35;
TH1D* h_MET_no0Lep_G_nj35;
TH1D* h_nLep_no0Lep_G_nj35;

TH1D* h_ht_AK4_no1Pho_G_nj35;
TH1D* h_jet1_pt_no1Pho_G_nj35;
TH1D* h_MR_no1Pho_G_nj35;
TH1D* h_R2_no1Pho_G_nj35;
TH1D* h_MET_no1Pho_G_nj35;
TH1D* h_nPho_no1Pho_G_nj35;

TH1D* h_ht_AK4_nomDPhi_G_nj35;
TH1D* h_jet1_pt_nomDPhi_G_nj35;
TH1D* h_MR_nomDPhi_G_nj35;
TH1D* h_R2_nomDPhi_G_nj35;
TH1D* h_MET_nomDPhi_G_nj35;
TH1D* h_mDPhi_nomDPhi_G_nj35;

TH1D* h_ht_AK4_no0Lep_G_nj6;
TH1D* h_jet1_pt_no0Lep_G_nj6;
TH1D* h_MR_no0Lep_G_nj6;
TH1D* h_R2_no0Lep_G_nj6;
TH1D* h_MET_no0Lep_G_nj6;
TH1D* h_nLep_no0Lep_G_nj6;

TH1D* h_ht_AK4_no1Pho_G_nj6;
TH1D* h_jet1_pt_no1Pho_G_nj6;
TH1D* h_MR_no1Pho_G_nj6;
TH1D* h_R2_no1Pho_G_nj6;
TH1D* h_MET_no1Pho_G_nj6;
TH1D* h_nPho_no1Pho_G_nj6;

TH1D* h_ht_AK4_nomDPhi_G_nj6;
TH1D* h_jet1_pt_nomDPhi_G_nj6;
TH1D* h_MR_nomDPhi_G_nj6;
TH1D* h_R2_nomDPhi_G_nj6;
TH1D* h_MET_nomDPhi_G_nj6;
TH1D* h_mDPhi_nomDPhi_G_nj6;

#if TOP == 0
TH1D* h_ht_AK4_nonW_G;
TH1D* h_jet1_pt_nonW_G;
TH1D* h_MR_nonW_G;
TH1D* h_R2_nonW_G;
TH1D* h_MET_nonW_G;
TH1D* h_nW_nonW_G;

TH1D* h_ht_AK4_nonW_G_nj35;
TH1D* h_jet1_pt_nonW_G_nj35;
TH1D* h_MR_nonW_G_nj35;
TH1D* h_R2_nonW_G_nj35;
TH1D* h_MET_nonW_G_nj35;
TH1D* h_nW_nonW_G_nj35;

TH1D* h_ht_AK4_nonW_G_nj6;
TH1D* h_jet1_pt_nonW_G_nj6;
TH1D* h_MR_nonW_G_nj6;
TH1D* h_R2_nonW_G_nj6;
TH1D* h_MET_nonW_G_nj6;
TH1D* h_nW_nonW_G_nj6;
#else
TH1D* h_ht_AK4_nonTop_G;
TH1D* h_jet1_pt_nonTop_G;
TH1D* h_MR_nonTop_G;
TH1D* h_R2_nonTop_G;
TH1D* h_MET_nonTop_G;
TH1D* h_nTop_nonTop_G;

TH1D* h_ht_AK4_nonTop_G_nj35;
TH1D* h_jet1_pt_nonTop_G_nj35;
TH1D* h_MR_nonTop_G_nj35;
TH1D* h_R2_nonTop_G_nj35;
TH1D* h_MET_nonTop_G_nj35;
TH1D* h_nTop_nonTop_G_nj35;

TH1D* h_ht_AK4_nonTop_G_nj6;
TH1D* h_jet1_pt_nonTop_G_nj6;
TH1D* h_MR_nonTop_G_nj6;
TH1D* h_R2_nonTop_G_nj6;
TH1D* h_MET_nonTop_G_nj6;
TH1D* h_nTop_nonTop_G_nj6;
#endif
TH1D* h_ht_AK4_Z;
TH1D* h_jet1_pt_Z;
TH1D* h_MR_Z;
TH1D* h_R2_Z;
TH1D* h_tau21_Z;
TH1D* h_tau32_Z;
TH1D* h_MET_Z;
TH1D* h_AK8_jet1_pt_Z;
TH1D* h_AK8_jet1_eta_Z;
TH1D* h_MET_phi_Z;
TH1D* h_AK8_jet1_pt_Z_nj35;
TH1D* h_AK8_jet1_eta_Z_nj35;
TH1D* h_MET_phi_Z_nj35;
TH1D* h_AK8_jet1_pt_Z_nj6;
TH1D* h_AK8_jet1_eta_Z_nj6;
TH1D* h_MET_phi_Z_nj6;

TH1D* h_ht_AK4_Z_nj35;
TH1D* h_jet1_pt_Z_nj35;
TH1D* h_MR_Z_nj35;
TH1D* h_R2_Z_nj35;
TH1D* h_tau21_Z_nj35;
TH1D* h_tau32_Z_nj35;
TH1D* h_MET_Z_nj35;

TH1D* h_ht_AK4_Z_nj6;
TH1D* h_jet1_pt_Z_nj6;
TH1D* h_MR_Z_nj6;
TH1D* h_R2_Z_nj6;
TH1D* h_tau21_Z_nj6;
TH1D* h_tau32_Z_nj6;
TH1D* h_MET_Z_nj6;

TH1D* h_ht_AK4_no2Lep_Z;
TH1D* h_jet1_pt_no2Lep_Z;
TH1D* h_MR_no2Lep_Z;
TH1D* h_R2_no2Lep_Z;
TH1D* h_MET_no2Lep_Z;
TH1D* h_nLep_no2Lep_Z;

TH1D* h_ht_AK4_nomDPhi_Z;
TH1D* h_jet1_pt_nomDPhi_Z;
TH1D* h_MR_nomDPhi_Z;
TH1D* h_R2_nomDPhi_Z;
TH1D* h_MET_nomDPhi_Z;
TH1D* h_mDPhi_nomDPhi_Z;

TH1D* h_ht_AK4_noMll_Z;
TH1D* h_jet1_pt_noMll_Z;
TH1D* h_MR_noMll_Z;
TH1D* h_R2_noMll_Z;
TH1D* h_MET_noMll_Z;
TH1D* h_Mll_noMll_Z;

TH1D* h_ht_AK4_no2Lep_Z_nj35;
TH1D* h_jet1_pt_no2Lep_Z_nj35;
TH1D* h_MR_no2Lep_Z_nj35;
TH1D* h_R2_no2Lep_Z_nj35;
TH1D* h_MET_no2Lep_Z_nj35;
TH1D* h_nLep_no2Lep_Z_nj35;

TH1D* h_ht_AK4_nomDPhi_Z_nj35;
TH1D* h_jet1_pt_nomDPhi_Z_nj35;
TH1D* h_MR_nomDPhi_Z_nj35;
TH1D* h_R2_nomDPhi_Z_nj35;
TH1D* h_MET_nomDPhi_Z_nj35;
TH1D* h_mDPhi_nomDPhi_Z_nj35;

TH1D* h_ht_AK4_noMll_Z_nj35;
TH1D* h_jet1_pt_noMll_Z_nj35;
TH1D* h_MR_noMll_Z_nj35;
TH1D* h_R2_noMll_Z_nj35;
TH1D* h_MET_noMll_Z_nj35;
TH1D* h_Mll_noMll_Z_nj35;

TH1D* h_ht_AK4_no2Lep_Z_nj6;
TH1D* h_jet1_pt_no2Lep_Z_nj6;
TH1D* h_MR_no2Lep_Z_nj6;
TH1D* h_R2_no2Lep_Z_nj6;
TH1D* h_MET_no2Lep_Z_nj6;
TH1D* h_nLep_no2Lep_Z_nj6;

TH1D* h_ht_AK4_noMll_Z_nj6;
TH1D* h_jet1_pt_noMll_Z_nj6;
TH1D* h_MR_noMll_Z_nj6;
TH1D* h_R2_noMll_Z_nj6;
TH1D* h_MET_noMll_Z_nj6;
TH1D* h_Mll_noMll_Z_nj6;

TH1D* h_ht_AK4_nomDPhi_Z_nj6;
TH1D* h_jet1_pt_nomDPhi_Z_nj6;
TH1D* h_MR_nomDPhi_Z_nj6;
TH1D* h_R2_nomDPhi_Z_nj6;
TH1D* h_MET_nomDPhi_Z_nj6;
TH1D* h_mDPhi_nomDPhi_Z_nj6;

#if TOP == 0
TH1D* h_ht_AK4_nonW_Z;
TH1D* h_jet1_pt_nonW_Z;
TH1D* h_MR_nonW_Z;
TH1D* h_R2_nonW_Z;
TH1D* h_MET_nonW_Z;
TH1D* h_nW_nonW_Z;

TH1D* h_ht_AK4_nonW_Z_nj35;
TH1D* h_jet1_pt_nonW_Z_nj35;
TH1D* h_MR_nonW_Z_nj35;
TH1D* h_R2_nonW_Z_nj35;
TH1D* h_MET_nonW_Z_nj35;
TH1D* h_nW_nonW_Z_nj35;

TH1D* h_ht_AK4_nonW_Z_nj6;
TH1D* h_jet1_pt_nonW_Z_nj6;
TH1D* h_MR_nonW_Z_nj6;
TH1D* h_R2_nonW_Z_nj6;
TH1D* h_MET_nonW_Z_nj6;
TH1D* h_nW_nonW_Z_nj6;
#else
TH1D* h_ht_AK4_nonTop_Z;
TH1D* h_jet1_pt_nonTop_Z;
TH1D* h_MR_nonTop_Z;
TH1D* h_R2_nonTop_Z;
TH1D* h_MET_nonTop_Z;
TH1D* h_nTop_nonTop_Z;

TH1D* h_ht_AK4_nonTop_Z_nj35;
TH1D* h_jet1_pt_nonTop_Z_nj35;
TH1D* h_MR_nonTop_Z_nj35;
TH1D* h_R2_nonTop_Z_nj35;
TH1D* h_MET_nonTop_Z_nj35;
TH1D* h_nTop_nonTop_Z_nj35;

TH1D* h_ht_AK4_nonTop_Z_nj6;
TH1D* h_jet1_pt_nonTop_Z_nj6;
TH1D* h_MR_nonTop_Z_nj6;
TH1D* h_R2_nonTop_Z_nj6;
TH1D* h_MET_nonTop_Z_nj6;
TH1D* h_nTop_nonTop_Z_nj6;
#endif

TH1D* h_ht_AK4_L;
TH1D* h_jet1_pt_L;
TH1D* h_MR_L;
TH1D* h_R2_L;
TH1D* h_tau21_L;
TH1D* h_tau32_L;
TH1D* h_MET_L;
TH1D* h_AK8_jet1_pt_L;
TH1D* h_AK8_jet1_eta_L;
TH1D* h_MET_phi_L;
TH1D* h_AK8_jet1_pt_L_nj35;
TH1D* h_AK8_jet1_eta_L_nj35;
TH1D* h_MET_phi_L_nj35;
TH1D* h_AK8_jet1_pt_L_nj6;
TH1D* h_AK8_jet1_eta_L_nj6;
TH1D* h_MET_phi_L_nj6;

TH1D* h_ht_AK4_L_nj35;
TH1D* h_jet1_pt_L_nj35;
TH1D* h_MR_L_nj35;
TH1D* h_R2_L_nj35;
TH1D* h_tau21_L_nj35;
TH1D* h_tau32_L_nj35;
TH1D* h_MET_L_nj35;

TH1D* h_ht_AK4_L_nj6;
TH1D* h_jet1_pt_L_nj6;
TH1D* h_MR_L_nj6;
TH1D* h_R2_L_nj6;
TH1D* h_tau21_L_nj6;
TH1D* h_tau32_L_nj6;
TH1D* h_MET_L_nj6;

TH1D* h_ht_AK4_no1Lep_L;
TH1D* h_jet1_pt_no1Lep_L;
TH1D* h_MR_no1Lep_L;
TH1D* h_R2_no1Lep_L;
TH1D* h_MET_no1Lep_L;
TH1D* h_nLep_no1Lep_L;

TH1D* h_ht_AK4_no0b_L;
TH1D* h_jet1_pt_no0b_L;
TH1D* h_MR_no0b_L;
TH1D* h_R2_no0b_L;
TH1D* h_MET_no0b_L;
TH1D* h_nb_no0b_L;

TH1D* h_ht_AK4_nomDPhi_L;
TH1D* h_jet1_pt_nomDPhi_L;
TH1D* h_MR_nomDPhi_L;
TH1D* h_R2_nomDPhi_L;
TH1D* h_MET_nomDPhi_L;
TH1D* h_mDPhi_nomDPhi_L;

TH1D* h_ht_AK4_noMT_L;
TH1D* h_jet1_pt_noMT_L;
TH1D* h_MR_noMT_L;
TH1D* h_R2_noMT_L;
TH1D* h_MET_noMT_L;
TH1D* h_MT_noMT_L;

TH1D* h_ht_AK4_no1Lep_L_nj35;
TH1D* h_jet1_pt_no1Lep_L_nj35;
TH1D* h_MR_no1Lep_L_nj35;
TH1D* h_R2_no1Lep_L_nj35;
TH1D* h_MET_no1Lep_L_nj35;
TH1D* h_nLep_no1Lep_L_nj35;

TH1D* h_ht_AK4_no0b_L_nj35;
TH1D* h_jet1_pt_no0b_L_nj35;
TH1D* h_MR_no0b_L_nj35;
TH1D* h_R2_no0b_L_nj35;
TH1D* h_MET_no0b_L_nj35;
TH1D* h_nb_no0b_L_nj35;

TH1D* h_ht_AK4_nomDPhi_L_nj35;
TH1D* h_jet1_pt_nomDPhi_L_nj35;
TH1D* h_MR_nomDPhi_L_nj35;
TH1D* h_R2_nomDPhi_L_nj35;
TH1D* h_MET_nomDPhi_L_nj35;
TH1D* h_mDPhi_nomDPhi_L_nj35;

TH1D* h_ht_AK4_noMT_L_nj35;
TH1D* h_jet1_pt_noMT_L_nj35;
TH1D* h_MR_noMT_L_nj35;
TH1D* h_R2_noMT_L_nj35;
TH1D* h_MET_noMT_L_nj35;
TH1D* h_MT_noMT_L_nj35;

TH1D* h_ht_AK4_no1Lep_L_nj6;
TH1D* h_jet1_pt_no1Lep_L_nj6;
TH1D* h_MR_no1Lep_L_nj6;
TH1D* h_R2_no1Lep_L_nj6;
TH1D* h_MET_no1Lep_L_nj6;
TH1D* h_nLep_no1Lep_L_nj6;

TH1D* h_ht_AK4_no0b_L_nj6;
TH1D* h_jet1_pt_no0b_L_nj6;
TH1D* h_MR_no0b_L_nj6;
TH1D* h_R2_no0b_L_nj6;
TH1D* h_MET_no0b_L_nj6;
TH1D* h_nb_no0b_L_nj6;

TH1D* h_ht_AK4_nomDPhi_L_nj6;
TH1D* h_jet1_pt_nomDPhi_L_nj6;
TH1D* h_MR_nomDPhi_L_nj6;
TH1D* h_R2_nomDPhi_L_nj6;
TH1D* h_MET_nomDPhi_L_nj6;
TH1D* h_mDPhi_nomDPhi_L_nj6;

TH1D* h_ht_AK4_noMT_L_nj6;
TH1D* h_jet1_pt_noMT_L_nj6;
TH1D* h_MR_noMT_L_nj6;
TH1D* h_R2_noMT_L_nj6;
TH1D* h_MET_noMT_L_nj6;
TH1D* h_MT_noMT_L_nj6;

#if TOP == 0
TH1D* h_ht_AK4_nonW_L;
TH1D* h_jet1_pt_nonW_L;
TH1D* h_MR_nonW_L;
TH1D* h_R2_nonW_L;
TH1D* h_MET_nonW_L;
TH1D* h_nW_nonW_L;

TH1D* h_ht_AK4_nonW_L_nj35;
TH1D* h_jet1_pt_nonW_L_nj35;
TH1D* h_MR_nonW_L_nj35;
TH1D* h_R2_nonW_L_nj35;
TH1D* h_MET_nonW_L_nj35;
TH1D* h_nW_nonW_L_nj35;

TH1D* h_ht_AK4_nonW_L_nj6;
TH1D* h_jet1_pt_nonW_L_nj6;
TH1D* h_MR_nonW_L_nj6;
TH1D* h_R2_nonW_L_nj6;
TH1D* h_MET_nonW_L_nj6;
TH1D* h_nW_nonW_L_nj6;
#else
TH1D* h_ht_AK4_nonTop_L;
TH1D* h_jet1_pt_nonTop_L;
TH1D* h_MR_nonTop_L;
TH1D* h_R2_nonTop_L;
TH1D* h_MET_nonTop_L;
TH1D* h_nTop_nonTop_L;

TH1D* h_ht_AK4_nonTop_L_nj35;
TH1D* h_jet1_pt_nonTop_L_nj35;
TH1D* h_MR_nonTop_L_nj35;
TH1D* h_R2_nonTop_L_nj35;
TH1D* h_MET_nonTop_L_nj35;
TH1D* h_nTop_nonTop_L_nj35;

TH1D* h_ht_AK4_nonTop_L_nj6;
TH1D* h_jet1_pt_nonTop_L_nj6;
TH1D* h_MR_nonTop_L_nj6;
TH1D* h_R2_nonTop_L_nj6;
TH1D* h_MET_nonTop_L_nj6;
TH1D* h_nTop_nonTop_L_nj6;
#endif
TH1D* h_ht_AK4_Q;
TH1D* h_jet1_pt_Q;
TH1D* h_MR_Q;
TH1D* h_R2_Q;
TH1D* h_tau21_Q;
TH1D* h_tau32_Q;
TH1D* h_MET_Q;
TH1D* h_AK8_jet1_pt_Q;
TH1D* h_AK8_jet1_eta_Q;
TH1D* h_MET_phi_Q;
TH1D* h_AK8_jet1_pt_Q_nj35;
TH1D* h_AK8_jet1_eta_Q_nj35;
TH1D* h_MET_phi_Q_nj35;
TH1D* h_AK8_jet1_pt_Q_nj6;
TH1D* h_AK8_jet1_eta_Q_nj6;
TH1D* h_MET_phi_Q_nj6;

TH1D* h_ht_AK4_nomDPhi_Q;
TH1D* h_jet1_pt_nomDPhi_Q;
TH1D* h_MR_nomDPhi_Q;
TH1D* h_R2_nomDPhi_Q;
TH1D* h_MET_nomDPhi_Q;
TH1D* h_mDPhi_nomDPhi_Q;

TH1D* h_ht_AK4_no0Lep_Q;
TH1D* h_jet1_pt_no0Lep_Q;
TH1D* h_MR_no0Lep_Q;
TH1D* h_R2_no0Lep_Q;
TH1D* h_MET_no0Lep_Q;
TH1D* h_nLep_no0Lep_Q;

TH1D* h_ht_AK4_nonb_Q;
TH1D* h_jet1_pt_nonb_Q;
TH1D* h_MR_nonb_Q;
TH1D* h_R2_nonb_Q;
TH1D* h_MET_nonb_Q;
TH1D* h_nb_nonb_Q;


TH1D* h_ht_AK4_Q_nj35;
TH1D* h_jet1_pt_Q_nj35;
TH1D* h_MR_Q_nj35;
TH1D* h_R2_Q_nj35;
TH1D* h_tau21_Q_nj35;
TH1D* h_tau32_Q_nj35;
TH1D* h_MET_Q_nj35;

TH1D* h_ht_AK4_nomDPhi_Q_nj35;
TH1D* h_jet1_pt_nomDPhi_Q_nj35;
TH1D* h_MR_nomDPhi_Q_nj35;
TH1D* h_R2_nomDPhi_Q_nj35;
TH1D* h_MET_nomDPhi_Q_nj35;
TH1D* h_mDPhi_nomDPhi_Q_nj35;

TH1D* h_ht_AK4_no0Lep_Q_nj35;
TH1D* h_jet1_pt_no0Lep_Q_nj35;
TH1D* h_MR_no0Lep_Q_nj35;
TH1D* h_R2_no0Lep_Q_nj35;
TH1D* h_MET_no0Lep_Q_nj35;
TH1D* h_nLep_no0Lep_Q_nj35;

TH1D* h_ht_AK4_nonb_Q_nj35;
TH1D* h_jet1_pt_nonb_Q_nj35;
TH1D* h_MR_nonb_Q_nj35;
TH1D* h_R2_nonb_Q_nj35;
TH1D* h_MET_nonb_Q_nj35;
TH1D* h_nb_nonb_Q_nj35;

TH1D* h_ht_AK4_Q_nj6;
TH1D* h_jet1_pt_Q_nj6;
TH1D* h_MR_Q_nj6;
TH1D* h_R2_Q_nj6;
TH1D* h_tau21_Q_nj6;
TH1D* h_tau32_Q_nj6;
TH1D* h_MET_Q_nj6;

TH1D* h_ht_AK4_nomDPhi_Q_nj6;
TH1D* h_jet1_pt_nomDPhi_Q_nj6;
TH1D* h_MR_nomDPhi_Q_nj6;
TH1D* h_R2_nomDPhi_Q_nj6;
TH1D* h_MET_nomDPhi_Q_nj6;
TH1D* h_mDPhi_nomDPhi_Q_nj6;

TH1D* h_ht_AK4_no0Lep_Q_nj6;
TH1D* h_jet1_pt_no0Lep_Q_nj6;
TH1D* h_MR_no0Lep_Q_nj6;
TH1D* h_R2_no0Lep_Q_nj6;
TH1D* h_MET_no0Lep_Q_nj6;
TH1D* h_nLep_no0Lep_Q_nj6;

TH1D* h_ht_AK4_nonb_Q_nj6;
TH1D* h_jet1_pt_nonb_Q_nj6;
TH1D* h_MR_nonb_Q_nj6;
TH1D* h_R2_nonb_Q_nj6;
TH1D* h_MET_nonb_Q_nj6;
TH1D* h_nb_nonb_Q_nj6;

#if TOP == 0
TH1D* h_ht_AK4_nonW_Q;
TH1D* h_jet1_pt_nonW_Q;
TH1D* h_MR_nonW_Q;
TH1D* h_R2_nonW_Q;
TH1D* h_MET_nonW_Q;
TH1D* h_nW_nonW_Q;

TH1D* h_ht_AK4_nonW_Q_nj35;
TH1D* h_jet1_pt_nonW_Q_nj35;
TH1D* h_MR_nonW_Q_nj35;
TH1D* h_R2_nonW_Q_nj35;
TH1D* h_MET_nonW_Q_nj35;
TH1D* h_nW_nonW_Q_nj35;

TH1D* h_ht_AK4_nonW_Q_nj6;
TH1D* h_jet1_pt_nonW_Q_nj6;
TH1D* h_MR_nonW_Q_nj6;
TH1D* h_R2_nonW_Q_nj6;
TH1D* h_MET_nonW_Q_nj6;
TH1D* h_nW_nonW_Q_nj6;
#else
TH1D* h_ht_AK4_nonTop_Q;
TH1D* h_jet1_pt_nonTop_Q;
TH1D* h_MR_nonTop_Q;
TH1D* h_R2_nonTop_Q;
TH1D* h_MET_nonTop_Q;
TH1D* h_nTop_nonTop_Q;

TH1D* h_ht_AK4_nonTop_Q_nj35;
TH1D* h_jet1_pt_nonTop_Q_nj35;
TH1D* h_MR_nonTop_Q_nj35;
TH1D* h_R2_nonTop_Q_nj35;
TH1D* h_MET_nonTop_Q_nj35;
TH1D* h_nTop_nonTop_Q_nj35;

TH1D* h_ht_AK4_nonTop_Q_nj6;
TH1D* h_jet1_pt_nonTop_Q_nj6;
TH1D* h_MR_nonTop_Q_nj6;
TH1D* h_R2_nonTop_Q_nj6;
TH1D* h_MET_nonTop_Q_nj6;
TH1D* h_nTop_nonTop_Q_nj6;
#endif

TH1D* h_ht_AK4_T;
TH1D* h_jet1_pt_T;
TH1D* h_MR_T;
TH1D* h_R2_T;
TH1D* h_tau21_T;
TH1D* h_tau32_T;
TH1D* h_MET_T;
TH1D* h_AK8_jet1_pt_T;
TH1D* h_AK8_jet1_eta_T;
TH1D* h_MET_phi_T;
TH1D* h_AK8_jet1_pt_T_nj35;
TH1D* h_AK8_jet1_eta_T_nj35;
TH1D* h_MET_phi_T_nj35;
TH1D* h_AK8_jet1_pt_T_nj6;
TH1D* h_AK8_jet1_eta_T_nj6;
TH1D* h_MET_phi_T_nj6;

#if TOP == 0
TH1D* h_ht_AK4_no1b_T;
TH1D* h_jet1_pt_no1b_T;
TH1D* h_MR_no1b_T;
TH1D* h_R2_no1b_T;
TH1D* h_MET_no1b_T;
TH1D* h_nb_no1b_T;
#endif
TH1D* h_ht_AK4_noMT_T;
TH1D* h_jet1_pt_noMT_T;
TH1D* h_MR_noMT_T;
TH1D* h_R2_noMT_T;
TH1D* h_MET_noMT_T;
TH1D* h_MT_noMT_T;

TH1D* h_ht_AK4_nomDPhi_T;
TH1D* h_jet1_pt_nomDPhi_T;
TH1D* h_MR_nomDPhi_T;
TH1D* h_R2_nomDPhi_T;
TH1D* h_MET_nomDPhi_T;
TH1D* h_mDPhi_nomDPhi_T;

TH1D* h_ht_AK4_no1Lep_T;
TH1D* h_jet1_pt_no1Lep_T;
TH1D* h_MR_no1Lep_T;
TH1D* h_R2_no1Lep_T;
TH1D* h_MET_no1Lep_T;
TH1D* h_MT_no1Lep_T;
TH1D* h_nLep_no1Lep_T;

TH1D* h_ht_AK4_T_nj35;
TH1D* h_jet1_pt_T_nj35;
TH1D* h_MR_T_nj35;
TH1D* h_R2_T_nj35;
TH1D* h_tau21_T_nj35;
TH1D* h_tau32_T_nj35;
TH1D* h_MET_T_nj35;

#if TOP == 0
TH1D* h_ht_AK4_no1b_T_nj35;
TH1D* h_jet1_pt_no1b_T_nj35;
TH1D* h_MR_no1b_T_nj35;
TH1D* h_R2_no1b_T_nj35;
TH1D* h_MET_no1b_T_nj35;
TH1D* h_nb_no1b_T_nj35;
#endif
TH1D* h_ht_AK4_noMT_T_nj35;
TH1D* h_jet1_pt_noMT_T_nj35;
TH1D* h_MR_noMT_T_nj35;
TH1D* h_R2_noMT_T_nj35;
TH1D* h_MET_noMT_T_nj35;
TH1D* h_MT_noMT_T_nj35;

TH1D* h_ht_AK4_nomDPhi_T_nj35;
TH1D* h_jet1_pt_nomDPhi_T_nj35;
TH1D* h_MR_nomDPhi_T_nj35;
TH1D* h_R2_nomDPhi_T_nj35;
TH1D* h_MET_nomDPhi_T_nj35;
TH1D* h_mDPhi_nomDPhi_T_nj35;

TH1D* h_ht_AK4_no1Lep_T_nj35;
TH1D* h_jet1_pt_no1Lep_T_nj35;
TH1D* h_MR_no1Lep_T_nj35;
TH1D* h_R2_no1Lep_T_nj35;
TH1D* h_MET_no1Lep_T_nj35;
TH1D* h_MT_no1Lep_T_nj35;
TH1D* h_nLep_no1Lep_T_nj35;

TH1D* h_ht_AK4_T_nj6;
TH1D* h_jet1_pt_T_nj6;
TH1D* h_MR_T_nj6;
TH1D* h_R2_T_nj6;
TH1D* h_tau21_T_nj6;
TH1D* h_tau32_T_nj6;
TH1D* h_MET_T_nj6;

#if TOP == 0
TH1D* h_ht_AK4_nonW_T;
TH1D* h_jet1_pt_nonW_T;
TH1D* h_MR_nonW_T;
TH1D* h_R2_nonW_T;
TH1D* h_MET_nonW_T;
TH1D* h_nW_nonW_T;

TH1D* h_ht_AK4_nonW_T_nj35;
TH1D* h_jet1_pt_nonW_T_nj35;
TH1D* h_MR_nonW_T_nj35;
TH1D* h_R2_nonW_T_nj35;
TH1D* h_MET_nonW_T_nj35;
TH1D* h_nW_nonW_T_nj35;

TH1D* h_ht_AK4_nonW_T_nj6;
TH1D* h_jet1_pt_nonW_T_nj6;
TH1D* h_MR_nonW_T_nj6;
TH1D* h_R2_nonW_T_nj6;
TH1D* h_MET_nonW_T_nj6;
TH1D* h_nW_nonW_T_nj6;
#else
TH1D* h_ht_AK4_nonTop_T;
TH1D* h_jet1_pt_nonTop_T;
TH1D* h_MR_nonTop_T;
TH1D* h_R2_nonTop_T;
TH1D* h_MET_nonTop_T;
TH1D* h_nTop_nonTop_T;

TH1D* h_ht_AK4_nonTop_T_nj35;
TH1D* h_jet1_pt_nonTop_T_nj35;
TH1D* h_MR_nonTop_T_nj35;
TH1D* h_R2_nonTop_T_nj35;
TH1D* h_MET_nonTop_T_nj35;
TH1D* h_nTop_nonTop_T_nj35;

TH1D* h_ht_AK4_nonTop_T_nj6;
TH1D* h_jet1_pt_nonTop_T_nj6;
TH1D* h_MR_nonTop_T_nj6;
TH1D* h_R2_nonTop_T_nj6;
TH1D* h_MET_nonTop_T_nj6;
TH1D* h_nTop_nonTop_T_nj6;
#endif

#if TOP == 0
TH1D* h_ht_AK4_no1b_T_nj6;
TH1D* h_jet1_pt_no1b_T_nj6;
TH1D* h_MR_no1b_T_nj6;
TH1D* h_R2_no1b_T_nj6;
TH1D* h_MET_no1b_T_nj6;
TH1D* h_nb_no1b_T_nj6;
#endif
TH1D* h_ht_AK4_noMT_T_nj6;
TH1D* h_jet1_pt_noMT_T_nj6;
TH1D* h_MR_noMT_T_nj6;
TH1D* h_R2_noMT_T_nj6;
TH1D* h_MET_noMT_T_nj6;
TH1D* h_MT_noMT_T_nj6;

TH1D* h_ht_AK4_nomDPhi_T_nj6;
TH1D* h_jet1_pt_nomDPhi_T_nj6;
TH1D* h_MR_nomDPhi_T_nj6;
TH1D* h_R2_nomDPhi_T_nj6;
TH1D* h_MET_nomDPhi_T_nj6;
TH1D* h_mDPhi_nomDPhi_T_nj6;

TH1D* h_ht_AK4_no1Lep_T_nj6;
TH1D* h_jet1_pt_no1Lep_T_nj6;
TH1D* h_MR_no1Lep_T_nj6;
TH1D* h_R2_no1Lep_T_nj6;
TH1D* h_MET_no1Lep_T_nj6;
TH1D* h_MT_no1Lep_T_nj6;
TH1D* h_nLep_no1Lep_T_nj6;

TH1D* h_ht_AK4_W;
TH1D* h_jet1_pt_W;
TH1D* h_MR_W;
TH1D* h_R2_W;
TH1D* h_tau21_W;
TH1D* h_tau32_W;
TH1D* h_MET_W;
TH1D* h_AK8_jet1_pt_W;
TH1D* h_AK8_jet1_eta_W;
TH1D* h_MET_phi_W;
TH1D* h_AK8_jet1_pt_W_nj35;
TH1D* h_AK8_jet1_eta_W_nj35;
TH1D* h_MET_phi_W_nj35;
TH1D* h_AK8_jet1_pt_W_nj6;
TH1D* h_AK8_jet1_eta_W_nj6;
TH1D* h_MET_phi_W_nj6;

TH1D* h_ht_AK4_noMT_W;
TH1D* h_jet1_pt_noMT_W;
TH1D* h_MR_noMT_W;
TH1D* h_R2_noMT_W;
TH1D* h_MET_noMT_W;
TH1D* h_MT_noMT_W;

TH1D* h_ht_AK4_nonb_W;
TH1D* h_jet1_pt_nonb_W;
TH1D* h_MR_nonb_W;
TH1D* h_R2_nonb_W;
TH1D* h_MET_nonb_W;
TH1D* h_nb_nonb_W;

TH1D* h_ht_AK4_nomDPhi_W;
TH1D* h_jet1_pt_nomDPhi_W;
TH1D* h_MR_nomDPhi_W;
TH1D* h_R2_nomDPhi_W;
TH1D* h_MET_nomDPhi_W;
TH1D* h_mDPhi_nomDPhi_W;

TH1D* h_ht_AK4_no1Lep_W;
TH1D* h_jet1_pt_no1Lep_W;
TH1D* h_MR_no1Lep_W;
TH1D* h_R2_no1Lep_W;
TH1D* h_MET_no1Lep_W;
TH1D* h_nLep_no1Lep_W;

#if TOP == 0
TH1D* h_ht_AK4_nonW_W;
TH1D* h_jet1_pt_nonW_W;
TH1D* h_MR_nonW_W;
TH1D* h_R2_nonW_W;
TH1D* h_MET_nonW_W;
TH1D* h_nW_nonW_W;

TH1D* h_ht_AK4_nonW_W_nj35;
TH1D* h_jet1_pt_nonW_W_nj35;
TH1D* h_MR_nonW_W_nj35;
TH1D* h_R2_nonW_W_nj35;
TH1D* h_MET_nonW_W_nj35;
TH1D* h_nW_nonW_W_nj35;

TH1D* h_ht_AK4_nonW_W_nj6;
TH1D* h_jet1_pt_nonW_W_nj6;
TH1D* h_MR_nonW_W_nj6;
TH1D* h_R2_nonW_W_nj6;
TH1D* h_MET_nonW_W_nj6;
TH1D* h_nW_nonW_W_nj6;
#else
TH1D* h_ht_AK4_nonTop_W;
TH1D* h_jet1_pt_nonTop_W;
TH1D* h_MR_nonTop_W;
TH1D* h_R2_nonTop_W;
TH1D* h_MET_nonTop_W;
TH1D* h_nTop_nonTop_W;

TH1D* h_ht_AK4_nonTop_W_nj35;
TH1D* h_jet1_pt_nonTop_W_nj35;
TH1D* h_MR_nonTop_W_nj35;
TH1D* h_R2_nonTop_W_nj35;
TH1D* h_MET_nonTop_W_nj35;
TH1D* h_nTop_nonTop_W_nj35;

TH1D* h_ht_AK4_nonTop_W_nj6;
TH1D* h_jet1_pt_nonTop_W_nj6;
TH1D* h_MR_nonTop_W_nj6;
TH1D* h_R2_nonTop_W_nj6;
TH1D* h_MET_nonTop_W_nj6;
TH1D* h_nTop_nonTop_W_nj6;
#endif

TH1D* h_ht_AK4_W_nj35;
TH1D* h_jet1_pt_W_nj35;
TH1D* h_MR_W_nj35;
TH1D* h_R2_W_nj35;
TH1D* h_tau21_W_nj35;
TH1D* h_tau32_W_nj35;
TH1D* h_MET_W_nj35;

TH1D* h_ht_AK4_noMT_W_nj35;
TH1D* h_jet1_pt_noMT_W_nj35;
TH1D* h_MR_noMT_W_nj35;
TH1D* h_R2_noMT_W_nj35;
TH1D* h_MET_noMT_W_nj35;
TH1D* h_MT_noMT_W_nj35;

TH1D* h_ht_AK4_nonb_W_nj35;
TH1D* h_jet1_pt_nonb_W_nj35;
TH1D* h_MR_nonb_W_nj35;
TH1D* h_R2_nonb_W_nj35;
TH1D* h_MET_nonb_W_nj35;
TH1D* h_nb_nonb_W_nj35;

TH1D* h_ht_AK4_nomDPhi_W_nj35;
TH1D* h_jet1_pt_nomDPhi_W_nj35;
TH1D* h_MR_nomDPhi_W_nj35;
TH1D* h_R2_nomDPhi_W_nj35;
TH1D* h_MET_nomDPhi_W_nj35;
TH1D* h_mDPhi_nomDPhi_W_nj35;

TH1D* h_ht_AK4_no1Lep_W_nj35;
TH1D* h_jet1_pt_no1Lep_W_nj35;
TH1D* h_MR_no1Lep_W_nj35;
TH1D* h_R2_no1Lep_W_nj35;
TH1D* h_MET_no1Lep_W_nj35;
TH1D* h_nLep_no1Lep_W_nj35;

TH1D* h_ht_AK4_W_nj6;
TH1D* h_jet1_pt_W_nj6;
TH1D* h_MR_W_nj6;
TH1D* h_R2_W_nj6;
TH1D* h_tau21_W_nj6;
TH1D* h_tau32_W_nj6;
TH1D* h_MET_W_nj6;

TH1D* h_ht_AK4_noMT_W_nj6;
TH1D* h_jet1_pt_noMT_W_nj6;
TH1D* h_MR_noMT_W_nj6;
TH1D* h_R2_noMT_W_nj6;
TH1D* h_MET_noMT_W_nj6;
TH1D* h_MT_noMT_W_nj6;

TH1D* h_ht_AK4_nonb_W_nj6;
TH1D* h_jet1_pt_nonb_W_nj6;
TH1D* h_MR_nonb_W_nj6;
TH1D* h_R2_nonb_W_nj6;
TH1D* h_MET_nonb_W_nj6;
TH1D* h_nb_nonb_W_nj6;

TH1D* h_ht_AK4_nomDPhi_W_nj6;
TH1D* h_jet1_pt_nomDPhi_W_nj6;
TH1D* h_MR_nomDPhi_W_nj6;
TH1D* h_R2_nomDPhi_W_nj6;
TH1D* h_MET_nomDPhi_W_nj6;
TH1D* h_mDPhi_nomDPhi_W_nj6;

TH1D* h_ht_AK4_no1Lep_W_nj6;
TH1D* h_jet1_pt_no1Lep_W_nj6;
TH1D* h_MR_no1Lep_W_nj6;
TH1D* h_R2_no1Lep_W_nj6;
TH1D* h_MET_no1Lep_W_nj6;
TH1D* h_nLep_no1Lep_W_nj6;

TH1D* h_ht_AK4_q;
TH1D* h_jet1_pt_q;
TH1D* h_MR_q;
TH1D* h_R2_q;
TH1D* h_tau21_q;
TH1D* h_tau32_q;
TH1D* h_MET_q;
TH1D* h_AK8_jet1_pt_q;
TH1D* h_AK8_jet1_eta_q;
TH1D* h_MET_phi_q;
TH1D* h_AK8_jet1_pt_q_nj35;
TH1D* h_AK8_jet1_eta_q_nj35;
TH1D* h_MET_phi_q_nj35;
TH1D* h_AK8_jet1_pt_q_nj6;
TH1D* h_AK8_jet1_eta_q_nj6;
TH1D* h_MET_phi_q_nj6;

TH1D* h_ht_AK4_q_nj35;
TH1D* h_jet1_pt_q_nj35;
TH1D* h_MR_q_nj35;
TH1D* h_R2_q_nj35;
TH1D* h_tau21_q_nj35;
TH1D* h_tau32_q_nj35;
TH1D* h_MET_q_nj35;

TH1D* h_ht_AK4_q_nj6;
TH1D* h_jet1_pt_q_nj6;
TH1D* h_MR_q_nj6;
TH1D* h_R2_q_nj6;
TH1D* h_tau21_q_nj6;
TH1D* h_tau32_q_nj6;
TH1D* h_MET_q_nj6;

TH1D* h_softDropMass;

TH2D* h_R2_MR;

TH2D* h_R2_MR_s;
TH2D* h_MR_MET_s;
TH2D* h_R2_MET_s;
TH2D* h_AK8Jet1pT_MET_s;
TH2D* h_R2_MR_q;
TH2D* h_MR_MET_q;
TH2D* h_R2_MET_q;
TH2D* h_AK8Jet1pT_MET_q;
TH2D* h_R2_MR_S;
TH2D* h_MR_MET_S;
TH2D* h_R2_MET_S;
TH2D* h_HT_MET_S;
TH2D* h_MR_HT_S;
TH2D* h_R2_HT_S;
TH2D* h_AK8Jet1pT_MET_S;
TH2D* h_MR_MET_S_nj35;
TH2D* h_R2_MET_S_nj35;
TH2D* h_HT_MET_S_nj35;
TH2D* h_MR_HT_S_nj35;
TH2D* h_R2_HT_S_nj35;
TH2D* h_AK8Jet1pT_MET_S_nj35;
TH2D* h_MR_MET_S_nj6;
TH2D* h_R2_MET_S_nj6;
TH2D* h_HT_MET_S_nj6;
TH2D* h_MR_HT_S_nj6;
TH2D* h_R2_HT_S_nj6;
TH2D* h_AK8Jet1pT_MET_S_nj6;
TH2D* h_R2_MR_Q;
TH2D* h_MR_MET_Q;
TH2D* h_R2_MET_Q;
TH2D* h_AK8Jet1pT_MET_Q;
TH2D* h_R2_MR_T;
TH2D* h_MR_MET_T;
TH2D* h_R2_MET_T;
TH2D* h_AK8Jet1pT_MET_T;
TH2D* h_R2_MR_W;
TH2D* h_MR_MET_W;
TH2D* h_R2_MET_W;
TH2D* h_AK8Jet1pT_MET_W;
TH2D* h_R2_MR_L;
TH2D* h_MR_MET_L;
TH2D* h_R2_MET_L;
TH2D* h_AK8Jet1pT_MET_L;
TH2D* h_R2_MR_Z;
TH2D* h_MR_MET_Z;
TH2D* h_R2_MET_Z;
TH2D* h_AK8Jet1pT_MET_Z;
TH2D* h_R2_MR_G;
TH2D* h_MR_MET_G;
TH2D* h_R2_MET_G;
TH2D* h_AK8Jet1pT_MET_G;

TH2D* h_R2_MR_s_nj35;
TH2D* h_R2_MR_s_nj6;
TH2D* h_R2_MR_q_nj35;
TH2D* h_R2_MR_q_nj6;
TH2D* h_R2_MR_S_nj35;
TH2D* h_R2_MR_S_nj6;
TH2D* h_R2_MR_Q_nj35;
TH2D* h_R2_MR_Q_nj6;
TH2D* h_R2_MR_T_nj35;
TH2D* h_R2_MR_T_nj6;
TH2D* h_R2_MR_W_nj35;
TH2D* h_R2_MR_W_nj6;
TH2D* h_R2_MR_Z_nj35;
TH2D* h_R2_MR_Z_nj6;
TH2D* h_R2_MR_L_nj35;
TH2D* h_R2_MR_L_nj6;
TH2D* h_R2_MR_G_nj35;
TH2D* h_R2_MR_G_nj6;

TH1D *h_HT_TrigNoMass_0;
TH1D *h_HT_TrigNoMass_1;

TH1D *h_AK8HT_TrigNoMass_0;
TH1D *h_AK8HT_TrigNoMass_1;

TH1D *h_AK8JetpT_TrigNoMass_0;
TH1D *h_AK8JetpT_TrigNoMass_1;

TH1D *h_AK8JetMass_TrigNoMass_0;
TH1D *h_AK8JetMass_TrigNoMass_1;

TH1D *h_MET_TrigNoMass_0;
TH1D *h_MET_TrigNoMass_1;

TH1D *h_MR_TrigNoMass_0;
TH1D *h_MR_TrigNoMass_1;

TH1D *h_R2_TrigNoMass_0;
TH1D *h_R2_TrigNoMass_1;

TH2D *h_R2_MR_TrigNoMass_0;
TH2D *h_R2_MR_TrigNoMass_1;

TH2D *h_MET_AK8JetMass_TrigNoMass_0;
TH2D *h_MET_AK8JetMass_TrigNoMass_1;

TH2D *h_HT_MET_TrigNoMass_0;
TH2D *h_HT_MET_TrigNoMass_1;

TH2D *h_HT_AK8JetMass_TrigNoMass_0;
TH2D *h_HT_AK8JetMass_TrigNoMass_1;

TH2D *h_AK8HT_MET_TrigNoMass_0;
TH2D *h_AK8HT_MET_TrigNoMass_1;

TH2D *h_AK8HT_AK8JetMass_TrigNoMass_0;
TH2D *h_AK8HT_AK8JetMass_TrigNoMass_1;

TH3D *h_HT_MET_AK8JetMass_TrigNoMass_0;
TH3D *h_HT_MET_AK8JetMass_TrigNoMass_1;

TH3D *h_AK8HT_MET_AK8JetMass_TrigNoMass_0;
TH3D *h_AK8HT_MET_AK8JetMass_TrigNoMass_1;

TH1D *h_HT_TrigMass_0;
TH1D *h_HT_TrigMass_1;

TH1D *h_AK8HT_TrigMass_0;
TH1D *h_AK8HT_TrigMass_1;

TH1D *h_AK8JetpT_TrigMass_0;
TH1D *h_AK8JetpT_TrigMass_1;

TH1D *h_AK8JetMass_TrigMass_0;
TH1D *h_AK8JetMass_TrigMass_1;

TH1D *h_MET_TrigMass_0;
TH1D *h_MET_TrigMass_1;

TH1D *h_MR_TrigMass_0;
TH1D *h_MR_TrigMass_1;

TH1D *h_R2_TrigMass_0;
TH1D *h_R2_TrigMass_1;

TH2D *h_R2_MR_TrigMass_0;
TH2D *h_R2_MR_TrigMass_1;

TH2D *h_MET_AK8JetMass_TrigMass_0;
TH2D *h_MET_AK8JetMass_TrigMass_1;

TH2D *h_HT_MET_TrigMass_0;
TH2D *h_HT_MET_TrigMass_1;

TH2D *h_HT_AK8JetMass_TrigMass_0;
TH2D *h_HT_AK8JetMass_TrigMass_1;

TH2D *h_AK8HT_MET_TrigMass_0;
TH2D *h_AK8HT_MET_TrigMass_1;

TH2D *h_AK8HT_AK8JetMass_TrigMass_0;
TH2D *h_AK8HT_AK8JetMass_TrigMass_1;

TH3D *h_HT_MET_AK8JetMass_TrigMass_0;
TH3D *h_HT_MET_AK8JetMass_TrigMass_1;

TH3D *h_AK8HT_MET_AK8JetMass_TrigMass_0;
TH3D *h_AK8HT_MET_AK8JetMass_TrigMass_1;

TH2D* h_AK8Jet1Pt_Eta_W_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_W_fakerate;
TH2D* h_AK8Jet1Pt_Eta_mW_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_mW_fakerate;
TH2D* h_AK8Jet1Pt_Eta_m0bW_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_m0bW_fakerate;
TH2D* h_AK8Jet1Pt_Eta_aW_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_aW_fakerate;

TH2D* h_AK8Jet1Pt_Eta_Top_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_Top_fakerate;
TH2D* h_AK8Jet1Pt_Eta_mTop_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_mTop_fakerate;
TH2D* h_AK8Jet1Pt_Eta_m0bTop_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_m0bTop_fakerate;
TH2D* h_AK8Jet1Pt_Eta_aTop_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_aTop_fakerate;

TH1D* h_AK8Jet1Pt_Z_fakerate;
TH1D* h_AK8Jet1Pt_no_Z_fakerate;
TH2D* h_AK8Jet1Pt_Eta_Z_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_Z_fakerate;
TH1D* h_AK8Jet1Pt_WtagZ_fakerate;
TH1D* h_AK8Jet1Pt_no_WtagZ_fakerate;
TH2D* h_AK8Jet1Pt_Eta_WtagZ_fakerate;
TH2D* h_AK8Jet1Pt_Eta_no_WtagZ_fakerate;

TH2D* h_AK8Jet1Pt_eta_GenW_no_W;
TH2D* h_AK8Jet1Pt_eta_GenTop_no_Top;
TH2D* h_AK8Jet1Pt_eta_GenW_W;
TH2D* h_AK8Jet1Pt_eta_GenTop_Top;
TH2D* h_AK8Jet1Pt_eta_FakeW_W;
TH2D* h_AK8Jet1Pt_eta_FakeTop_Top;

std::vector<TH2D*> vh_R2_MR_s_nj35;
std::vector<TH2D*> vh_R2_MR_s_nj6;
std::vector<TH2D*> vh_R2_MR_q_nj35;
std::vector<TH2D*> vh_R2_MR_q_nj6;
std::vector<TH2D*> vh_R2_MR_S_nj35;
std::vector<TH2D*> vh_R2_MR_S_nj6;
std::vector<TH2D*> vh_R2_MR_Q_nj35;
std::vector<TH2D*> vh_R2_MR_Q_nj6;
std::vector<TH2D*> vh_R2_MR_T_nj35;
std::vector<TH2D*> vh_R2_MR_T_nj6;
std::vector<TH2D*> vh_R2_MR_W_nj35;
std::vector<TH2D*> vh_R2_MR_W_nj6;
std::vector<TH2D*> vh_R2_MR_Z_nj35;
std::vector<TH2D*> vh_R2_MR_Z_nj6;
std::vector<TH2D*> vh_R2_MR_L_nj35;
std::vector<TH2D*> vh_R2_MR_L_nj6;
std::vector<TH2D*> vh_R2_MR_G_nj35;
std::vector<TH2D*> vh_R2_MR_G_nj6;


//std::vector<TH1D*> vh_jet1_pt;

//_______________________________________________________
//              Define Histograms here
void
Analysis::init_analysis_histos(const unsigned int& syst_nSyst, const unsigned int& syst_index)
{

  //double htbn = 20;
  //double htmn = 0;
 // double htmx = 1500;

  //double j1ptbn = 20;
  //double j1ptmn = 0;
  //double j1ptmx = 1000;

  //double MRbn = 7;
  //double MRmn = 0;
  //double MRmx = 4000;
  //double Rbn = 7;
  //double Rmn = 0;
  //double Rmx = 1;

  // Variable binning
  //for test
  int nbn_HT = 20;
  int nbn_MET = 16;

  //int nbnHT = 11;
  int nbnHT = 7;
  int nbnMET = 6;
  int nbnj1pt = 7;
  int nbn3DHT = 7;
  int nbn3DMET = 6;
  int nbnjmass = 5;
  //int nbn_j1pt = 12;
  //int nbn_AK8j1pt = 13;
  int nbn_AK8J1pt = 7;
  int nbn_MR = 7;
  int nbn_R2 = 7;
  int nbn_eta = 2;
  //Double_t  HT_bins[20]  = {0, 200, 300, 400, 500, 600, 650, 700, 750, 800, 900, 1000, 1200, 1500, 2000, 4000, 10000};
  //Double_t bn_HTtmp[] = {400.,500.,600.,700.,750.,800.,850.,900.,950.,1000.,1500.,100000.};
  Double_t bn_HTtmp[] = {200.,400.,500.,600.,800.,1000.,1500.,10000.};
  Double_t* bnHT = 0;
  bnHT = getVariableBinEdges(nbnHT+1,bn_HTtmp);

  Double_t bn_METtmp[] = {0.,100.,200.,400.,600.,1000.,10000.};
  Double_t* bnMET = 0;
  bnMET = getVariableBinEdges(nbnMET+1,bn_METtmp);

  Double_t bn_3DHTtmp[] = {200.,400.,500.,550,600.,800.,1500.,10000.};
  Double_t* bn3DHT = 0;
  bn3DHT = getVariableBinEdges(nbn3DHT+1,bn_3DHTtmp);

  Double_t bn_3DMETtmp[] = {0.,70.,110.,150.,200.,400.,10000.};
  Double_t* bn3DMET = 0;
  bn3DMET = getVariableBinEdges(nbn3DMET+1,bn_3DMETtmp);

  Double_t bn_jmasstmp[] = {0.,65.,105.,150.,210.,500.};
  Double_t* bnjmass = 0;
  bnjmass = getVariableBinEdges(nbnjmass+1,bn_jmasstmp);

  Double_t bn_j1pttmp[] = {200.,300.,400.,450.,500.,550.,600.,1000.};
  Double_t* bnj1pt = 0;
  bnj1pt = getVariableBinEdges(nbnj1pt+1,bn_j1pttmp);

  Double_t bn_MET_tmp[] = {0.,50.,100.,150.,200.,250.,300.,350.,400.,450.,500.,600.,700.,800.,1000.,2000.,10000.};
  Double_t* bn_MET = 0;
  bn_MET = getVariableBinEdges(nbn_MET+1,bn_MET_tmp);

  Double_t bn_HT_tmp[] = {0.,200.,400.,450.,500.,550.,600.,650.,700.,750.,800.,900.,1000.,1200.,1400.,1600.,1800.,2000.,2500.,3000.,10000.};
  Double_t* bn_HT = 0;
  bn_HT = getVariableBinEdges(nbn_HT+1,bn_HT_tmp);

  //Double_t bn_j1pt_tmp[] = {0.,50.,100.,150.,200.,250.,300.,350.,400.,450.,500.,700.,1000.};
  //Double_t* bn_j1pt = 0;
 // bn_j1pt = getVariableBinEdges(nbn_j1pt+1,bn_j1pt_tmp);
  //Double_t bn_AK8j1pt_tmp[] = {0.,200.,220.,240,260,280,300.,320.,340,360,380,400.,500.,1000.};
  //Double_t* bn_AK8j1pt = 0;
  //bn_AK8j1pt = getVariableBinEdges(nbn_AK8j1pt+1,bn_AK8j1pt_tmp);
  //Double_t bn_AK8J1pt_tmp[] = {200.,250,350.,1000.};
  Double_t bn_AK8J1pt_tmp[] = {0.,200.,300.,400.,500.,600.,800.,2000.};
  //Double_t bn_AK8J1pt_tmp[] = {0.,200.,220.,240,260,280,300.,350,400.,450,500.,700,1000.,1500.,2000.};
  //Double_t bn_AK8J1pt_tmp[] = {0.,200.,220.,240,260,280,300.,320.,340,360,380.,400.,450,500.,700,1000.,1500.,2000.};
  Double_t* bn_AK8J1pt = 0;
  bn_AK8J1pt = getVariableBinEdges(nbn_AK8J1pt+1,bn_AK8J1pt_tmp);
  Double_t bn_MR_tmp[] = {0.,600.,800.,1000.,1200.,1600.,2000.,4000.};
  Double_t* bn_MR = 0;
  bn_MR = getVariableBinEdges(nbn_MR+1,bn_MR_tmp);
  Double_t bn_R2_tmp[] = {0.,0.04,0.08,0.12,0.16,0.24,0.4,1.5};
  Double_t* bn_R2 = 0;
  bn_R2 = getVariableBinEdges(nbn_R2+1,bn_R2_tmp);
  Double_t bn_eta_tmp[] = {0.,1.5,2.5};
  Double_t* bn_eta = 0;
  bn_eta = getVariableBinEdges(nbn_eta+1,bn_eta_tmp);
/*
  std::vector<double> MR  = {0, 600, 800, 1000, 1200, 1600, 2000, 4000, 10000};
  std::vector<double> R2  = {0, 0.04, 0.08, 0.12, 0.16, 0.24, 0.4, 2.0, 5.0};
  // Unrolled bin labels
  std::map<int, std::string> Razor_labels; int bin = 1; size_t n, m;
  for (size_t i=2, n=MR.size(); i<n-2; ++i) for (size_t j=2, m=R2.size(); j<m-2; ++j) {
    std::stringstream ss;
    ss.precision(2);
    if (bin<=18) {
      ss<<fixed<<"["<<R2[j]<<", "<<R2[j+1]<<"]";
      Razor_labels[bin] = ss.str();                                                                                                                                           
    } else if (bin==19) {
      ss<<fixed<<"["<<R2[j]<<", "<<R2[j+2]<<"]";
      Razor_labels[bin] = ss.str();
    } else if (bin>=21&&bin<=22) {
      ss<<fixed<<"["<<R2[j]<<", "<<R2[j+1]<<"]";
      Razor_labels[bin-1] = ss.str();
    } else if (bin==23) {
      ss<<fixed<<"["<<R2[j]<<", "<<R2[j+3]<<"]";
      Razor_labels[bin-1] = ss.str();
    }    
    bin++;
  }

  size_t bin = -1;
  for (size_t i=2, n=MR.size(); i<n-2; ++i) if (MR>=MR[i]&&MR<MR[i+1])
    for (size_t j=2, m=R2.size(); j<m-2; ++j) if (R2>=R2[j]&&R2<R2[j+1])
      bin = (i-2)*(m-4)+j-2;
      if (bin>=19&&bin<=22) bin--;
      if (bin>=23&&bin<=24) bin=21;
*/

  h_dR_min = new TH1D("dR_min",	";dR_{min}(l, jet)", 40, 0, 2);
  h_rel_pT = new TH1D("rel_pT", ";p_{T, rel}(l, jet)", 40, 0, 100);
  h_rel_pT_dR_min = new TH2D("rel_pT_dR_min", ";dR_{min}(l, jet);p_{T, rel}(l, jet)", 40,0,2,40,0,100);
  h_dR_min_tag = new TH1D("dR_min_tag",	";dR_{min}(l, jet)", 40, 0, 2);
  h_rel_pT_tag = new TH1D("rel_pT_tag", ";p_{T, rel}(l, jet)", 40, 0, 100);
  h_rel_pT_dR_min_tag = new TH2D("rel_pT_dR_min_tag", ";dR_{min}(l, jet);p_{T, rel}(l, jet)", 40,0,2,40,0,100);
  h_dR_min_untag = new TH1D("dR_min_untag",	";dR_{min}(l, jet)", 40, 0, 2);
  h_rel_pT_untag = new TH1D("rel_pT_untag", ";p_{T, rel}(l, jet)", 40, 0, 100);
  h_rel_pT_dR_min_untag = new TH2D("rel_pT_dR_min_untag", ";dR_{min}(l, jet);p_{T, rel}(l, jet)", 40,0,2,40,0,100);

  h_MR_S_1Lep = new TH1D("MR_S_1Lep",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_S_1Lep = new TH1D("R2_S_1Lep",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_R2_MR_S_1Lep = new TH2D("R2_MR_S_1Lep", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_MR_T_1Lep = new TH1D("MR_T_1Lep",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_T_1Lep = new TH1D("R2_T_1Lep",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_R2_MR_T_1Lep = new TH2D("R2_MR_T_1Lep", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_MR_W_1Lep = new TH1D("MR_W_1Lep",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_W_1Lep = new TH1D("R2_W_1Lep",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_R2_MR_W_1Lep = new TH2D("R2_MR_W_1Lep", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);

  h_njet         = new TH1D("njet",         ";N_{jet}",                20, 0,  20);
  h_nAK8jet      = new TH1D("nAK8jet",      ";N_{jet}",                20, 0,  20);
  h_nbTight      = new TH1D("nbTight",     ";N_{jet}",                20, 0,  20);
  h_nbMedium     = new TH1D("nbMedium",     ";N_{jet}",                20, 0,  20);
  h_nbLoose      = new TH1D("nbLoose",     ";N_{jet}",                20, 0,  20);
  h_nEleVeto     = new TH1D("nEleVeto",     ";N_{jet}",                20, 0,  20);
  h_nEleTight    = new TH1D("nEleTight",     ";N_{jet}",                20, 0,  20);
  h_nEleMedium   = new TH1D("nEleMedium",     ";N_{jet}",                20, 0,  20);
  h_nEleLoose    = new TH1D("nEleLoose",     ";N_{jet}",                20, 0,  20);
  h_nMuTight     = new TH1D("nMuTight",     ";N_{jet}",                20, 0,  20);
  h_nMuMedium    = new TH1D("nMuMedium",     ";N_{jet}",                20, 0,  20);
  h_nMuSoft      = new TH1D("nMuSoft",     ";N_{jet}",                20, 0,  20);
  h_nTauVeto     = new TH1D("nTauVeto",     ";N_{jet}",                20, 0,  20);
  h_nIsoTrack    = new TH1D("nIsoTrack",     ";N_{jet}",                20, 0,  20);
  h_nPhoMedium   = new TH1D("nPhoMedium",     ";N_{jet}",                20, 0,  20);
  h_nw           = new TH1D("nw",           ";N_{W tag}",              20, 0,  20);
  h_ht_AK4  = new TH1D("ht_AK4",  ";H_{T}",                  nbn_HT, bn_HT);

  h_ele_pt_T = new TH1D("ele_pt_T", ";p_{T, ele}", 20, 0,400);
  h_ele_pt_W = new TH1D("ele_pt_W", ";p_{T, ele}", 20, 0,400);
  h_ele_pt_L = new TH1D("ele_pt_L", ";p_{T, ele}", 20, 0,400);
  h_mu_pt_T = new TH1D("mu_pt_T", ";p_{T, mu}", 20, 0,400);
  h_mu_pt_W = new TH1D("mu_pt_W", ";p_{T, mu}", 20, 0,400);
  h_mu_pt_L = new TH1D("mu_pt_L", ";p_{T, mu}", 20, 0,400);

  h_ht_AK4_S = new TH1D("ht_AK4_S",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_S = new TH1D("jet1_pt_S",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_S = new TH1D("MR_S",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_S = new TH1D("R2_S",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_S = new TH1D("tau21_S", ";tau21", 200,0,1);
  h_tau32_S = new TH1D("tau32_S", ";tau32", 200,0,1);
  h_MET_S = new TH1D("MET_S", ";MET", 400,0,2000);
  h_AK8_jet1_pt_S = new TH1D("AK8_jet1_pt_S",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_S = new TH1D("AK8_jet1_eta_S",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_S = new TH1D("MET_phi_S", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_S_nj35 = new TH1D("AK8_jet1_pt_S_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_S_nj35 = new TH1D("AK8_jet1_eta_S_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_S_nj35 = new TH1D("MET_phi_S_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_S_nj6 = new TH1D("AK8_jet1_pt_S_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_S_nj6 = new TH1D("AK8_jet1_eta_S_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_S_nj6 = new TH1D("MET_phi_S_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_S_nj35 = new TH1D("ht_AK4_S_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_S_nj35 = new TH1D("jet1_pt_S_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_S_nj35 = new TH1D("MR_S_nj35",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_S_nj35 = new TH1D("R2_S_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_S_nj35 = new TH1D("tau21_S_nj35", ";tau21", 200,0,1);
  h_tau32_S_nj35 = new TH1D("tau32_S_nj35", ";tau32", 200,0,1);
  h_MET_S_nj35 = new TH1D("MET_S_nj35", ";MET", 400,0,2000);

  h_ht_AK4_S_nj6 = new TH1D("ht_AK4_S_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_S_nj6 = new TH1D("jet1_pt_S_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_S_nj6 = new TH1D("MR_S_nj6",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_S_nj6 = new TH1D("R2_S_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_S_nj6 = new TH1D("tau21_S_nj6", ";tau21", 200,0,1);
  h_tau32_S_nj6 = new TH1D("tau32_S_nj6", ";tau32", 200,0,1);
  h_MET_S_nj6 = new TH1D("MET_S_nj6", ";MET", 400,0,2000);

  h_ht_AK4_s = new TH1D("ht_AK4_s",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_s = new TH1D("jet1_pt_s",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_s = new TH1D("MR_s",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_s = new TH1D("R2_s",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_s = new TH1D("tau21_s", ";tau21", 200,0,1);
  h_tau32_s = new TH1D("tau32_s", ";tau32", 200,0,1);
  h_MET_s = new TH1D("MET_s", ";MET", 400,0,2000);
  h_AK8_jet1_pt_s = new TH1D("AK8_jet1_pt_s",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_s = new TH1D("AK8_jet1_eta_s",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_s = new TH1D("MET_phi_s", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_s_nj35 = new TH1D("AK8_jet1_pt_s_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_s_nj35 = new TH1D("AK8_jet1_eta_s_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_s_nj35 = new TH1D("MET_phi_s_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_s_nj6 = new TH1D("AK8_jet1_pt_s_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_s_nj6 = new TH1D("AK8_jet1_eta_s_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_s_nj6 = new TH1D("MET_phi_s_nj6", ";#phi_{MET}", 640, -3.2,3.2);
  h_mDPhi_nomDPhi_s = new TH1D("mDPhi_nomDPhi_s", ";#Delta#phi_{megajets}", 64,0,3.2);
  h_mDPhi_nomDPhi_s_nj35 = new TH1D("mDPhi_nomDPhi_s_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);
  h_mDPhi_nomDPhi_s_nj6 = new TH1D("mDPhi_nomDPhi_s_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);
#if TOP == 0
  h_nW_nonW_s = new TH1D("nW_nonW_s", ";# of Wjet", 10,0,10);
  h_nW_nonW_s_nj35 = new TH1D("nW_nonW_s_nj35", ";# of Wjet", 10,0,10);
  h_nW_nonW_s_nj6 = new TH1D("nW_nonW_s_nj6", ";# of Wjet", 10,0,10);
  h_nb_nonb_s = new TH1D("nb_nonb_s", ";# of bjet", 10,0,10);
  h_nb_nonb_s_nj35 = new TH1D("nb_nonb_s_nj35", ";# of bjet", 10,0,10);
  h_nb_nonb_s_nj6 = new TH1D("nb_nonb_s_nj6", ";# of bjet", 10,0,10);
#else
  h_nTop_nonTop_s = new TH1D("nTop_nonTop_s", ";# of Topjet", 10,0,10);
  h_nTop_nonTop_s_nj35 = new TH1D("nTop_nonTop_s_nj35", ";# of Topjet", 10,0,10);
  h_nTop_nonTop_s_nj6 = new TH1D("nTop_nonTop_s_nj6", ";# of Topjet", 10,0,10);
#endif

#if TOP == 0
  h_nW_nonW_q = new TH1D("nW_nonW_q", ";# of Wjet", 10,0,10);
  h_nW_nonW_q_nj35 = new TH1D("nW_nonW_q_nj35", ";# of Wjet", 10,0,10);
  h_nW_nonW_q_nj6 = new TH1D("nW_nonW_q_nj6", ";# of Wjet", 10,0,10);
#else
  h_nTop_nonTop_q = new TH1D("nTop_nonTop_q", ";# of Topjet", 10,0,10);
  h_nTop_nonTop_q_nj35 = new TH1D("nTop_nonTop_q_nj35", ";# of Topjet", 10,0,10);
  h_nTop_nonTop_q_nj6 = new TH1D("nTop_nonTop_q_nj6", ";# of Topjet", 10,0,10);
#endif
  h_nb_nonb_q = new TH1D("nb_nonb_q", ";# of bjet", 10,0,10);
  h_nb_nonb_q_nj35 = new TH1D("nb_nonb_q_nj35", ";# of bjet", 10,0,10);
  h_nb_nonb_q_nj6 = new TH1D("nb_nonb_q_nj6", ";# of bjet", 10,0,10);

  h_ht_AK4_s_nj35 = new TH1D("ht_AK4_s_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_s_nj35 = new TH1D("jet1_pt_s_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_s_nj35 = new TH1D("MR_s_nj35",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_s_nj35 = new TH1D("R2_s_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_s_nj35 = new TH1D("tau21_s_nj35", ";tau21", 200,0,1);
  h_tau32_s_nj35 = new TH1D("tau32_s_nj35", ";tau32", 200,0,1);
  h_MET_s_nj35 = new TH1D("MET_s_nj35", ";MET", 400,0,2000);

  h_ht_AK4_s_nj6 = new TH1D("ht_AK4_s_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_s_nj6 = new TH1D("jet1_pt_s_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_s_nj6 = new TH1D("MR_s_nj6",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_s_nj6 = new TH1D("R2_s_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_s_nj6 = new TH1D("tau21_s_nj6", ";tau21", 200,0,1);
  h_tau32_s_nj6 = new TH1D("tau32_s_nj6", ";tau32", 200,0,1);
  h_MET_s_nj6 = new TH1D("MET_s_nj6", ";MET", 400,0,2000);

  h_ht_AK4_Z = new TH1D("ht_AK4_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Z = new TH1D("jet1_pt_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Z = new TH1D("MR_Z",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_Z = new TH1D("R2_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Z = new TH1D("tau21_Z", ";tau21", 200,0,1);
  h_tau32_Z = new TH1D("tau32_Z", ";tau32", 200,0,1);
  h_MET_Z = new TH1D("MET_Z", ";MET", 400,0,2000);
  h_AK8_jet1_pt_Z = new TH1D("AK8_jet1_pt_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Z = new TH1D("AK8_jet1_eta_Z",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Z = new TH1D("MET_phi_Z", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_Z_nj35 = new TH1D("AK8_jet1_pt_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Z_nj35 = new TH1D("AK8_jet1_eta_Z_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Z_nj35 = new TH1D("MET_phi_Z_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_Z_nj6 = new TH1D("AK8_jet1_pt_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Z_nj6 = new TH1D("AK8_jet1_eta_Z_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Z_nj6 = new TH1D("MET_phi_Z_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_Z_nj35 = new TH1D("ht_AK4_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Z_nj35 = new TH1D("jet1_pt_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Z_nj35 = new TH1D("MR_Z_nj35",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_Z_nj35 = new TH1D("R2_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Z_nj35 = new TH1D("tau21_Z_nj35", ";tau21", 200,0,1);
  h_tau32_Z_nj35 = new TH1D("tau32_Z_nj35", ";tau32", 200,0,1);
  h_MET_Z_nj35 = new TH1D("MET_Z_nj35", ";MET", 400,0,2000);

  h_ht_AK4_Z_nj6 = new TH1D("ht_AK4_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Z_nj6 = new TH1D("jet1_pt_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Z_nj6 = new TH1D("MR_Z_nj6",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_Z_nj6 = new TH1D("R2_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Z_nj6 = new TH1D("tau21_Z_nj6", ";tau21", 200,0,1);
  h_tau32_Z_nj6 = new TH1D("tau32_Z_nj6", ";tau32", 200,0,1);
  h_MET_Z_nj6 = new TH1D("MET_Z_nj6", ";MET", 400,0,2000);

  h_ht_AK4_no2Lep_Z = new TH1D("ht_AK4_no2Lep_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no2Lep_Z = new TH1D("jet1_pt_no2Lep_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no2Lep_Z = new TH1D("MR_no2Lep_Z",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no2Lep_Z = new TH1D("R2_no2Lep_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no2Lep_Z = new TH1D("MET_no2Lep_Z", ";MET", 400,0,2000);
  h_nLep_no2Lep_Z = new TH1D("nLep_no2Lep_Z", ";# of Lepton", 10,0,10);

  h_ht_AK4_nomDPhi_Z = new TH1D("ht_AK4_nomDPhi_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Z = new TH1D("jet1_pt_nomDPhi_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Z = new TH1D("MR_nomDPhi_Z",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Z = new TH1D("R2_nomDPhi_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Z = new TH1D("MET_nomDPhi_Z", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Z = new TH1D("mDPhi_nomDPhi_Z", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_noMll_Z = new TH1D("ht_AK4_noMll_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMll_Z = new TH1D("jet1_pt_noMll_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMll_Z = new TH1D("MR_noMll_Z",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMll_Z = new TH1D("R2_noMll_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMll_Z = new TH1D("MET_noMll_Z", ";MET", 400,0,2000);
  h_Mll_noMll_Z = new TH1D("Mll_noMll_Z", ";Mass_{two leptons}", 50,50,150);

  h_ht_AK4_no2Lep_Z_nj35 = new TH1D("ht_AK4_no2Lep_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no2Lep_Z_nj35 = new TH1D("jet1_pt_no2Lep_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no2Lep_Z_nj35 = new TH1D("MR_no2Lep_Z_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no2Lep_Z_nj35 = new TH1D("R2_no2Lep_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no2Lep_Z_nj35 = new TH1D("MET_no2Lep_Z_nj35", ";MET", 400,0,2000);
  h_nLep_no2Lep_Z_nj35 = new TH1D("nLep_no2Lep_Z_nj35", ";# of Lepton", 10,0,10);

  h_ht_AK4_noMll_Z_nj35 = new TH1D("ht_AK4_noMll_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMll_Z_nj35 = new TH1D("jet1_pt_noMll_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMll_Z_nj35 = new TH1D("MR_noMll_Z_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMll_Z_nj35 = new TH1D("R2_noMll_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMll_Z_nj35 = new TH1D("MET_noMll_Z_nj35", ";MET", 400,0,2000);
  h_Mll_noMll_Z_nj35 = new TH1D("Mll_noMll_Z_nj35", ";Mass_{two leptons}", 50,50,150);

  h_ht_AK4_nomDPhi_Z_nj35 = new TH1D("ht_AK4_nomDPhi_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Z_nj35 = new TH1D("jet1_pt_nomDPhi_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Z_nj35 = new TH1D("MR_nomDPhi_Z_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Z_nj35 = new TH1D("R2_nomDPhi_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Z_nj35 = new TH1D("MET_nomDPhi_Z_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Z_nj35 = new TH1D("mDPhi_nomDPhi_Z_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no2Lep_Z_nj6 = new TH1D("ht_AK4_no2Lep_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no2Lep_Z_nj6 = new TH1D("jet1_pt_no2Lep_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no2Lep_Z_nj6 = new TH1D("MR_no2Lep_Z_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no2Lep_Z_nj6 = new TH1D("R2_no2Lep_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no2Lep_Z_nj6 = new TH1D("MET_no2Lep_Z_nj6", ";MET", 400,0,2000);
  h_nLep_no2Lep_Z_nj6 = new TH1D("nLep_no2Lep_Z_nj6", ";# of Lepton", 10,0,10);

  h_ht_AK4_noMll_Z_nj6 = new TH1D("ht_AK4_noMll_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMll_Z_nj6 = new TH1D("jet1_pt_noMll_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMll_Z_nj6 = new TH1D("MR_noMll_Z_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMll_Z_nj6 = new TH1D("R2_noMll_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMll_Z_nj6 = new TH1D("MET_noMll_Z_nj6", ";MET", 400,0,2000);
  h_Mll_noMll_Z_nj6 = new TH1D("Mll_noMll_Z_nj6", ";Mass_{two leptons}", 50,50,150);

  h_ht_AK4_nomDPhi_Z_nj6 = new TH1D("ht_AK4_nomDPhi_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Z_nj6 = new TH1D("jet1_pt_nomDPhi_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Z_nj6 = new TH1D("MR_nomDPhi_Z_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Z_nj6 = new TH1D("R2_nomDPhi_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Z_nj6 = new TH1D("MET_nomDPhi_Z_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Z_nj6 = new TH1D("mDPhi_nomDPhi_Z_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

#if TOP == 0
  h_ht_AK4_nonW_Z = new TH1D("ht_AK4_nonW_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Z = new TH1D("jet1_pt_nonW_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Z = new TH1D("MR_nonW_Z",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Z = new TH1D("R2_nonW_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Z = new TH1D("MET_nonW_Z", ";MET", 400,0,2000);
  h_nW_nonW_Z = new TH1D("nW_nonW_Z", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_Z_nj35 = new TH1D("ht_AK4_nonW_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Z_nj35 = new TH1D("jet1_pt_nonW_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Z_nj35 = new TH1D("MR_nonW_Z_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Z_nj35 = new TH1D("R2_nonW_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Z_nj35 = new TH1D("MET_nonW_Z_nj35", ";MET", 400,0,2000);
  h_nW_nonW_Z_nj35 = new TH1D("nW_nonW_Z_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_Z_nj6 = new TH1D("ht_AK4_nonW_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Z_nj6 = new TH1D("jet1_pt_nonW_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Z_nj6 = new TH1D("MR_nonW_Z_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Z_nj6 = new TH1D("R2_nonW_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Z_nj6 = new TH1D("MET_nonW_Z_nj6", ";MET", 400,0,2000);
  h_nW_nonW_Z_nj6 = new TH1D("nW_nonW_Z_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_Z = new TH1D("ht_AK4_nonTop_Z",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Z = new TH1D("jet1_pt_nonTop_Z",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Z = new TH1D("MR_nonTop_Z",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Z = new TH1D("R2_nonTop_Z",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Z = new TH1D("MET_nonTop_Z", ";MET", 400,0,2000);
  h_nTop_nonTop_Z = new TH1D("nTop_nonTop_Z", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_Z_nj35 = new TH1D("ht_AK4_nonTop_Z_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Z_nj35 = new TH1D("jet1_pt_nonTop_Z_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Z_nj35 = new TH1D("MR_nonTop_Z_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Z_nj35 = new TH1D("R2_nonTop_Z_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Z_nj35 = new TH1D("MET_nonTop_Z_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_Z_nj35 = new TH1D("nTop_nonTop_Z_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_Z_nj6 = new TH1D("ht_AK4_nonTop_Z_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Z_nj6 = new TH1D("jet1_pt_nonTop_Z_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Z_nj6 = new TH1D("MR_nonTop_Z_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Z_nj6 = new TH1D("R2_nonTop_Z_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Z_nj6 = new TH1D("MET_nonTop_Z_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_Z_nj6 = new TH1D("nTop_nonTop_Z_nj6", ";# of Topjet", 10,0,10);
#endif

  h_ht_AK4_L = new TH1D("ht_AK4_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_L = new TH1D("jet1_pt_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_L = new TH1D("MR_L",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_L = new TH1D("R2_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_L = new TH1D("tau21_L", ";tau21", 200,0,1);
  h_tau32_L = new TH1D("tau32_L", ";tau32", 200,0,1);
  h_MET_L = new TH1D("MET_L", ";MET", 400,0,2000);
  h_AK8_jet1_pt_L = new TH1D("AK8_jet1_pt_L",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_L = new TH1D("AK8_jet1_eta_L",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_L = new TH1D("MET_phi_L", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_L_nj35 = new TH1D("AK8_jet1_pt_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_L_nj35 = new TH1D("AK8_jet1_eta_L_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_L_nj35 = new TH1D("MET_phi_L_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_L_nj6 = new TH1D("AK8_jet1_pt_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_L_nj6 = new TH1D("AK8_jet1_eta_L_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_L_nj6 = new TH1D("MET_phi_L_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_L_nj35 = new TH1D("ht_AK4_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_L_nj35 = new TH1D("jet1_pt_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_L_nj35 = new TH1D("MR_L_nj35",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_L_nj35 = new TH1D("R2_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_L_nj35 = new TH1D("tau21_L_nj35", ";tau21", 200,0,1);
  h_tau32_L_nj35 = new TH1D("tau32_L_nj35", ";tau32", 200,0,1);
  h_MET_L_nj35 = new TH1D("MET_L_nj35", ";MET", 400,0,2000);

  h_ht_AK4_L_nj6 = new TH1D("ht_AK4_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_L_nj6 = new TH1D("jet1_pt_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_L_nj6 = new TH1D("MR_L_nj6",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_L_nj6 = new TH1D("R2_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_L_nj6 = new TH1D("tau21_L_nj6", ";tau21", 200,0,1);
  h_tau32_L_nj6 = new TH1D("tau32_L_nj6", ";tau32", 200,0,1);
  h_MET_L_nj6 = new TH1D("MET_L_nj6", ";MET", 400,0,2000);

  h_ht_AK4_no1Lep_L = new TH1D("ht_AK4_no1Lep_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_L = new TH1D("jet1_pt_no1Lep_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_L = new TH1D("MR_no1Lep_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_L = new TH1D("R2_no1Lep_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_L = new TH1D("MET_no1Lep_L", ";MET", 400,0,2000);
  h_nLep_no1Lep_L = new TH1D("nLep_no1Lep_L", ";# of Lepton", 10,0,10);

  h_ht_AK4_no0b_L = new TH1D("ht_AK4_no0b_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0b_L = new TH1D("jet1_pt_no0b_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0b_L = new TH1D("MR_no0b_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0b_L = new TH1D("R2_no0b_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0b_L = new TH1D("MET_no0b_L", ";MET", 400,0,2000);
  h_nb_no0b_L = new TH1D("nb_no0b_L", ";# of b jet", 10,0,10);

  h_ht_AK4_nomDPhi_L = new TH1D("ht_AK4_nomDPhi_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_L = new TH1D("jet1_pt_nomDPhi_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_L = new TH1D("MR_nomDPhi_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_L = new TH1D("R2_nomDPhi_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_L = new TH1D("MET_nomDPhi_L", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_L = new TH1D("mDPhi_nomDPhi_L", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_noMT_L = new TH1D("ht_AK4_noMT_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_L = new TH1D("jet1_pt_noMT_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_L = new TH1D("MR_noMT_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_L = new TH1D("R2_noMT_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_L = new TH1D("MET_noMT_L", ";MET", 400,0,2000);
  h_MT_noMT_L = new TH1D("MT_noMT_L", ";MT", 20,0,500);

  h_ht_AK4_no1Lep_L_nj35 = new TH1D("ht_AK4_no1Lep_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_L_nj35 = new TH1D("jet1_pt_no1Lep_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_L_nj35 = new TH1D("MR_no1Lep_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_L_nj35 = new TH1D("R2_no1Lep_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_L_nj35 = new TH1D("MET_no1Lep_L_nj35", ";MET", 400,0,2000);
  h_nLep_no1Lep_L_nj35 = new TH1D("nLep_no1Lep_L_nj35", ";# of Lepton", 10,0,10);

  h_ht_AK4_no0b_L_nj35 = new TH1D("ht_AK4_no0b_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0b_L_nj35 = new TH1D("jet1_pt_no0b_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0b_L_nj35 = new TH1D("MR_no0b_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0b_L_nj35 = new TH1D("R2_no0b_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0b_L_nj35 = new TH1D("MET_no0b_L_nj35", ";MET", 400,0,2000);
  h_nb_no0b_L_nj35 = new TH1D("nb_no0b_L_nj35", ";# of b jet", 10,0,10);

  h_ht_AK4_nomDPhi_L_nj35 = new TH1D("ht_AK4_nomDPhi_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_L_nj35 = new TH1D("jet1_pt_nomDPhi_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_L_nj35 = new TH1D("MR_nomDPhi_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_L_nj35 = new TH1D("R2_nomDPhi_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_L_nj35 = new TH1D("MET_nomDPhi_L_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_L_nj35 = new TH1D("mDPhi_nomDPhi_L_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_noMT_L_nj35 = new TH1D("ht_AK4_noMT_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_L_nj35 = new TH1D("jet1_pt_noMT_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_L_nj35 = new TH1D("MR_noMT_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_L_nj35 = new TH1D("R2_noMT_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_L_nj35 = new TH1D("MET_noMT_L_nj35", ";MET", 400,0,2000);
  h_MT_noMT_L_nj35 = new TH1D("MT_noMT_L_nj35", ";MT", 20,0,500);

  h_ht_AK4_no1Lep_L_nj6 = new TH1D("ht_AK4_no1Lep_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_L_nj6 = new TH1D("jet1_pt_no1Lep_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_L_nj6 = new TH1D("MR_no1Lep_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_L_nj6 = new TH1D("R2_no1Lep_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_L_nj6 = new TH1D("MET_no1Lep_L_nj6", ";MET", 400,0,2000);
  h_nLep_no1Lep_L_nj6 = new TH1D("nLep_no1Lep_L_nj6", ";# of Lepton", 10,0,10);

  h_ht_AK4_no0b_L_nj6 = new TH1D("ht_AK4_no0b_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0b_L_nj6 = new TH1D("jet1_pt_no0b_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0b_L_nj6 = new TH1D("MR_no0b_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0b_L_nj6 = new TH1D("R2_no0b_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0b_L_nj6 = new TH1D("MET_no0b_L_nj6", ";MET", 400,0,2000);
  h_nb_no0b_L_nj6 = new TH1D("nb_no0b_L_nj6", ";# of b jet", 10,0,10);

  h_ht_AK4_nomDPhi_L_nj6 = new TH1D("ht_AK4_nomDPhi_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_L_nj6 = new TH1D("jet1_pt_nomDPhi_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_L_nj6 = new TH1D("MR_nomDPhi_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_L_nj6 = new TH1D("R2_nomDPhi_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_L_nj6 = new TH1D("MET_nomDPhi_L_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_L_nj6 = new TH1D("mDPhi_nomDPhi_L_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_noMT_L_nj6 = new TH1D("ht_AK4_noMT_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_L_nj6 = new TH1D("jet1_pt_noMT_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_L_nj6 = new TH1D("MR_noMT_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_L_nj6 = new TH1D("R2_noMT_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_L_nj6 = new TH1D("MET_noMT_L_nj6", ";MET", 400,0,2000);
  h_MT_noMT_L_nj6 = new TH1D("MT_noMT_L_nj6", ";MT", 20,0,500);

#if TOP == 0
  h_ht_AK4_nonW_L = new TH1D("ht_AK4_nonW_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_L = new TH1D("jet1_pt_nonW_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_L = new TH1D("MR_nonW_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_L = new TH1D("R2_nonW_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_L = new TH1D("MET_nonW_L", ";MET", 400,0,2000);
  h_nW_nonW_L = new TH1D("nW_nonW_L", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_L_nj35 = new TH1D("ht_AK4_nonW_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_L_nj35 = new TH1D("jet1_pt_nonW_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_L_nj35 = new TH1D("MR_nonW_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_L_nj35 = new TH1D("R2_nonW_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_L_nj35 = new TH1D("MET_nonW_L_nj35", ";MET", 400,0,2000);
  h_nW_nonW_L_nj35 = new TH1D("nW_nonW_L_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_L_nj6 = new TH1D("ht_AK4_nonW_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_L_nj6 = new TH1D("jet1_pt_nonW_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_L_nj6 = new TH1D("MR_nonW_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_L_nj6 = new TH1D("R2_nonW_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_L_nj6 = new TH1D("MET_nonW_L_nj6", ";MET", 400,0,2000);
  h_nW_nonW_L_nj6 = new TH1D("nW_nonW_L_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_L = new TH1D("ht_AK4_nonTop_L",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_L = new TH1D("jet1_pt_nonTop_L",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_L = new TH1D("MR_nonTop_L",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_L = new TH1D("R2_nonTop_L",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_L = new TH1D("MET_nonTop_L", ";MET", 400,0,2000);
  h_nTop_nonTop_L = new TH1D("nTop_nonTop_L", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_L_nj35 = new TH1D("ht_AK4_nonTop_L_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_L_nj35 = new TH1D("jet1_pt_nonTop_L_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_L_nj35 = new TH1D("MR_nonTop_L_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_L_nj35 = new TH1D("R2_nonTop_L_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_L_nj35 = new TH1D("MET_nonTop_L_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_L_nj35 = new TH1D("nTop_nonTop_L_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_L_nj6 = new TH1D("ht_AK4_nonTop_L_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_L_nj6 = new TH1D("jet1_pt_nonTop_L_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_L_nj6 = new TH1D("MR_nonTop_L_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_L_nj6 = new TH1D("R2_nonTop_L_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_L_nj6 = new TH1D("MET_nonTop_L_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_L_nj6 = new TH1D("nTop_nonTop_L_nj6", ";# of Topjet", 10,0,10);
#endif

  h_ht_AK4_G = new TH1D("ht_AK4_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_G = new TH1D("jet1_pt_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_G = new TH1D("MR_G",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_G = new TH1D("R2_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_G = new TH1D("tau21_G", ";tau21", 200,0,1);
  h_tau32_G = new TH1D("tau32_G", ";tau32", 200,0,1);
  h_MET_G = new TH1D("MET_G", ";MET", 400,0,2000);
  h_AK8_jet1_pt_G = new TH1D("AK8_jet1_pt_G",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_G = new TH1D("AK8_jet1_eta_G",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_G = new TH1D("MET_phi_G", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_G_nj35 = new TH1D("AK8_jet1_pt_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_G_nj35 = new TH1D("AK8_jet1_eta_G_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_G_nj35 = new TH1D("MET_phi_G_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_G_nj6 = new TH1D("AK8_jet1_pt_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_G_nj6 = new TH1D("AK8_jet1_eta_G_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_G_nj6 = new TH1D("MET_phi_G_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_G_nj35 = new TH1D("ht_AK4_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_G_nj35 = new TH1D("jet1_pt_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_G_nj35 = new TH1D("MR_G_nj35",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_G_nj35 = new TH1D("R2_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_G_nj35 = new TH1D("tau21_G_nj35", ";tau21", 200,0,1);
  h_tau32_G_nj35 = new TH1D("tau32_G_nj35", ";tau32", 200,0,1);
  h_MET_G_nj35 = new TH1D("MET_G_nj35", ";MET", 400,0,2000);

  h_ht_AK4_G_nj6 = new TH1D("ht_AK4_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_G_nj6 = new TH1D("jet1_pt_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_G_nj6 = new TH1D("MR_G_nj6",   ";MR_{AK4}",     nbn_MR,    bn_MR);
  h_R2_G_nj6 = new TH1D("R2_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_G_nj6 = new TH1D("tau21_G_nj6", ";tau21", 200,0,1);
  h_tau32_G_nj6 = new TH1D("tau32_G_nj6", ";tau32", 200,0,1);
  h_MET_G_nj6 = new TH1D("MET_G_nj6", ";MET", 400,0,2000);

  h_ht_AK4_no0Lep_G = new TH1D("ht_AK4_0Lep_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_G = new TH1D("jet1_pt_0Lep_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_G = new TH1D("MR_0Lep_G",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_G = new TH1D("R2_0Lep_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_G = new TH1D("MET_0Lep_G", ";MET", 400,0,2000);
  h_nLep_no0Lep_G = new TH1D("nLep_0Lep_G", ";# of Lepton", 10,0,10);

  h_ht_AK4_no1Pho_G = new TH1D("ht_AK4_1Pho_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Pho_G = new TH1D("jet1_pt_1Pho_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Pho_G = new TH1D("MR_1Pho_G",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Pho_G = new TH1D("R2_1Pho_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Pho_G = new TH1D("MET_1Pho_G", ";MET", 400,0,2000);
  h_nPho_no1Pho_G = new TH1D("nPho_1Pho_G", ";# of photon", 10,0,10);

  h_ht_AK4_nomDPhi_G = new TH1D("ht_AK4_nomDPhi_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_G = new TH1D("jet1_pt_nomDPhi_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_G = new TH1D("MR_nomDPhi_G",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_G = new TH1D("R2_nomDPhi_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_G = new TH1D("MET_nomDPhi_G", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_G = new TH1D("mDPhi_nomDPhi_G", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no0Lep_G_nj35 = new TH1D("ht_AK4_0Lep_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_G_nj35 = new TH1D("jet1_pt_0Lep_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_G_nj35 = new TH1D("MR_0Lep_G_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_G_nj35 = new TH1D("R2_0Lep_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_G_nj35 = new TH1D("MET_0Lep_G_nj35", ";MET", 400,0,2000);
  h_nLep_no0Lep_G_nj35 = new TH1D("nLep_0Lep_G_nj35", ";# of Lepton", 10,0,10);

  h_ht_AK4_no1Pho_G_nj35 = new TH1D("ht_AK4_1Pho_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Pho_G_nj35 = new TH1D("jet1_pt_1Pho_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Pho_G_nj35 = new TH1D("MR_1Pho_G_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Pho_G_nj35 = new TH1D("R2_1Pho_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Pho_G_nj35 = new TH1D("MET_1Pho_G_nj35", ";MET", 400,0,2000);
  h_nPho_no1Pho_G_nj35 = new TH1D("nPho_1Pho_G_nj35", ";# of photon", 10,0,10);

  h_ht_AK4_nomDPhi_G_nj35 = new TH1D("ht_AK4_nomDPhi_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_G_nj35 = new TH1D("jet1_pt_nomDPhi_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_G_nj35 = new TH1D("MR_nomDPhi_G_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_G_nj35 = new TH1D("R2_nomDPhi_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_G_nj35 = new TH1D("MET_nomDPhi_G_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_G_nj35 = new TH1D("mDPhi_nomDPhi_G_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no0Lep_G_nj6 = new TH1D("ht_AK4_0Lep_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_G_nj6 = new TH1D("jet1_pt_0Lep_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_G_nj6 = new TH1D("MR_0Lep_G_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_G_nj6 = new TH1D("R2_0Lep_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_G_nj6 = new TH1D("MET_0Lep_G_nj6", ";MET", 400,0,2000);
  h_nLep_no0Lep_G_nj6 = new TH1D("nLep_0Lep_G_nj6", ";# of Lepton", 10,0,10);

  h_ht_AK4_no1Pho_G_nj6 = new TH1D("ht_AK4_1Pho_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Pho_G_nj6 = new TH1D("jet1_pt_1Pho_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Pho_G_nj6 = new TH1D("MR_1Pho_G_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Pho_G_nj6 = new TH1D("R2_1Pho_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Pho_G_nj6 = new TH1D("MET_1Pho_G_nj6", ";MET", 400,0,2000);
  h_nPho_no1Pho_G_nj6 = new TH1D("nPho_1Pho_G_nj6", ";# of photon", 10,0,10);

  h_ht_AK4_nomDPhi_G_nj6 = new TH1D("ht_AK4_nomDPhi_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_G_nj6 = new TH1D("jet1_pt_nomDPhi_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_G_nj6 = new TH1D("MR_nomDPhi_G_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_G_nj6 = new TH1D("R2_nomDPhi_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_G_nj6 = new TH1D("MET_nomDPhi_G_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_G_nj6 = new TH1D("mDPhi_nomDPhi_G_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

#if TOP == 0
  h_ht_AK4_nonW_G = new TH1D("ht_AK4_nonW_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_G = new TH1D("jet1_pt_nonW_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_G = new TH1D("MR_nonW_G",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_G = new TH1D("R2_nonW_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_G = new TH1D("MET_nonW_G", ";MET", 400,0,2000);
  h_nW_nonW_G = new TH1D("nW_nonW_G", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_G_nj35 = new TH1D("ht_AK4_nonW_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_G_nj35 = new TH1D("jet1_pt_nonW_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_G_nj35 = new TH1D("MR_nonW_G_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_G_nj35 = new TH1D("R2_nonW_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_G_nj35 = new TH1D("MET_nonW_G_nj35", ";MET", 400,0,2000);
  h_nW_nonW_G_nj35 = new TH1D("nW_nonW_G_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_G_nj6 = new TH1D("ht_AK4_nonW_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_G_nj6 = new TH1D("jet1_pt_nonW_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_G_nj6 = new TH1D("MR_nonW_G_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_G_nj6 = new TH1D("R2_nonW_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_G_nj6 = new TH1D("MET_nonW_G_nj6", ";MET", 400,0,2000);
  h_nW_nonW_G_nj6 = new TH1D("nW_nonW_G_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_G = new TH1D("ht_AK4_nonTop_G",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_G = new TH1D("jet1_pt_nonTop_G",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_G = new TH1D("MR_nonTop_G",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_G = new TH1D("R2_nonTop_G",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_G = new TH1D("MET_nonTop_G", ";MET", 400,0,2000);
  h_nTop_nonTop_G = new TH1D("nTop_nonTop_G", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_G_nj35 = new TH1D("ht_AK4_nonTop_G_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_G_nj35 = new TH1D("jet1_pt_nonTop_G_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_G_nj35 = new TH1D("MR_nonTop_G_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_G_nj35 = new TH1D("R2_nonTop_G_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_G_nj35 = new TH1D("MET_nonTop_G_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_G_nj35 = new TH1D("nTop_nonTop_G_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_G_nj6 = new TH1D("ht_AK4_nonTop_G_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_G_nj6 = new TH1D("jet1_pt_nonTop_G_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_G_nj6 = new TH1D("MR_nonTop_G_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_G_nj6 = new TH1D("R2_nonTop_G_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_G_nj6 = new TH1D("MET_nonTop_G_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_G_nj6 = new TH1D("nTop_nonTop_G_nj6", ";# of Topjet", 10,0,10);
#endif

  //h_MR_S_nj6 = new TH1D("MR_S_nj6",";MR_{AK4}",nbn_MR,bn_MR);
  //h_MR_S_nj35 = new TH1D("MR_S_nj35",";MR_{AK4}",nbn_MR,bn_MR);
  //h_R2_S_nj6 = new TH1D("R2_S_nj6",";R^{2}_{AK4}",nbn_R2,bn_R2);
  //h_R2_S_nj35 = new TH1D("R2_S_nj35",";R^{2}_{AK4}",nbn_R2,bn_R2);

  //h_MR_Q_nj6 = new TH1D("MR_Q_nj6",";MR_{AK4}",nbn_MR,bn_MR);
  //h_MR_Q_nj35 = new TH1D("MR_Q_nj35",";MR_{AK4}",nbn_MR,bn_MR);
  //h_R2_Q_nj35 = new TH1D("R2_Q_nj35",";R^{2}_{AK4}",nbn_R2,bn_R2);
  //h_R2_Q_nj6 = new TH1D("R2_Q_nj6",";R^{2}_{AK4}",nbn_R2,bn_R2);

  //h_MR_T_nj6 = new TH1D("MR_T_nj6",";MR_{AK4}",nbn_MR,bn_MR);
  //h_MR_T_nj35 = new TH1D("MR_T_nj35",";MR_{AK4}",nbn_MR,bn_MR);
  //h_R2_T_nj35 = new TH1D("R2_T_nj35",";R^{2}_{AK4}",nbn_R2,bn_R2);
  //h_R2_T_nj6 = new TH1D("R2_T_nj6",";R^{2}_{AK4}",nbn_R2,bn_R2);

  //h_MR_W_nj6 = new TH1D("MR_W_nj6",";MR_{AK4}",nbn_MR,bn_MR);
  //h_MR_W_nj35 = new TH1D("MR_W_nj35",";MR_{AK4}",nbn_MR,bn_MR);
  //h_R2_W_nj35 = new TH1D("R2_W_nj35",";R^{2}_{AK4}",nbn_R2,bn_R2);
  //h_R2_W_nj6 = new TH1D("R2_W_nj6",";R^{2}_{AK4}",nbn_R2,bn_R2);

  h_ht_AK4_Q = new TH1D("ht_AK4_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Q = new TH1D("jet1_pt_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Q = new TH1D("MR_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_Q = new TH1D("R2_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Q = new TH1D("tau21_Q", ";tau21", 200,0,1);
  h_tau32_Q = new TH1D("tau32_Q", ";tau32", 200,0,1);
  h_MET_Q = new TH1D("MET_Q", ";MET", 400,0,2000);
  h_AK8_jet1_pt_Q = new TH1D("AK8_jet1_pt_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Q = new TH1D("AK8_jet1_eta_Q",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Q = new TH1D("MET_phi_Q", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_Q_nj35 = new TH1D("AK8_jet1_pt_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Q_nj35 = new TH1D("AK8_jet1_eta_Q_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Q_nj35 = new TH1D("MET_phi_Q_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_Q_nj6 = new TH1D("AK8_jet1_pt_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_Q_nj6 = new TH1D("AK8_jet1_eta_Q_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_Q_nj6 = new TH1D("MET_phi_Q_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_nomDPhi_Q = new TH1D("ht_AK4_nomDPhi_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Q = new TH1D("jet1_pt_nomDPhi_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Q = new TH1D("MR_nomDPhi_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Q = new TH1D("R2_nomDPhi_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Q = new TH1D("MET_nomDPhi_Q", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Q = new TH1D("mDPhi_nomDPhi_Q", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no0Lep_Q = new TH1D("ht_AK4_no0Lep_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_Q = new TH1D("jet1_pt_no0Lep_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_Q = new TH1D("MR_no0Lep_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_Q = new TH1D("R2_no0Lep_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_Q = new TH1D("MET_no0Lep_Q", ";MET", 400,0,2000);
  h_nLep_no0Lep_Q = new TH1D("nLep_no0Lep_Q", ";# of lepton", 10,0,10);

  h_ht_AK4_nonb_Q = new TH1D("ht_AK4_nonb_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_Q = new TH1D("jet1_pt_nonb_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_Q = new TH1D("MR_nonb_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_Q = new TH1D("R2_nonb_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_Q = new TH1D("MET_nonb_Q", ";MET", 400,0,2000);
  h_nb_nonb_Q = new TH1D("nb_nonb_Q", ";# of b jet", 10,0,10);

  h_ht_AK4_Q_nj35 = new TH1D("ht_AK4_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Q_nj35 = new TH1D("jet1_pt_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Q_nj35 = new TH1D("MR_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_Q_nj35 = new TH1D("R2_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Q_nj35 = new TH1D("tau21_Q_nj35", ";tau21", 200,0,1);
  h_tau32_Q_nj35 = new TH1D("tau32_Q_nj35", ";tau32", 200,0,1);
  h_MET_Q_nj35 = new TH1D("MET_Q_nj35", ";MET", 400,0,2000);

  h_ht_AK4_nomDPhi_Q_nj35 = new TH1D("ht_AK4_nomDPhi_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Q_nj35 = new TH1D("jet1_pt_nomDPhi_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Q_nj35 = new TH1D("MR_nomDPhi_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Q_nj35 = new TH1D("R2_nomDPhi_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Q_nj35 = new TH1D("MET_nomDPhi_Q_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Q_nj35 = new TH1D("mDPhi_nomDPhi_Q_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no0Lep_Q_nj35 = new TH1D("ht_AK4_no0Lep_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_Q_nj35 = new TH1D("jet1_pt_no0Lep_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_Q_nj35 = new TH1D("MR_no0Lep_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_Q_nj35 = new TH1D("R2_no0Lep_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_Q_nj35 = new TH1D("MET_no0Lep_Q_nj35", ";MET", 400,0,2000);
  h_nLep_no0Lep_Q_nj35 = new TH1D("nLep_no0Lep_Q_nj35", ";# of lepton", 10,0,10);

  h_ht_AK4_nonb_Q_nj35 = new TH1D("ht_AK4_nonb_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_Q_nj35 = new TH1D("jet1_pt_nonb_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_Q_nj35 = new TH1D("MR_nonb_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_Q_nj35 = new TH1D("R2_nonb_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_Q_nj35 = new TH1D("MET_nonb_Q_nj35", ";MET", 400,0,2000);
  h_nb_nonb_Q_nj35 = new TH1D("nb_nonb_Q_nj35", ";# of b jet", 10,0,10);

  h_ht_AK4_Q_nj6 = new TH1D("ht_AK4_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_Q_nj6 = new TH1D("jet1_pt_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_Q_nj6 = new TH1D("MR_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_Q_nj6 = new TH1D("R2_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_Q_nj6 = new TH1D("tau21_Q_nj6", ";tau21", 200,0,1);
  h_tau32_Q_nj6 = new TH1D("tau32_Q_nj6", ";tau32", 200,0,1);
  h_MET_Q_nj6 = new TH1D("MET_Q_nj6", ";MET", 400,0,2000);

  h_ht_AK4_nomDPhi_Q_nj6 = new TH1D("ht_AK4_nomDPhi_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_Q_nj6 = new TH1D("jet1_pt_nomDPhi_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_Q_nj6 = new TH1D("MR_nomDPhi_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_Q_nj6 = new TH1D("R2_nomDPhi_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_Q_nj6 = new TH1D("MET_nomDPhi_Q_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_Q_nj6 = new TH1D("mDPhi_nomDPhi_Q_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no0Lep_Q_nj6 = new TH1D("ht_AK4_no0Lep_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no0Lep_Q_nj6 = new TH1D("jet1_pt_no0Lep_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no0Lep_Q_nj6 = new TH1D("MR_no0Lep_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no0Lep_Q_nj6 = new TH1D("R2_no0Lep_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no0Lep_Q_nj6 = new TH1D("MET_no0Lep_Q_nj6", ";MET", 400,0,2000);
  h_nLep_no0Lep_Q_nj6 = new TH1D("nLep_no0Lep_Q_nj6", ";# of lepton", 10,0,10);

  h_ht_AK4_nonb_Q_nj6 = new TH1D("ht_AK4_nonb_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_Q_nj6 = new TH1D("jet1_pt_nonb_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_Q_nj6 = new TH1D("MR_nonb_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_Q_nj6 = new TH1D("R2_nonb_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_Q_nj6 = new TH1D("MET_nonb_Q_nj6", ";MET", 400,0,2000);
  h_nb_nonb_Q_nj6 = new TH1D("nb_nonb_Q_nj6", ";# of b jet", 10,0,10);

#if TOP == 0
  h_ht_AK4_nonW_Q = new TH1D("ht_AK4_nonW_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Q = new TH1D("jet1_pt_nonW_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Q = new TH1D("MR_nonW_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Q = new TH1D("R2_nonW_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Q = new TH1D("MET_nonW_Q", ";MET", 400,0,2000);
  h_nW_nonW_Q = new TH1D("nW_nonW_Q", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_Q_nj35 = new TH1D("ht_AK4_nonW_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Q_nj35 = new TH1D("jet1_pt_nonW_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Q_nj35 = new TH1D("MR_nonW_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Q_nj35 = new TH1D("R2_nonW_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Q_nj35 = new TH1D("MET_nonW_Q_nj35", ";MET", 400,0,2000);
  h_nW_nonW_Q_nj35 = new TH1D("nW_nonW_Q_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_Q_nj6 = new TH1D("ht_AK4_nonW_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_Q_nj6 = new TH1D("jet1_pt_nonW_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_Q_nj6 = new TH1D("MR_nonW_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_Q_nj6 = new TH1D("R2_nonW_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_Q_nj6 = new TH1D("MET_nonW_Q_nj6", ";MET", 400,0,2000);
  h_nW_nonW_Q_nj6 = new TH1D("nW_nonW_Q_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_Q = new TH1D("ht_AK4_nonTop_Q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Q = new TH1D("jet1_pt_nonTop_Q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Q = new TH1D("MR_nonTop_Q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Q = new TH1D("R2_nonTop_Q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Q = new TH1D("MET_nonTop_Q", ";MET", 400,0,2000);
  h_nTop_nonTop_Q = new TH1D("nTop_nonTop_Q", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_Q_nj35 = new TH1D("ht_AK4_nonTop_Q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Q_nj35 = new TH1D("jet1_pt_nonTop_Q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Q_nj35 = new TH1D("MR_nonTop_Q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Q_nj35 = new TH1D("R2_nonTop_Q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Q_nj35 = new TH1D("MET_nonTop_Q_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_Q_nj35 = new TH1D("nTop_nonTop_Q_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_Q_nj6 = new TH1D("ht_AK4_nonTop_Q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_Q_nj6 = new TH1D("jet1_pt_nonTop_Q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_Q_nj6 = new TH1D("MR_nonTop_Q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_Q_nj6 = new TH1D("R2_nonTop_Q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_Q_nj6 = new TH1D("MET_nonTop_Q_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_Q_nj6 = new TH1D("nTop_nonTop_Q_nj6", ";# of Topjet", 10,0,10);
#endif

  h_ht_AK4_T = new TH1D("ht_AK4_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_T = new TH1D("jet1_pt_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_T = new TH1D("MR_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_T = new TH1D("R2_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_T = new TH1D("tau21_T", ";tau21", 200,0,1);
  h_tau32_T = new TH1D("tau32_T", ";tau32", 200,0,1);
  h_MET_T = new TH1D("MET_T", ";MET", 400,0,2000);
  h_AK8_jet1_pt_T = new TH1D("AK8_jet1_pt_T",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_T = new TH1D("AK8_jet1_eta_T",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_T = new TH1D("MET_phi_T", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_T_nj35 = new TH1D("AK8_jet1_pt_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_T_nj35 = new TH1D("AK8_jet1_eta_T_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_T_nj35 = new TH1D("MET_phi_T_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_T_nj6 = new TH1D("AK8_jet1_pt_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_T_nj6 = new TH1D("AK8_jet1_eta_T_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_T_nj6 = new TH1D("MET_phi_T_nj6", ";#phi_{MET}", 640, -3.2,3.2);

#if TOP == 0
  h_ht_AK4_no1b_T = new TH1D("ht_AK4_no1b_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1b_T = new TH1D("jet1_pt_no1b_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1b_T = new TH1D("MR_no1b_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1b_T = new TH1D("R2_no1b_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1b_T = new TH1D("MET_no1b_T", ";MET", 400,0,2000);
  h_nb_no1b_T = new TH1D("nb_no1b_T", ";# of b jet", 10,0,10);
#endif
  h_ht_AK4_noMT_T = new TH1D("ht_AK4_noMT_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_T = new TH1D("jet1_pt_noMT_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_T = new TH1D("MR_noMT_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_T = new TH1D("R2_noMT_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_T = new TH1D("MET_noMT_T", ";MET", 400,0,2000);
  h_MT_noMT_T = new TH1D("MT_noMT_T", ";MT", 25,0,500);

  h_ht_AK4_nomDPhi_T = new TH1D("ht_AK4_nomDPhi_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_T = new TH1D("jet1_pt_nomDPhi_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_T = new TH1D("MR_nomDPhi_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_T = new TH1D("R2_nomDPhi_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_T = new TH1D("MET_nomDPhi_T", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_T = new TH1D("mDPhi_nomDPhi_T", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_T = new TH1D("ht_AK4_no1Lep_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_T = new TH1D("jet1_pt_no1Lep_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_T = new TH1D("MR_no1Lep_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_T = new TH1D("R2_no1Lep_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_T = new TH1D("MET_no1Lep_T", ";MET", 400,0,2000);
  h_nLep_no1Lep_T = new TH1D("nLep_no1Lep_T", ";# of Lepton", 10,0,10);

  h_ht_AK4_T_nj35 = new TH1D("ht_AK4_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_T_nj35 = new TH1D("jet1_pt_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_T_nj35 = new TH1D("MR_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_T_nj35 = new TH1D("R2_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_T_nj35 = new TH1D("tau21_T_nj35", ";tau21", 200,0,1);
  h_tau32_T_nj35 = new TH1D("tau32_T_nj35", ";tau32", 200,0,1);
  h_MET_T_nj35 = new TH1D("MET_T_nj35", ";MET", 400,0,2000);

#if TOP == 0
  h_ht_AK4_no1b_T_nj35 = new TH1D("ht_AK4_no1b_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1b_T_nj35 = new TH1D("jet1_pt_no1b_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1b_T_nj35 = new TH1D("MR_no1b_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1b_T_nj35 = new TH1D("R2_no1b_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1b_T_nj35 = new TH1D("MET_no1b_T_nj35", ";MET", 400,0,2000);
  h_nb_no1b_T_nj35 = new TH1D("nb_no1b_T_nj35", ";# of b jet", 10,0,10);
#endif
  h_ht_AK4_noMT_T_nj35 = new TH1D("ht_AK4_noMT_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_T_nj35 = new TH1D("jet1_pt_noMT_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_T_nj35 = new TH1D("MR_noMT_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_T_nj35 = new TH1D("R2_noMT_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_T_nj35 = new TH1D("MET_noMT_T_nj35", ";MET", 400,0,2000);
  h_MT_noMT_T_nj35 = new TH1D("MT_noMT_T_nj35", ";MT", 25,0,500);

  h_ht_AK4_nomDPhi_T_nj35 = new TH1D("ht_AK4_nomDPhi_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_T_nj35 = new TH1D("jet1_pt_nomDPhi_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_T_nj35 = new TH1D("MR_nomDPhi_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_T_nj35 = new TH1D("R2_nomDPhi_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_T_nj35 = new TH1D("MET_nomDPhi_T_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_T_nj35 = new TH1D("mDPhi_nomDPhi_T_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_T_nj35 = new TH1D("ht_AK4_no1Lep_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_T_nj35 = new TH1D("jet1_pt_no1Lep_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_T_nj35 = new TH1D("MR_no1Lep_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_T_nj35 = new TH1D("R2_no1Lep_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_T_nj35 = new TH1D("MET_no1Lep_T_nj35", ";MET", 400,0,2000);
  h_nLep_no1Lep_T_nj35 = new TH1D("nLep_no1Lep_T_nj35", ";# of Lepton", 10,0,10);

  h_ht_AK4_T_nj6 = new TH1D("ht_AK4_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_T_nj6 = new TH1D("jet1_pt_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_T_nj6 = new TH1D("MR_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_T_nj6 = new TH1D("R2_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_T_nj6 = new TH1D("tau21_T_nj6", ";tau21", 200,0,1);
  h_tau32_T_nj6 = new TH1D("tau32_T_nj6", ";tau32", 200,0,1);
  h_MET_T_nj6 = new TH1D("MET_T_nj6", ";MET", 400,0,2000);

#if TOP == 0
  h_ht_AK4_no1b_T_nj6 = new TH1D("ht_AK4_no1b_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1b_T_nj6 = new TH1D("jet1_pt_no1b_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1b_T_nj6 = new TH1D("MR_no1b_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1b_T_nj6 = new TH1D("R2_no1b_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1b_T_nj6 = new TH1D("MET_no1b_T_nj6", ";MET", 400,0,2000);
  h_nb_no1b_T_nj6 = new TH1D("nb_no1b_T_nj6", ";# of b jet", 10,0,10);
#endif
  h_ht_AK4_noMT_T_nj6 = new TH1D("ht_AK4_noMT_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_T_nj6 = new TH1D("jet1_pt_noMT_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_T_nj6 = new TH1D("MR_noMT_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_T_nj6 = new TH1D("R2_noMT_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_T_nj6 = new TH1D("MET_noMT_T_nj6", ";MET", 400,0,2000);
  h_MT_noMT_T_nj6 = new TH1D("MT_noMT_T_nj6", ";MT", 25,0,500);

  h_ht_AK4_nomDPhi_T_nj6 = new TH1D("ht_AK4_nomDPhi_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_T_nj6 = new TH1D("jet1_pt_nomDPhi_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_T_nj6 = new TH1D("MR_nomDPhi_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_T_nj6 = new TH1D("R2_nomDPhi_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_T_nj6 = new TH1D("MET_nomDPhi_T_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_T_nj6 = new TH1D("mDPhi_nomDPhi_T_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_T_nj6 = new TH1D("ht_AK4_no1Lep_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_T_nj6 = new TH1D("jet1_pt_no1Lep_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_T_nj6 = new TH1D("MR_no1Lep_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_T_nj6 = new TH1D("R2_no1Lep_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_T_nj6 = new TH1D("MET_no1Lep_T_nj6", ";MET", 400,0,2000);
  h_nLep_no1Lep_T_nj6 = new TH1D("nLep_no1Lep_T_nj6", ";# of Lepton", 10,0,10);

#if TOP == 0
  h_ht_AK4_nonW_T = new TH1D("ht_AK4_nonW_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_T = new TH1D("jet1_pt_nonW_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_T = new TH1D("MR_nonW_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_T = new TH1D("R2_nonW_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_T = new TH1D("MET_nonW_T", ";MET", 400,0,2000);
  h_nW_nonW_T = new TH1D("nW_nonW_T", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_T_nj35 = new TH1D("ht_AK4_nonW_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_T_nj35 = new TH1D("jet1_pt_nonW_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_T_nj35 = new TH1D("MR_nonW_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_T_nj35 = new TH1D("R2_nonW_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_T_nj35 = new TH1D("MET_nonW_T_nj35", ";MET", 400,0,2000);
  h_nW_nonW_T_nj35 = new TH1D("nW_nonW_T_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_T_nj6 = new TH1D("ht_AK4_nonW_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_T_nj6 = new TH1D("jet1_pt_nonW_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_T_nj6 = new TH1D("MR_nonW_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_T_nj6 = new TH1D("R2_nonW_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_T_nj6 = new TH1D("MET_nonW_T_nj6", ";MET", 400,0,2000);
  h_nW_nonW_T_nj6 = new TH1D("nW_nonW_T_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_T = new TH1D("ht_AK4_nonTop_T",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_T = new TH1D("jet1_pt_nonTop_T",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_T = new TH1D("MR_nonTop_T",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_T = new TH1D("R2_nonTop_T",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_T = new TH1D("MET_nonTop_T", ";MET", 400,0,2000);
  h_nTop_nonTop_T = new TH1D("nTop_nonTop_T", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_T_nj35 = new TH1D("ht_AK4_nonTop_T_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_T_nj35 = new TH1D("jet1_pt_nonTop_T_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_T_nj35 = new TH1D("MR_nonTop_T_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_T_nj35 = new TH1D("R2_nonTop_T_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_T_nj35 = new TH1D("MET_nonTop_T_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_T_nj35 = new TH1D("nTop_nonTop_T_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_T_nj6 = new TH1D("ht_AK4_nonTop_T_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_T_nj6 = new TH1D("jet1_pt_nonTop_T_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_T_nj6 = new TH1D("MR_nonTop_T_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_T_nj6 = new TH1D("R2_nonTop_T_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_T_nj6 = new TH1D("MET_nonTop_T_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_T_nj6 = new TH1D("nTop_nonTop_T_nj6", ";# of Topjet", 10,0,10);
#endif

  h_ht_AK4_W = new TH1D("ht_AK4_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_W = new TH1D("jet1_pt_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_W = new TH1D("MR_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_W = new TH1D("R2_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_W = new TH1D("tau21_W", ";tau21", 200,0,1);
  h_tau32_W = new TH1D("tau32_W", ";tau32", 200,0,1);
  h_MET_W = new TH1D("MET_W", ";MET", 400,0,2000);
  h_AK8_jet1_pt_W = new TH1D("AK8_jet1_pt_W",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_W = new TH1D("AK8_jet1_eta_W",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_W = new TH1D("MET_phi_W", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_W_nj35 = new TH1D("AK8_jet1_pt_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_W_nj35 = new TH1D("AK8_jet1_eta_W_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_W_nj35 = new TH1D("MET_phi_W_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_W_nj6 = new TH1D("AK8_jet1_pt_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_W_nj6 = new TH1D("AK8_jet1_eta_W_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_W_nj6 = new TH1D("MET_phi_W_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_noMT_W = new TH1D("ht_AK4_noMT_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_W = new TH1D("jet1_pt_noMT_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_W = new TH1D("MR_noMT_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_W = new TH1D("R2_noMT_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_W = new TH1D("MET_noMT_W", ";MET", 400,0,2000);
  h_MT_noMT_W = new TH1D("MT_noMT_W", ";MET", 25,0,500);

  h_ht_AK4_nonb_W = new TH1D("ht_AK4_nonb_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_W = new TH1D("jet1_pt_nonb_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_W = new TH1D("MR_nonb_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_W = new TH1D("R2_nonb_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_W = new TH1D("MET_nonb_W", ";MET", 400,0,2000);
  h_nb_nonb_W = new TH1D("nb_nonb_W", ";# of bjet", 10,0,10);

  h_ht_AK4_nomDPhi_W = new TH1D("ht_AK4_nomDPhi_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_W = new TH1D("jet1_pt_nomDPhi_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_W = new TH1D("MR_nomDPhi_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_W = new TH1D("R2_nomDPhi_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_W = new TH1D("MET_nomDPhi_W", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_W = new TH1D("mDPhi_nomDPhi_W", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_W = new TH1D("ht_AK4_no1Lep_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_W = new TH1D("jet1_pt_no1Lep_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_W = new TH1D("MR_no1Lep_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_W = new TH1D("R2_no1Lep_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_W = new TH1D("MET_no1Lep_W", ";MET", 400,0,2000);
  h_nLep_no1Lep_W = new TH1D("nLep_no1Lep_W", ";# of Lepton", 10,0,10);

  h_ht_AK4_W_nj35 = new TH1D("ht_AK4_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_W_nj35 = new TH1D("jet1_pt_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_W_nj35 = new TH1D("MR_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_W_nj35 = new TH1D("R2_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_W_nj35 = new TH1D("tau21_W_nj35", ";tau21", 200,0,1);
  h_tau32_W_nj35 = new TH1D("tau32_W_nj35", ";tau32", 200,0,1);
  h_MET_W_nj35 = new TH1D("MET_W_nj35", ";MET", 400,0,2000);

  h_ht_AK4_noMT_W_nj35 = new TH1D("ht_AK4_noMT_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_W_nj35 = new TH1D("jet1_pt_noMT_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_W_nj35 = new TH1D("MR_noMT_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_W_nj35 = new TH1D("R2_noMT_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_W_nj35 = new TH1D("MET_noMT_W_nj35", ";MET", 400,0,2000);
  h_MT_noMT_W_nj35 = new TH1D("MT_noMT_W_nj35", ";MET", 25,0,500);

  h_ht_AK4_nonb_W_nj35 = new TH1D("ht_AK4_nonb_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_W_nj35 = new TH1D("jet1_pt_nonb_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_W_nj35 = new TH1D("MR_nonb_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_W_nj35 = new TH1D("R2_nonb_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_W_nj35 = new TH1D("MET_nonb_W_nj35", ";MET", 400,0,2000);
  h_nb_nonb_W_nj35 = new TH1D("nb_nonb_W_nj35", ";# of bjet", 10,0,10);

  h_ht_AK4_nomDPhi_W_nj35 = new TH1D("ht_AK4_nomDPhi_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_W_nj35 = new TH1D("jet1_pt_nomDPhi_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_W_nj35 = new TH1D("MR_nomDPhi_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_W_nj35 = new TH1D("R2_nomDPhi_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_W_nj35 = new TH1D("MET_nomDPhi_W_nj35", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_W_nj35 = new TH1D("mDPhi_nomDPhi_W_nj35", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_W_nj35 = new TH1D("ht_AK4_no1Lep_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_W_nj35 = new TH1D("jet1_pt_no1Lep_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_W_nj35 = new TH1D("MR_no1Lep_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_W_nj35 = new TH1D("R2_no1Lep_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_W_nj35 = new TH1D("MET_no1Lep_W_nj35", ";MET", 400,0,2000);
  h_nLep_no1Lep_W_nj35 = new TH1D("nLep_no1Lep_W_nj35", ";# of Lepton", 10,0,10);

  h_ht_AK4_W_nj6 = new TH1D("ht_AK4_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_W_nj6 = new TH1D("jet1_pt_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_W_nj6 = new TH1D("MR_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_W_nj6 = new TH1D("R2_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_W_nj6 = new TH1D("tau21_W_nj6", ";tau21", 200,0,1);
  h_tau32_W_nj6 = new TH1D("tau32_W_nj6", ";tau32", 200,0,1);
  h_MET_W_nj6 = new TH1D("MET_W_nj6", ";MET", 400,0,2000);

  h_ht_AK4_noMT_W_nj6 = new TH1D("ht_AK4_noMT_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_noMT_W_nj6 = new TH1D("jet1_pt_noMT_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_noMT_W_nj6 = new TH1D("MR_noMT_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_noMT_W_nj6 = new TH1D("R2_noMT_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_noMT_W_nj6 = new TH1D("MET_noMT_W_nj6", ";MET", 400,0,2000);
  h_MT_noMT_W_nj6 = new TH1D("MT_noMT_W_nj6", ";MET", 25,0,500);

  h_ht_AK4_nonb_W_nj6 = new TH1D("ht_AK4_nonb_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonb_W_nj6 = new TH1D("jet1_pt_nonb_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonb_W_nj6 = new TH1D("MR_nonb_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonb_W_nj6 = new TH1D("R2_nonb_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonb_W_nj6 = new TH1D("MET_nonb_W_nj6", ";MET", 400,0,2000);
  h_nb_nonb_W_nj6 = new TH1D("nb_nonb_W_nj6", ";# of bjet", 10,0,10);

  h_ht_AK4_nomDPhi_W_nj6 = new TH1D("ht_AK4_nomDPhi_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nomDPhi_W_nj6 = new TH1D("jet1_pt_nomDPhi_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nomDPhi_W_nj6 = new TH1D("MR_nomDPhi_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nomDPhi_W_nj6 = new TH1D("R2_nomDPhi_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nomDPhi_W_nj6 = new TH1D("MET_nomDPhi_W_nj6", ";MET", 400,0,2000);
  h_mDPhi_nomDPhi_W_nj6 = new TH1D("mDPhi_nomDPhi_W_nj6_nj6", ";#Delta#phi_{megajets}", 64,0,3.2);

  h_ht_AK4_no1Lep_W_nj6 = new TH1D("ht_AK4_no1Lep_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_no1Lep_W_nj6 = new TH1D("jet1_pt_no1Lep_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_no1Lep_W_nj6 = new TH1D("MR_no1Lep_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_no1Lep_W_nj6 = new TH1D("R2_no1Lep_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_no1Lep_W_nj6 = new TH1D("MET_no1Lep_W_nj6", ";MET", 400,0,2000);
  h_nLep_no1Lep_W_nj6 = new TH1D("nLep_no1Lep_W_nj6", ";# of Lepton", 10,0,10);

#if TOP == 0
  h_ht_AK4_nonW_W = new TH1D("ht_AK4_nonW_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_W = new TH1D("jet1_pt_nonW_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_W = new TH1D("MR_nonW_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_W = new TH1D("R2_nonW_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_W = new TH1D("MET_nonW_W", ";MET", 400,0,2000);
  h_nW_nonW_W = new TH1D("nW_nonW_W", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_W_nj35 = new TH1D("ht_AK4_nonW_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_W_nj35 = new TH1D("jet1_pt_nonW_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_W_nj35 = new TH1D("MR_nonW_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_W_nj35 = new TH1D("R2_nonW_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_W_nj35 = new TH1D("MET_nonW_W_nj35", ";MET", 400,0,2000);
  h_nW_nonW_W_nj35 = new TH1D("nW_nonW_W_nj35", ";# of Wjet", 10,0,10);

  h_ht_AK4_nonW_W_nj6 = new TH1D("ht_AK4_nonW_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonW_W_nj6 = new TH1D("jet1_pt_nonW_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonW_W_nj6 = new TH1D("MR_nonW_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonW_W_nj6 = new TH1D("R2_nonW_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonW_W_nj6 = new TH1D("MET_nonW_W_nj6", ";MET", 400,0,2000);
  h_nW_nonW_W_nj6 = new TH1D("nW_nonW_W_nj6", ";# of Wjet", 10,0,10);
#else
  h_ht_AK4_nonTop_W = new TH1D("ht_AK4_nonTop_W",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_W = new TH1D("jet1_pt_nonTop_W",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_W = new TH1D("MR_nonTop_W",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_W = new TH1D("R2_nonTop_W",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_W = new TH1D("MET_nonTop_W", ";MET", 400,0,2000);
  h_nTop_nonTop_W = new TH1D("nTop_nonTop_W", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_W_nj35 = new TH1D("ht_AK4_nonTop_W_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_W_nj35 = new TH1D("jet1_pt_nonTop_W_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_W_nj35 = new TH1D("MR_nonTop_W_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_W_nj35 = new TH1D("R2_nonTop_W_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_W_nj35 = new TH1D("MET_nonTop_W_nj35", ";MET", 400,0,2000);
  h_nTop_nonTop_W_nj35 = new TH1D("nTop_nonTop_W_nj35", ";# of Topjet", 10,0,10);

  h_ht_AK4_nonTop_W_nj6 = new TH1D("ht_AK4_nonTop_W_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_nonTop_W_nj6 = new TH1D("jet1_pt_nonTop_W_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_nonTop_W_nj6 = new TH1D("MR_nonTop_W_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_nonTop_W_nj6 = new TH1D("R2_nonTop_W_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_MET_nonTop_W_nj6 = new TH1D("MET_nonTop_W_nj6", ";MET", 400,0,2000);
  h_nTop_nonTop_W_nj6 = new TH1D("nTop_nonTop_W_nj6", ";# of Topjet", 10,0,10);
#endif

  h_ht_AK4_q = new TH1D("ht_AK4_q",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_q = new TH1D("jet1_pt_q",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_q = new TH1D("MR_q",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_q = new TH1D("R2_q",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_q = new TH1D("tau21_q", ";tau21", 200,0,1);
  h_tau32_q = new TH1D("tau32_q", ";tau32", 200,0,1);
  h_MET_q = new TH1D("MET_q", ";MET", 400,0,2000);
  h_AK8_jet1_pt_q = new TH1D("AK8_jet1_pt_q",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_q = new TH1D("AK8_jet1_eta_q",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_q = new TH1D("MET_phi_q", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_q_nj35 = new TH1D("AK8_jet1_pt_q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_q_nj35 = new TH1D("AK8_jet1_eta_q_nj35",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_q_nj35 = new TH1D("MET_phi_q_nj35", ";#phi_{MET}", 640, -3.2,3.2);
  h_AK8_jet1_pt_q_nj6 = new TH1D("AK8_jet1_pt_q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_AK8_jet1_eta_q_nj6 = new TH1D("AK8_jet1_eta_q_nj6",      ";#eta_{jet1}",            640, -3.2,3.2);
  h_MET_phi_q_nj6 = new TH1D("MET_phi_q_nj6", ";#phi_{MET}", 640, -3.2,3.2);

  h_ht_AK4_q_nj35 = new TH1D("ht_AK4_q_nj35",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_q_nj35 = new TH1D("jet1_pt_q_nj35",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_q_nj35 = new TH1D("MR_q_nj35",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_q_nj35 = new TH1D("R2_q_nj35",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_q_nj35 = new TH1D("tau21_q_nj35", ";tau21", 200,0,1);
  h_tau32_q_nj35 = new TH1D("tau32_q_nj35", ";tau32", 200,0,1);
  h_MET_q_nj35 = new TH1D("MET_q_nj35", ";MET", 400,0,2000);

  h_ht_AK4_q_nj6 = new TH1D("ht_AK4_q_nj6",  ";H_{T}",                  nbn_HT, bn_HT);
  h_jet1_pt_q_nj6 = new TH1D("jet1_pt_q_nj6",      ";p_{T, jet1}",            200, 0,2000);
  h_MR_q_nj6 = new TH1D("MR_q_nj6",   ";MR_{AK4}",         nbn_MR,bn_MR);
  h_R2_q_nj6 = new TH1D("R2_q_nj6",   ";R^{2}_{AK4}",         nbn_R2,bn_R2);
  h_tau21_q_nj6 = new TH1D("tau21_q_nj6", ";tau21", 200,0,1);
  h_tau32_q_nj6 = new TH1D("tau32_q_nj6", ";tau32", 200,0,1);
  h_MET_q_nj6 = new TH1D("MET_q_nj6", ";MET", 400,0,2000);

  h_softDropMass = new TH1D("softDropMass", "", 100,0,500);
  h_R2_MR = new TH2D("R2_MR", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);

  h_R2_MR_s = new TH2D("R2_MR_s", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_s = new TH2D("R2_MET_s", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_s = new TH2D("MR_MET_s", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_s = new TH2D("AK8Jet1pT_MET_s", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_q = new TH2D("R2_MR_q", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_q = new TH2D("R2_MET_q", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_q = new TH2D("MR_MET_q", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_q = new TH2D("AK8Jet1pT_MET_q", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_S = new TH2D("R2_MR_S", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_S = new TH2D("R2_MET_S", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_S = new TH2D("MR_MET_S", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_HT_MET_S = new TH2D("HT_MET_S", ";MET_{AK4};HT_{AK4}",40,0,2000,40,0,2000);
  h_R2_HT_S = new TH2D("R2_HT_S", ";HT_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_HT_S = new TH2D("MR_HT_S", ";HT_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_S = new TH2D("AK8Jet1pT_MET_S", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MET_S_nj35 = new TH2D("R2_MET_S_nj35", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_S_nj35 = new TH2D("MR_MET_S_nj35", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_HT_MET_S_nj35 = new TH2D("HT_MET_S_nj35", ";MET_{AK4};HT_{AK4}",40,0,2000,40,0,2000);
  h_R2_HT_S_nj35 = new TH2D("R2_HT_S_nj35", ";HT_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_HT_S_nj35 = new TH2D("MR_HT_S_nj35", ";HT_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_S_nj35 = new TH2D("AK8Jet1pT_MET_S_nj35", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MET_S_nj6 = new TH2D("R2_MET_S_nj6", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_S_nj6 = new TH2D("MR_MET_S_nj6", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_HT_MET_S_nj6 = new TH2D("HT_MET_S_nj6", ";MET_{AK4};HT_{AK4}",40,0,2000,40,0,2000);
  h_R2_HT_S_nj6 = new TH2D("R2_HT_S_nj6", ";HT_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_HT_S_nj6 = new TH2D("MR_HT_S_nj6", ";HT_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_S_nj6 = new TH2D("AK8Jet1pT_MET_S_nj6", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_Q = new TH2D("R2_MR_Q", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_Q = new TH2D("R2_MET_Q", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_Q = new TH2D("MR_MET_Q", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_Q = new TH2D("AK8Jet1pT_MET_Q", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_T = new TH2D("R2_MR_T", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_T = new TH2D("R2_MET_T", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_T = new TH2D("MR_MET_T", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_T = new TH2D("AK8Jet1pT_MET_T", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_W = new TH2D("R2_MR_W", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_W = new TH2D("R2_MET_W", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_W = new TH2D("MR_MET_W", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_W = new TH2D("AK8Jet1pT_MET_W", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_Z = new TH2D("R2_MR_Z", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_Z = new TH2D("R2_MET_Z", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_Z = new TH2D("MR_MET_Z", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_Z = new TH2D("AK8Jet1pT_MET_Z", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_L = new TH2D("R2_MR_L", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_L = new TH2D("R2_MET_L", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_L = new TH2D("MR_MET_L", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_L = new TH2D("AK8Jet1pT_MET_L", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_G = new TH2D("R2_MR_G", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MET_G = new TH2D("R2_MET_G", ";MET_{AK4};R^{2}_{AK4}",40,0,2000,nbn_R2,bn_R2);
  h_MR_MET_G = new TH2D("MR_MET_G", ";MET_{AK4};MR_{AK4}",40,0,2000,nbn_MR,bn_MR);
  h_AK8Jet1pT_MET_G = new TH2D("AK8Jet1pT_MET_G", ";MET_{AK4};p_{T,AK8Jet1}",40,0,2000,40,0,2000);
  h_R2_MR_s_nj35 = new TH2D("R2_MR_s_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_s_nj6 = new TH2D("R2_MR_s_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_q_nj35 = new TH2D("R2_MR_q_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_q_nj6 = new TH2D("R2_MR_q_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_S_nj35 = new TH2D("R2_MR_S_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_S_nj6 = new TH2D("R2_MR_S_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_Q_nj35 = new TH2D("R2_MR_Q_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_Q_nj6 = new TH2D("R2_MR_Q_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_T_nj35 = new TH2D("R2_MR_T_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_T_nj6 = new TH2D("R2_MR_T_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_W_nj35 = new TH2D("R2_MR_W_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_W_nj6 = new TH2D("R2_MR_W_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_Z_nj35 = new TH2D("R2_MR_Z_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_Z_nj6 = new TH2D("R2_MR_Z_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_L_nj35 = new TH2D("R2_MR_L_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_L_nj6 = new TH2D("R2_MR_L_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_G_nj35 = new TH2D("R2_MR_G_nj35", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_G_nj6 = new TH2D("R2_MR_G_nj6", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  
  for (unsigned int i=0; i<=syst_nSyst; ++i) {
    std::stringstream histoname, title;
    title<<"Systematic variation #="<<i;

    histoname<<"R2_MR_S_nj35_syst"<<i;
    vh_R2_MR_S_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_s_nj35_syst"<<i;
    vh_R2_MR_s_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_Q_nj35_syst"<<i;
    vh_R2_MR_Q_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_q_nj35_syst"<<i;
    vh_R2_MR_q_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_T_nj35_syst"<<i;
    vh_R2_MR_T_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_W_nj35_syst"<<i;
    vh_R2_MR_W_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_Z_nj35_syst"<<i;
    vh_R2_MR_Z_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_L_nj35_syst"<<i;
    vh_R2_MR_L_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_G_nj35_syst"<<i;
    vh_R2_MR_G_nj35.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_S_nj6_syst"<<i;
    vh_R2_MR_S_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_s_nj6_syst"<<i;
    vh_R2_MR_s_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_Q_nj6_syst"<<i;
    vh_R2_MR_Q_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_q_nj6_syst"<<i;
    vh_R2_MR_q_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname<<"R2_MR_T_nj6_syst"<<i;
    vh_R2_MR_T_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_W_nj6_syst"<<i;
    vh_R2_MR_W_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_Z_nj6_syst"<<i;
    vh_R2_MR_Z_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_L_nj6_syst"<<i;
    vh_R2_MR_L_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));
    histoname.str("");
    histoname<<"R2_MR_G_nj6_syst"<<i;
    vh_R2_MR_G_nj6.push_back(new TH2D(histoname.str().c_str(), (title.str()+";MR_{AK4};R^{2}_{AK4}").c_str(),nbn_MR,bn_MR,nbn_R2, bn_R2));

  }

  h_HT_TrigMass_0 = new TH1D("h_HT_TrigMass_0", ";H_{T} [GeV]", nbn_HT, bn_HT);
  h_HT_TrigMass_1 = new TH1D("h_HT_TrigMass_1", ";H_{T} [GeV]", nbn_HT, bn_HT);

  h_AK8HT_TrigMass_0 = new TH1D("h_AK8HT_TrigMass_0", ";H_{T} [GeV]", nbn_HT, bn_HT);
  h_AK8HT_TrigMass_1 = new TH1D("h_AK8HT_TrigMass_1", ";H_{T} [GeV]", nbn_HT, bn_HT);

  h_AK8JetpT_TrigMass_0 = new TH1D("h_AK8JetpT_TrigMass_0", ";p_{T} [GeV]", nbn_AK8J1pt, bn_AK8J1pt);
  h_AK8JetpT_TrigMass_1 = new TH1D("h_AK8JetpT_TrigMass_1", ";p_{T} [GeV]", nbn_AK8J1pt, bn_AK8J1pt);

  h_AK8JetMass_TrigMass_0 = new TH1D("h_AK8JetMass_TrigMass_0", ";mass_{Jet}", 6, 60, 300);
  h_AK8JetMass_TrigMass_1 = new TH1D("h_AK8JetMass_TrigMass_1", ";mass_{Jet}", 6, 60, 300);

  h_MET_TrigMass_0 = new TH1D("h_MET_TrigMass_0", ";MET", nbn_MET, bn_MET);
  h_MET_TrigMass_1 = new TH1D("h_MET_TrigMass_1", ";MET", nbn_MET, bn_MET);

  h_MR_TrigMass_0 = new TH1D("h_MR_TrigMass_0", ";MR", nbn_MR, bn_MR);
  h_MR_TrigMass_1 = new TH1D("h_MR_TrigMass_1", ";MR", nbn_MR, bn_MR);

  h_R2_TrigMass_0 = new TH1D("h_R2_TrigMass_0", ";R2", 20, 0., 2.);
  h_R2_TrigMass_1 = new TH1D("h_R2_TrigMass_1", ";R2", 20, 0., 2.);

  h_R2_MR_TrigMass_0 = new TH2D("h_R2_MR_TrigMass_0", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_TrigMass_1 = new TH2D("h_R2_MR_TrigMass_1", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);

  h_HT_MET_TrigMass_0 = new TH2D("h_HT_MET_TrigMass_0", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);
  h_HT_MET_TrigMass_1 = new TH2D("h_HT_MET_TrigMass_1", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);

  h_HT_AK8JetMass_TrigMass_0 = new TH2D("h_HT_AK8JetMass_TrigMass_0", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);
  h_HT_AK8JetMass_TrigMass_1 = new TH2D("h_HT_AK8JetMass_TrigMass_1", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);

  h_MET_AK8JetMass_TrigMass_0 = new TH2D("h_MET_AK8JetMass_TrigMass_0", ";Mass_{AK8 leading Jet};MET",nbnjmass,bnjmass,nbnMET,bnMET);
  h_MET_AK8JetMass_TrigMass_1 = new TH2D("h_MET_AK8JetMass_TrigMass_1", ";Mass_{AK8 leading Jet};MET",nbnjmass,bnjmass,nbnMET,bnMET);

  h_AK8HT_MET_TrigMass_0 = new TH2D("h_AK8HT_MET_TrigMass_0", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);
  h_AK8HT_MET_TrigMass_1 = new TH2D("h_AK8HT_MET_TrigMass_1", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);

  h_AK8HT_AK8JetMass_TrigMass_0 = new TH2D("h_AK8HT_AK8JetMass_TrigMass_0", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);
  h_AK8HT_AK8JetMass_TrigMass_1 = new TH2D("h_AK8HT_AK8JetMass_TrigMass_1", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);

  h_HT_MET_AK8JetMass_TrigMass_0 = new TH3D("h_HT_MET_AK8JetMass_TrigMass_0", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);
  h_HT_MET_AK8JetMass_TrigMass_1 = new TH3D("h_HT_MET_AK8JetMass_TrigMass_1", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);

  h_AK8HT_MET_AK8JetMass_TrigMass_0 = new TH3D("h_AK8HT_MET_AK8JetMass_TrigMass_0", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);
  h_AK8HT_MET_AK8JetMass_TrigMass_1 = new TH3D("h_AK8HT_MET_AK8JetMass_TrigMass_1", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);

  h_HT_TrigNoMass_0 = new TH1D("h_HT_TrigNoMass_0", ";H_{T} [GeV]", nbn_HT, bn_HT);
  h_HT_TrigNoMass_1 = new TH1D("h_HT_TrigNoMass_1", ";H_{T} [GeV]", nbn_HT, bn_HT);

  h_AK8HT_TrigNoMass_0 = new TH1D("h_AK8HT_TrigNoMass_0", ";H_{T} [GeV]", nbn_HT, bn_HT);
  h_AK8HT_TrigNoMass_1 = new TH1D("h_AK8HT_TrigNoMass_1", ";H_{T} [GeV]", nbn_HT, bn_HT);

  h_AK8JetpT_TrigNoMass_0 = new TH1D("h_AK8JetpT_TrigNoMass_0", ";p_{T} [GeV]", nbn_AK8J1pt, bn_AK8J1pt);
  h_AK8JetpT_TrigNoMass_1 = new TH1D("h_AK8JetpT_TrigNoMass_1", ";p_{T} [GeV]", nbn_AK8J1pt, bn_AK8J1pt);

  h_AK8JetMass_TrigNoMass_0 = new TH1D("h_AK8JetMass_TrigNoMass_0", ";mass_{Jet}", 6, 60, 300);
  h_AK8JetMass_TrigNoMass_1 = new TH1D("h_AK8JetMass_TrigNoMass_1", ";mass_{Jet}", 6, 60, 300);

  h_MET_TrigNoMass_0 = new TH1D("h_MET_TrigNoMass_0", ";MET", nbn_MET, bn_MET);
  h_MET_TrigNoMass_1 = new TH1D("h_MET_TrigNoMass_1", ";MET", nbn_MET, bn_MET);

  h_MR_TrigNoMass_0 = new TH1D("h_MR_TrigNoMass_0", ";MR", nbn_MR, bn_MR);
  h_MR_TrigNoMass_1 = new TH1D("h_MR_TrigNoMass_1", ";MR", nbn_MR, bn_MR);

  h_R2_TrigNoMass_0 = new TH1D("h_R2_TrigNoMass_0", ";R2", 20, 0., 2.);
  h_R2_TrigNoMass_1 = new TH1D("h_R2_TrigNoMass_1", ";R2", 20, 0., 2.);

  h_R2_MR_TrigNoMass_0 = new TH2D("h_R2_MR_TrigNoMass_0", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);
  h_R2_MR_TrigNoMass_1 = new TH2D("h_R2_MR_TrigNoMass_1", ";MR_{AK4};R^{2}_{AK4}",nbn_MR,bn_MR,nbn_R2,bn_R2);

  h_HT_MET_TrigNoMass_0 = new TH2D("h_HT_MET_TrigNoMass_0", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);
  h_HT_MET_TrigNoMass_1 = new TH2D("h_HT_MET_TrigNoMass_1", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);

  h_HT_AK8JetMass_TrigNoMass_0 = new TH2D("h_HT_AK8JetMass_TrigNoMass_0", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);
  h_HT_AK8JetMass_TrigNoMass_1 = new TH2D("h_HT_AK8JetMass_TrigNoMass_1", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);

  h_MET_AK8JetMass_TrigNoMass_0 = new TH2D("h_MET_AK8JetMass_TrigNoMass_0", ";Mass_{AK8 leading Jet};MET",nbnjmass,bnjmass,nbnMET,bnMET);
  h_MET_AK8JetMass_TrigNoMass_1 = new TH2D("h_MET_AK8JetMass_TrigNoMass_1", ";Mass_{AK8 leading Jet};MET",nbnjmass,bnjmass,nbnMET,bnMET);

  h_AK8HT_MET_TrigNoMass_0 = new TH2D("h_AK8HT_MET_TrigNoMass_0", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);
  h_AK8HT_MET_TrigNoMass_1 = new TH2D("h_AK8HT_MET_TrigNoMass_1", ";MET;H_{T}",nbnMET,bnMET,nbnHT,bnHT);

  h_AK8HT_AK8JetMass_TrigNoMass_0 = new TH2D("h_AK8HT_AK8JetMass_TrigNoMass_0", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);
  h_AK8HT_AK8JetMass_TrigNoMass_1 = new TH2D("h_AK8HT_AK8JetMass_TrigNoMass_1", ";Mass_{AK8 leading Jet};H_{T}",nbnjmass,bnjmass,nbnHT,bnHT);

  h_HT_MET_AK8JetMass_TrigNoMass_0 = new TH3D("h_HT_MET_AK8JetMass_TrigNoMass_0", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);
  h_HT_MET_AK8JetMass_TrigNoMass_1 = new TH3D("h_HT_MET_AK8JetMass_TrigNoMass_1", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);

  h_AK8HT_MET_AK8JetMass_TrigNoMass_0 = new TH3D("h_AK8HT_MET_AK8JetMass_TrigNoMass_0", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);
  h_AK8HT_MET_AK8JetMass_TrigNoMass_1 = new TH3D("h_AK8HT_MET_AK8JetMass_TrigNoMass_1", ";Mass_{AK8 leading Jet};MET;H_{T}",nbnjmass,bnjmass,nbn3DMET,bn3DMET,nbn3DHT,bn3DHT);

  h_AK8Jet1Pt_Eta_W_fakerate = new TH2D("AK8Jet1Pt_Eta_W_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_W_fakerate = new TH2D("AK8Jet1Pt_Eta_no_W_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_mW_fakerate = new TH2D("AK8Jet1Pt_Eta_mW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_mW_fakerate = new TH2D("AK8Jet1Pt_Eta_no_mW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_m0bW_fakerate = new TH2D("AK8Jet1Pt_Eta_m0bW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_m0bW_fakerate = new TH2D("AK8Jet1Pt_Eta_no_m0bW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_aW_fakerate = new TH2D("AK8Jet1Pt_Eta_aW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_aW_fakerate = new TH2D("AK8Jet1Pt_Eta_no_aW_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);

  h_AK8Jet1Pt_Eta_Top_fakerate = new TH2D("AK8Jet1Pt_Eta_Top_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_Top_fakerate = new TH2D("AK8Jet1Pt_Eta_no_Top_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_mTop_fakerate = new TH2D("AK8Jet1Pt_Eta_mTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_mTop_fakerate = new TH2D("AK8Jet1Pt_Eta_no_mTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_m0bTop_fakerate = new TH2D("AK8Jet1Pt_Eta_m0bTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_m0bTop_fakerate = new TH2D("AK8Jet1Pt_Eta_no_m0bTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_aTop_fakerate = new TH2D("AK8Jet1Pt_Eta_aTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_aTop_fakerate = new TH2D("AK8Jet1Pt_Eta_no_aTop_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);

  h_AK8Jet1Pt_Z_fakerate = new TH1D("AK8Jet1Pt_Z_fakerate",";p_{T, AK8 jet} [GeV]",nbn_AK8J1pt,bn_AK8J1pt);
  h_AK8Jet1Pt_no_Z_fakerate = new TH1D("AK8Jet1Pt_no_Z_fakerate",";p_{T, AK8 jet} [GeV]",nbn_AK8J1pt,bn_AK8J1pt);
  h_AK8Jet1Pt_Eta_Z_fakerate = new TH2D("AK8Jet1Pt_Eta_Z_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_Z_fakerate = new TH2D("AK8Jet1Pt_Eta_no_Z_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_WtagZ_fakerate = new TH1D("AK8Jet1Pt_WtagZ_fakerate",";p_{T, AK8 jet} [GeV]",nbn_AK8J1pt,bn_AK8J1pt);
  h_AK8Jet1Pt_no_WtagZ_fakerate = new TH1D("AK8Jet1Pt_no_WtagZ_fakerate",";p_{T, AK8 jet} [GeV]",nbn_AK8J1pt,bn_AK8J1pt);
  h_AK8Jet1Pt_Eta_WtagZ_fakerate = new TH2D("AK8Jet1Pt_Eta_WtagZ_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_Eta_no_WtagZ_fakerate = new TH2D("AK8Jet1Pt_Eta_no_WtagZ_fakerate",";p_{T, AK8 jet} [GeV];#eta_{AK8 jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);

  h_AK8Jet1Pt_eta_GenW_no_W = new TH2D("AK8Jet1Pt_eta_GenW_no_W",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_eta_GenTop_no_Top = new TH2D("AK8Jet1Pt_eta_GenTop_no_Top",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_eta_GenW_W = new TH2D("AK8Jet1Pt_eta_GenW_W",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_eta_GenTop_Top = new TH2D("AK8Jet1Pt_eta_GenTop_Top",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_eta_FakeW_W = new TH2D("AK8Jet1Pt_eta_FakeW_W",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);
  h_AK8Jet1Pt_eta_FakeTop_Top = new TH2D("AK8Jet1Pt_eta_FakeTop_Top",";p_{T, AK8 jet} [GeV];#eta_{jet}",nbn_AK8J1pt,bn_AK8J1pt,nbn_eta,bn_eta);

}

//_______________________________________________________
//               Fill Histograms here
void
Analysis::fill_analysis_histos(eventBuffer& data, const unsigned int& syst_index, const double& weight)
{
  double w = weight; // No scale factor applied
  if (MET_phi<-0.6 && MET_phi>-1.8) continue;
  if (syst_index == 0) {
    // syst_index should only be non-0 if settings.varySystematics is true
    // in case of studying systematics, one should fill a different histogram for each syst_index
    // this variable can be used to chose the correct vector element in case there is a vector of histograms
    // It makes sense, to cut on syst_index == 0, for all ordinary plots
    // syst_index == 0 always guarantees, there are no variations in any systematics
    
    // Check what include variables are available in AnalysisBase.h
    // There a good chance a lot of stuff is already calculated!
    // Especially include object selections or variables to cut on in Analysis

    //bool pass = data.HLT_AK8PFJet360_TrimMass30+data.HLT_PFHT800 == 0 ? false : true;
    //bool pass1 = (data.HLT_AK8PFJet450 == 1 || data.HLT_PFHT800 == 1 || data.HLT_PFHT900 == 1);
    //bool pass2 = (data.HLT_AK8PFJet450 == 1 || data.HLT_PFHT800 == 1);
    //bool pass3 = (data.HLT_AK8PFJet450 == 1 || data.HLT_PFHT900 == 1);

    /*
      Weight:
      They now include trigger efficiencies for MC by default
      w is the event weight without any scale factor applied
      Because scale factors are region dependend, then
      in order to apply them, one has to use the sf_weight[region] variable instead
      eg. sf_weight['S']
     */

    // Baseline cuts 
    // Additionally, let's apply the trigger selection
    // since the weight contains the trigger scaling in MC
    // no specific region, so don't apply scale factors
    // Especially for comparison plots with Ufuk/Fatma
    // Alternatively, could apply SF of the Signal regio

    //double w = sf_weight['S']; // Scale factors applied for the Signal region


/*
  float sinangle=0;
	TLorentzVector LepTag;
	TLorentzVector JetTag;
    if(apply_all_cuts('p')){
      if(nEleTest==1)LepTag.SetPtEtaPhiE(data.ele.pt[iLepTest[0]], data.ele.eta[iLepTest[0]], data.ele.phi[iLepTest[0]], data.ele.E[iLepTest[0]]);
      if(nMuTest==1) LepTag.SetPtEtaPhiE(data.mu.pt[iLepTest[0]], data.mu.eta[iLepTest[0]], data.mu.phi[iLepTest[0]], data.mu.E[iLepTest[0]]);
      JetTag.SetPtEtaPhiE(data.jetsAK4.pt[iJetTest[std::distance(dR_test.begin(), std::min_element(dR_test.begin(),dR_test.end()))]], data.jetsAK4.eta[iJetTest[std::distance(dR_test.begin(), std::min_element(dR_test.begin(),dR_test.end()))]], data.jetsAK4.phi[iJetTest[std::distance(dR_test.begin(), std::min_element(dR_test.begin(),dR_test.end()))]], data.jetsAK4.E[iJetTest[std::distance(dR_test.begin(), std::min_element(dR_test.begin(),dR_test.end()))]]);

      TVector3 test1 = LepTag.Vect();
      TVector3 test2 = JetTag.Vect();
      sinangle = sqrt(abs(1-(test1.Dot(test2)*test1.Dot(test2)/test1.Mag2()/test2.Mag2())));

      
      //h_dR_min->Fill(*std::min_element(dR_test.begin(),dR_test.end()), w);
      //h_rel_pT->Fill(sinangle*data.ele.pt[iLepTest[0]], w);
      //h_rel_pT_dR_min	->Fill(*std::min_element(dR_test.begin(),dR_test.end()), sinangle*data.ele.pt[iLepTest[0]], w);
      //std::std::cout << *std::min_element(dR_test.begin(),dR_test.end()) << std::std::endl;
      //std::std::cout << sinangle*data.ele.pt[iLepTest[0]] << std::std::endl;
      if(nGenLep == 1){
        if(selected_genlep[0].DeltaR(LepTag) < 0.4){
          h_dR_min_tag->Fill(*std::min_element(dR_test.begin(),dR_test.end()), w);
          h_rel_pT_tag->Fill(sinangle*data.ele.pt[iLepTest[0]], w);
          h_rel_pT_dR_min_tag->Fill(*std::min_element(dR_test.begin(),dR_test.end()), sinangle*data.ele.pt[iLepTest[0]], w);
        }
        else if(selected_genlep[0].DeltaR(LepTag) >= 0.4){
          h_dR_min_untag->Fill(*std::min_element(dR_test.begin(),dR_test.end()), w);
          h_rel_pT_untag->Fill(sinangle*data.ele.pt[iLepTest[0]], w);
          h_rel_pT_dR_min_untag->Fill(*std::min_element(dR_test.begin(),dR_test.end()), sinangle*data.ele.pt[iLepTest[0]], w);
        }
      }
 
      //std::std::cout << dR_2D << std::std::endl;
      //std::std::cout << rel_pT_2D << std::std::endl;
      h_dR_min->Fill(dR_2D, w);
      h_rel_pT->Fill(rel_pT_2D, w);
      h_rel_pT_dR_min	->Fill(dR_2D, rel_pT_2D, w);
      h_MR_S_1Lep->Fill(MR, w);
      h_R2_S_1Lep->Fill(R2, w);
      h_R2_MR_S_1Lep->Fill(MR, R2, w);
    }
    if(apply_all_cuts('D')){
      h_MR_T_1Lep->Fill(MR, w);
      h_R2_T_1Lep->Fill(R2, w);
      h_R2_MR_T_1Lep->Fill(MR, R2, w);
    }
    if(apply_all_cuts('d')){
      h_MR_W_1Lep->Fill(MR, w);
      h_R2_W_1Lep->Fill(R2, w);
      h_R2_MR_W_1Lep->Fill(MR, R2, w);
    }
*/

    if (apply_all_cuts('P')) {
      h_njet    ->Fill(nJet,        w);
      h_nAK8jet ->Fill(nJetAK8,     w);
      h_nbTight ->Fill(nTightBTag, w);
      h_nbMedium->Fill(nMediumBTag, w);
      h_nbLoose ->Fill(nLooseBTag, w);
      h_nw      ->Fill(nTightWTag,  w);
      h_nEleVeto  ->Fill(nEleVeto,	w); //cut based
      h_nEleTight ->Fill(nEleTight,	w); //MVA
      h_nEleMedium->Fill(nEleSelect,w); //MVA
      h_nEleLoose ->Fill(nEleLoose,	w); //MVA
      h_nMuTight ->Fill(nMuTight,	w); //cut
      h_nMuMedium->Fill(nMuSelect,w); //cut
      h_nMuSoft  ->Fill(nMuVeto,	w); //cut
      h_nTauVeto ->Fill(nTauVeto, w);
      h_nIsoTrack->Fill(data.IsoTrack.size(), w);

      h_nPhoMedium->Fill(nPhotonSelect, w);

      h_ht_AK4->Fill(AK4_Ht, w); // Calculated in AnalysisBase.h
      h_R2_MR->Fill(MR, R2, w);
    }

    w = sf_weight['F'];
    if (apply_all_cuts('F')){
      for (size_t i=0; i<data.FatJet.size(); ++i) {
        h_AK8Jet1Pt_Eta_no_W_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_m0bW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_aW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passTightWTag[i]) h_AK8Jet1Pt_Eta_W_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w);
        if(passWMassTag[i])  h_AK8Jet1Pt_Eta_m0bW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passTightWAntiTag[i])h_AK8Jet1Pt_Eta_aW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_Top_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_m0bTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_aTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passHadTopTag[i]) h_AK8Jet1Pt_Eta_Top_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passHadTop0BMassTag[i])h_AK8Jet1Pt_Eta_m0bTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passHadTop0BAntiTag[i])h_AK8Jet1Pt_Eta_aTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 

        h_AK8Jet1Pt_no_Z_fakerate->Fill(data.FatJet[i].pt, w); 
        h_AK8Jet1Pt_Eta_no_Z_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_no_WtagZ_fakerate->Fill(data.FatJet[i].pt, w); 
        h_AK8Jet1Pt_Eta_no_WtagZ_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
		  }
      for (size_t i=0; i<iGenZ.size(); ++i) {
        h_AK8Jet1Pt_Eta_Z_fakerate->Fill(data.FatJet[itGenZ[i]].pt, std::abs(data.FatJet[itGenZ[i]].eta), w);
        h_AK8Jet1Pt_Z_fakerate->Fill(data.FatJet[itGenZ[i]].pt, w);
		  }
      for (size_t i=0; i<iGenWtagZ.size(); ++i) {
        h_AK8Jet1Pt_WtagZ_fakerate->Fill(data.FatJet[itGenWtagZ[i]].pt, w);
        h_AK8Jet1Pt_Eta_WtagZ_fakerate->Fill(data.FatJet[itGenWtagZ[i]].pt, std::abs(data.FatJet[itGenWtagZ[i]].eta), w);
      }
    }

    w = sf_weight['f'];
    if (apply_all_cuts('f')){
      for (size_t i=0; i<data.FatJet.size(); ++i) {
        h_AK8Jet1Pt_Eta_no_mW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passWMassTag[i])  h_AK8Jet1Pt_Eta_mW_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        h_AK8Jet1Pt_Eta_no_mTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
        if(passHadTopMassTag[i])h_AK8Jet1Pt_Eta_mTop_fakerate->Fill(data.FatJet[i].pt, std::abs(data.FatJet[i].eta), w); 
      }
    }
    // W enriched region
    w = sf_weight['W'];
    if (apply_all_cuts('W')) {
      h_ht_AK4_W->Fill(AK4_Ht, w);
      h_jet1_pt_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_W->Fill(MR, w);
      h_R2_W->Fill(R2, w);
      h_tau21_W->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_W->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_W->Fill(data.MET_pt,w);
      h_R2_MR_W->Fill(MR, R2, w);
      h_R2_MET_W->Fill(data.MET_pt, R2, w);
      h_MR_MET_W->Fill(data.MET_pt, MR, w);
      h_AK8Jet1pT_MET_W->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_W->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_W->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_W->Fill(data.MET_phi,w);
      if(nEleVeto==1) h_ele_pt_W->Fill(data.Electron[iEleVeto[0]].pt,w);
      else h_mu_pt_W->Fill(data.Muon[iMuVeto[0]].pt,w);

      if(nJet>=3 && nJet<6){
        h_ht_AK4_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_W_nj35->Fill(MR, w);
        h_R2_W_nj35->Fill(R2, w);
        h_tau21_W_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_W_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_W_nj35->Fill(data.MET_pt,w);
        h_R2_MR_W_nj35->Fill(MR, R2, w);
        h_AK8_jet1_pt_W_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
        h_AK8_jet1_eta_W_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
        h_MET_phi_W_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_W_nj6->Fill(MR, w);
        h_R2_W_nj6->Fill(R2, w);
        h_tau21_W_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_W_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_W_nj6->Fill(data.MET_pt,w);
        h_R2_MR_W_nj6->Fill(MR, R2, w);
        h_AK8_jet1_pt_W_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
        h_AK8_jet1_eta_W_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
        h_MET_phi_W_nj6->Fill(data.MET_phi,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('W', "1mW")) {
      h_ht_AK4_nonW_W->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_W->Fill(MR, w);
      h_R2_nonW_W->Fill(R2, w);
      h_MET_nonW_W->Fill(data.MET_pt,w);
      h_nW_nonW_W->Fill(nWMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_W_nj35->Fill(MR, w);
        h_R2_nonW_W_nj35->Fill(R2, w);
        h_MET_nonW_W_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_W_nj35->Fill(nWMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_W_nj6->Fill(MR, w);
        h_R2_nonW_W_nj6->Fill(R2, w);
        h_MET_nonW_W_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_W_nj6->Fill(nWMassTag,w);
      }
    }
#else
    if (apply_all_cuts_except('W', "1mTop")) {
      h_ht_AK4_nonTop_W->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_W->Fill(MR, w);
      h_R2_nonTop_W->Fill(R2, w);
      h_MET_nonTop_W->Fill(data.MET_pt,w);
      h_nTop_nonTop_W->Fill(nHadTop0BMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_W_nj35->Fill(MR, w);
        h_R2_nonTop_W_nj35->Fill(R2, w);
        h_MET_nonTop_W_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_W_nj35->Fill(nHadTop0BMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_W_nj6->Fill(MR, w);
        h_R2_nonTop_W_nj6->Fill(R2, w);
        h_MET_nonTop_W_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_W_nj6->Fill(nHadTop0BMassTag,w);
      }
    }
#endif
    if (apply_all_cuts_except('W', "0b")) {
      h_ht_AK4_nonb_W->Fill(AK4_Ht, w);
      h_jet1_pt_nonb_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonb_W->Fill(MR, w);
      h_R2_nonb_W->Fill(R2, w);
      h_MET_nonb_W->Fill(data.MET_pt,w);
      h_nb_nonb_W->Fill(nLooseBTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonb_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonb_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonb_W_nj35->Fill(MR, w);
        h_R2_nonb_W_nj35->Fill(R2, w);
        h_MET_nonb_W_nj35->Fill(data.MET_pt,w);
        h_nb_nonb_W_nj35->Fill(nLooseBTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonb_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonb_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonb_W_nj6->Fill(MR, w);
        h_R2_nonb_W_nj6->Fill(R2, w);
        h_MET_nonb_W_nj6->Fill(data.MET_pt,w);
        h_nb_nonb_W_nj6->Fill(nLooseBTag,w);
      }
    }
    if (apply_all_cuts_except('W', "MT")) {
      h_ht_AK4_noMT_W->Fill(AK4_Ht, w);
      h_jet1_pt_noMT_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMT_W->Fill(MR, w);
      h_R2_noMT_W->Fill(R2, w);
      h_MET_noMT_W->Fill(data.MET_pt,w);
      h_MT_noMT_W->Fill(MT_vetolep,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_noMT_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_noMT_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMT_W_nj35->Fill(MR, w);
        h_R2_noMT_W_nj35->Fill(R2, w);
        h_MET_noMT_W_nj35->Fill(data.MET_pt,w);
        h_MT_noMT_W_nj35->Fill(MT_vetolep,w);
      }
      if(nJet>=6){
        h_ht_AK4_noMT_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_noMT_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMT_W_nj6->Fill(MR, w);
        h_R2_noMT_W_nj6->Fill(R2, w);
        h_MET_noMT_W_nj6->Fill(data.MET_pt,w);
        h_MT_noMT_W_nj6->Fill(MT_vetolep,w);
      }
    }
    if (apply_all_cuts_except('W', "mDPhi")) {
      h_ht_AK4_nomDPhi_W->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_W->Fill(MR, w);
      h_R2_nomDPhi_W->Fill(R2, w);
      h_MET_nomDPhi_W->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_W->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nomDPhi_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_W_nj35->Fill(MR, w);
        h_R2_nomDPhi_W_nj35->Fill(R2, w);
        h_MET_nomDPhi_W_nj35->Fill(data.MET_pt,w);
        h_mDPhi_nomDPhi_W_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
        h_ht_AK4_nomDPhi_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_W_nj6->Fill(MR, w);
        h_R2_nomDPhi_W_nj6->Fill(R2, w);
        h_MET_nomDPhi_W_nj6->Fill(data.MET_pt,w);
        h_mDPhi_nomDPhi_W_nj6->Fill(dPhiRazor,w);
      }
    }
    std::vector<std::string> vect = {"1Lep", "MT" };
    if (apply_all_cuts_except('W', vect)) {
      h_ht_AK4_no1Lep_W->Fill(AK4_Ht, w);
      h_jet1_pt_no1Lep_W->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Lep_W->Fill(MR, w);
      h_R2_no1Lep_W->Fill(R2, w);
      h_MET_no1Lep_W->Fill(data.MET_pt,w);
      h_nLep_no1Lep_W->Fill(nLepVeto,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no1Lep_W_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no1Lep_W_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Lep_W_nj35->Fill(MR, w);
        h_R2_no1Lep_W_nj35->Fill(R2, w);
        h_MET_no1Lep_W_nj35->Fill(data.MET_pt,w);
        h_nLep_no1Lep_W_nj35->Fill(nLepVeto,w);
      }
      if(nJet>=6){
        h_ht_AK4_no1Lep_W_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no1Lep_W_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Lep_W_nj6->Fill(MR, w);
        h_R2_no1Lep_W_nj6->Fill(R2, w);
        h_MET_no1Lep_W_nj6->Fill(data.MET_pt,w);
        h_nLep_no1Lep_W_nj6->Fill(nLepVeto,w);
      }
    }

    // top enriched region
    w = sf_weight['T'];
    if (apply_all_cuts('T')) {
      h_ht_AK4_T->Fill(AK4_Ht, w);
      h_jet1_pt_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_T->Fill(MR, w);
      h_R2_T->Fill(R2, w);
      h_tau21_T->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_T->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_T->Fill(data.MET_pt,w);
      h_R2_MR_T->Fill(MR, R2, w);
      h_R2_MET_T->Fill(data.MET_pt, R2, w);
      h_MR_MET_T->Fill(data.MET_pt, MR, w);
      h_AK8Jet1pT_MET_T->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_T->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_T->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_T->Fill(data.MET_phi,w);
      if(nEleVeto==1) h_ele_pt_T->Fill(data.Electron[iEleVeto[0]].pt,w);
      else h_mu_pt_T->Fill(data.Muon[iMuVeto[0]].pt,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_T_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_T_nj35->Fill(MR, w);
      h_R2_T_nj35->Fill(R2, w);
      h_tau21_T_nj35->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_T_nj35->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_T_nj35->Fill(data.MET_pt,w);
      h_R2_MR_T_nj35->Fill(MR, R2, w);
      h_AK8_jet1_pt_T_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_T_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_T_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
      h_ht_AK4_T_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_T_nj6->Fill(MR, w);
      h_R2_T_nj6->Fill(R2, w);
      h_tau21_T_nj6->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_T_nj6->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_T_nj6->Fill(data.MET_pt,w);
      h_R2_MR_T_nj6->Fill(MR, R2, w);
      h_AK8_jet1_pt_T_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_T_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_T_nj6->Fill(data.MET_phi,w);
      }

      //h_HT_j1pt_T->Fill(AK4_Ht,data.FatJet[iJetAK8[0]].pt,w);
    }
    if (apply_all_cuts_except('T', "MT")) {
      h_ht_AK4_noMT_T->Fill(AK4_Ht, w);
      h_jet1_pt_noMT_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMT_T->Fill(MR, w);
      h_R2_noMT_T->Fill(R2, w);
      h_MET_noMT_T->Fill(data.MET_pt,w);
      h_MT_noMT_T->Fill(MT_vetolep,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_noMT_T_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_noMT_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMT_T_nj35->Fill(MR, w);
      h_R2_noMT_T_nj35->Fill(R2, w);
      h_MET_noMT_T_nj35->Fill(data.MET_pt,w);
      h_MT_noMT_T_nj35->Fill(MT_vetolep,w);
      }
      if(nJet>=6){
      h_ht_AK4_noMT_T_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_noMT_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMT_T_nj6->Fill(MR, w);
      h_R2_noMT_T_nj6->Fill(R2, w);
      h_MET_noMT_T_nj6->Fill(data.MET_pt,w);
      h_MT_noMT_T_nj6->Fill(MT_vetolep,w);
      }
    }
    vect = {"1Lep", "MT" };
    if (apply_all_cuts_except('T', vect)) {
      h_ht_AK4_no1Lep_T->Fill(AK4_Ht, w);
      h_jet1_pt_no1Lep_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Lep_T->Fill(MR, w);
      h_R2_no1Lep_T->Fill(R2, w);
      h_MET_no1Lep_T->Fill(data.MET_pt,w);
      h_nLep_no1Lep_T->Fill(nLepVeto,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_no1Lep_T_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_no1Lep_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Lep_T_nj35->Fill(MR, w);
      h_R2_no1Lep_T_nj35->Fill(R2, w);
      h_MET_no1Lep_T_nj35->Fill(data.MET_pt,w);
      h_nLep_no1Lep_T_nj35->Fill(nLepVeto,w);
      }
      if(nJet>=6){
      h_ht_AK4_no1Lep_T_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_no1Lep_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Lep_T_nj6->Fill(MR, w);
      h_R2_no1Lep_T_nj6->Fill(R2, w);
      h_MET_no1Lep_T_nj6->Fill(data.MET_pt,w);
      h_nLep_no1Lep_T_nj6->Fill(nLepVeto,w);
      }
    }
    if (apply_all_cuts_except('T', "mDPhi")) {
      h_ht_AK4_nomDPhi_T->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_T->Fill(MR, w);
      h_R2_nomDPhi_T->Fill(R2, w);
      h_MET_nomDPhi_T->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_T->Fill(dPhiRazor,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_nomDPhi_T_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_T_nj35->Fill(MR, w);
      h_R2_nomDPhi_T_nj35->Fill(R2, w);
      h_MET_nomDPhi_T_nj35->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_T_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
      h_ht_AK4_nomDPhi_T_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_T_nj6->Fill(MR, w);
      h_R2_nomDPhi_T_nj6->Fill(R2, w);
      h_MET_nomDPhi_T_nj6->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_T_nj6->Fill(dPhiRazor,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('T', "1W")) {
      h_ht_AK4_nonW_T->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_T->Fill(MR, w);
      h_R2_nonW_T->Fill(R2, w);
      h_MET_nonW_T->Fill(data.MET_pt,w);
      h_nW_nonW_T->Fill(nTightWTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_T_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_T_nj35->Fill(MR, w);
        h_R2_nonW_T_nj35->Fill(R2, w);
        h_MET_nonW_T_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_T_nj35->Fill(nTightWTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_T_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_T_nj6->Fill(MR, w);
        h_R2_nonW_T_nj6->Fill(R2, w);
        h_MET_nonW_T_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_T_nj6->Fill(nTightWTag,w);
      }
    }
#else
    if (apply_all_cuts_except('T', "1Top")) {
      h_ht_AK4_nonTop_T->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_T->Fill(MR, w);
      h_R2_nonTop_T->Fill(R2, w);
      h_MET_nonTop_T->Fill(data.MET_pt,w);
      h_nTop_nonTop_T->Fill(nHadTopTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_T_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_T_nj35->Fill(MR, w);
        h_R2_nonTop_T_nj35->Fill(R2, w);
        h_MET_nonTop_T_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_T_nj35->Fill(nHadTopTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_T_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_T_nj6->Fill(MR, w);
        h_R2_nonTop_T_nj6->Fill(R2, w);
        h_MET_nonTop_T_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_T_nj6->Fill(nHadTopTag,w);
      }
    }
#endif
#if TOP == 0
    if (apply_all_cuts_except('T', "1b")) {
      h_ht_AK4_no1b_T->Fill(AK4_Ht, w);
      h_jet1_pt_no1b_T->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1b_T->Fill(MR, w);
      h_R2_no1b_T->Fill(R2, w);
      h_MET_no1b_T->Fill(data.MET_pt,w);
      h_nb_no1b_T->Fill(nMediumBTag,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_no1b_T_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_no1b_T_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1b_T_nj35->Fill(MR, w);
      h_R2_no1b_T_nj35->Fill(R2, w);
      h_MET_no1b_T_nj35->Fill(data.MET_pt,w);
      h_nb_no1b_T_nj35->Fill(nMediumBTag,w);
      }
      if(nJet>=6){
      h_ht_AK4_no1b_T_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_no1b_T_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1b_T_nj6->Fill(MR, w);
      h_R2_no1b_T_nj6->Fill(R2, w);
      h_MET_no1b_T_nj6->Fill(data.MET_pt,w);
      h_nb_no1b_T_nj6->Fill(nMediumBTag,w);
      }
    }
#endif
    

    // Z enriched region
    w = sf_weight['Z'];
    if (apply_all_cuts('Z')) {
      h_ht_AK4_Z->Fill(AK4_Ht, w);
      h_jet1_pt_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_Z->Fill(MR, w);
      h_R2_Z->Fill(R2_ll, w);
      h_tau21_Z->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_Z->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_Z->Fill(MET_ll,w);
      h_R2_MR_Z->Fill(MR, R2_ll, w);
      h_R2_MET_Z->Fill(MET_ll, R2_ll, w);
      h_MR_MET_Z->Fill(MET_ll, MR, w);
      h_AK8Jet1pT_MET_Z->Fill(MET_ll, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_Z->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Z->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Z->Fill(data.MET_phi,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_Z_nj35->Fill(MR, w);
        h_R2_Z_nj35->Fill(R2_ll, w);
        h_tau21_Z_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_Z_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_Z_nj35->Fill(MET_ll,w);
        h_R2_MR_Z_nj35->Fill(MR, R2_ll, w);
      h_AK8_jet1_pt_Z_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Z_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Z_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_Z_nj6->Fill(MR, w);
        h_R2_Z_nj6->Fill(R2_ll, w);
        h_tau21_Z_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_Z_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_Z_nj6->Fill(MET_ll,w);
        h_R2_MR_Z_nj6->Fill(MR, R2_ll, w);
      h_AK8_jet1_pt_Z_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Z_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Z_nj6->Fill(data.MET_phi,w);
      }
    }
    if (apply_all_cuts_except('Z', "mDPhi")) {
      h_ht_AK4_nomDPhi_Z->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_Z->Fill(MR, w);
      h_R2_nomDPhi_Z->Fill(R2_ll, w);
      h_MET_nomDPhi_Z->Fill(MET_ll,w);
      h_mDPhi_nomDPhi_Z->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nomDPhi_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_Z_nj35->Fill(MR, w);
        h_R2_nomDPhi_Z_nj35->Fill(R2_ll, w);
        h_MET_nomDPhi_Z_nj35->Fill(MET_ll,w);
        h_mDPhi_nomDPhi_Z_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
        h_ht_AK4_nomDPhi_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_Z_nj6->Fill(MR, w);
        h_R2_nomDPhi_Z_nj6->Fill(R2_ll, w);
        h_MET_nomDPhi_Z_nj6->Fill(MET_ll,w);
        h_mDPhi_nomDPhi_Z_nj6->Fill(dPhiRazor,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('Z', "1mW")) {
      h_ht_AK4_nonW_Z->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_Z->Fill(MR, w);
      h_R2_nonW_Z->Fill(R2, w);
      h_MET_nonW_Z->Fill(data.MET_pt,w);
      h_nW_nonW_Z->Fill(nWMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_Z_nj35->Fill(MR, w);
        h_R2_nonW_Z_nj35->Fill(R2, w);
        h_MET_nonW_Z_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_Z_nj35->Fill(nWMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_Z_nj6->Fill(MR, w);
        h_R2_nonW_Z_nj6->Fill(R2, w);
        h_MET_nonW_Z_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_Z_nj6->Fill(nWMassTag,w);
      }
    }
#else
    if (apply_all_cuts_except('Z', "1mTop")) {
      h_ht_AK4_nonTop_Z->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_Z->Fill(MR, w);
      h_R2_nonTop_Z->Fill(R2, w);
      h_MET_nonTop_Z->Fill(data.MET_pt,w);
      h_nTop_nonTop_Z->Fill(nHadTopMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_Z_nj35->Fill(MR, w);
        h_R2_nonTop_Z_nj35->Fill(R2, w);
        h_MET_nonTop_Z_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_Z_nj35->Fill(nHadTopMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_Z_nj6->Fill(MR, w);
        h_R2_nonTop_Z_nj6->Fill(R2, w);
        h_MET_nonTop_Z_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_Z_nj6->Fill(nHadTopMassTag,w);
      }
    }
#endif
    if (apply_all_cuts_except('Z', "Mll")) {
      h_ht_AK4_noMll_Z->Fill(AK4_Ht, w);
      h_jet1_pt_noMll_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMll_Z->Fill(MR, w);
      h_R2_noMll_Z->Fill(R2_ll, w);
      h_MET_noMll_Z->Fill(MET_ll,w);
      h_Mll_noMll_Z->Fill(M_ll,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_noMll_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_noMll_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMll_Z_nj35->Fill(MR, w);
        h_R2_noMll_Z_nj35->Fill(R2_ll, w);
        h_MET_noMll_Z_nj35->Fill(MET_ll,w);
        h_Mll_noMll_Z->Fill(M_ll,w);
      }
      if(nJet>=6){
        h_ht_AK4_noMll_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_noMll_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMll_Z_nj6->Fill(MR, w);
        h_R2_noMll_Z_nj6->Fill(R2_ll, w);
        h_MET_noMll_Z_nj6->Fill(MET_ll,w);
        h_Mll_noMll_Z->Fill(M_ll,w);
      }
    }
    vect = {"2Lep", "OppCharge", "Mll" };
    if (apply_all_cuts_except('Z', vect)) {
      h_ht_AK4_no2Lep_Z->Fill(AK4_Ht, w);
      h_jet1_pt_no2Lep_Z->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no2Lep_Z->Fill(MR, w);
      h_R2_no2Lep_Z->Fill(R2_ll, w);
      h_MET_no2Lep_Z->Fill(MET_ll,w);
      h_nLep_no2Lep_Z->Fill(nEleSelect+nMuSelect,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no2Lep_Z_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no2Lep_Z_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no2Lep_Z_nj35->Fill(MR, w);
        h_R2_no2Lep_Z_nj35->Fill(R2_ll, w);
        h_MET_no2Lep_Z_nj35->Fill(MET_ll,w);
        h_nLep_no2Lep_Z_nj35->Fill(nEleSelect+nMuSelect,w);
      }
      if(nJet>=6){
        h_ht_AK4_no2Lep_Z_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no2Lep_Z_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no2Lep_Z_nj6->Fill(MR, w);
        h_R2_no2Lep_Z_nj6->Fill(R2_ll, w);
        h_MET_no2Lep_Z_nj6->Fill(MET_ll,w);
        h_nLep_no2Lep_Z_nj6->Fill(nEleSelect+nMuSelect,w);
      }
    }

    // L enriched region
    w = sf_weight['L'];
    if (apply_all_cuts('L')) {
      h_ht_AK4_L->Fill(AK4_Ht, w);
      h_jet1_pt_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_L->Fill(MR, w);
      h_R2_L->Fill(R2_1vl, w);
      h_tau21_L->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_L->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_L->Fill(data.MET_pt,w);
      h_R2_MR_L->Fill(MR,R2_1vl, w);
      h_R2_MET_L->Fill(MET_1vl, R2_1vl, w);
      h_MR_MET_L->Fill(MET_1vl, MR, w);
      h_AK8Jet1pT_MET_L->Fill(MET_1vl, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_L->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_L->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_L->Fill(data.MET_phi,w);
      if(nEleVeto==1) h_ele_pt_L->Fill(data.Electron[iEleVeto[0]].pt,w);
      else h_mu_pt_L->Fill(data.Muon[iMuVeto[0]].pt,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_L_nj35->Fill(MR, w);
        h_R2_L_nj35->Fill(R2_1vl, w);
        h_tau21_L_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_L_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_L_nj35->Fill(MET_1vl,w);
        h_R2_MR_L_nj35->Fill(MR, R2_1vl, w);
      h_AK8_jet1_pt_L_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_L_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_L_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_L_nj6->Fill(MR, w);
        h_R2_L_nj6->Fill(R2_1vl, w);
        h_tau21_L_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_L_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_L_nj6->Fill(MET_1vl,w);
        h_R2_MR_L_nj6->Fill(MR, R2_1vl, w);
      h_AK8_jet1_pt_L_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_L_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_L_nj6->Fill(data.MET_phi,w);
      }
    }
    vect = {"1Lep", "MT" };
    if (apply_all_cuts_except('L', vect)) {
      h_ht_AK4_no1Lep_L->Fill(AK4_Ht, w);
      h_jet1_pt_no1Lep_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Lep_L->Fill(MR, w);
      h_R2_no1Lep_L->Fill(R2_1vl, w);
      h_MET_no1Lep_L->Fill(MET_1vl,w);
      h_nLep_no1Lep_L->Fill(nLepVeto,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no1Lep_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no1Lep_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Lep_L_nj35->Fill(MR, w);
        h_R2_no1Lep_L_nj35->Fill(R2_1vl, w);
        h_MET_no1Lep_L_nj35->Fill(MET_1vl,w);
        h_nLep_no1Lep_L_nj35->Fill(nLepVeto,w);
      }
      if(nJet>=6){
        h_ht_AK4_no1Lep_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no1Lep_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Lep_L_nj6->Fill(MR, w);
        h_R2_no1Lep_L_nj6->Fill(R2_1vl, w);
        h_MET_no1Lep_L_nj6->Fill(MET_1vl,w);
        h_nLep_no1Lep_L_nj6->Fill(nLepVeto,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('L', "1mW")) {
      h_ht_AK4_nonW_L->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_L->Fill(MR, w);
      h_R2_nonW_L->Fill(R2, w);
      h_MET_nonW_L->Fill(data.MET_pt,w);
      h_nW_nonW_L->Fill(nWMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_L_nj35->Fill(MR, w);
        h_R2_nonW_L_nj35->Fill(R2, w);
        h_MET_nonW_L_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_L_nj35->Fill(nWMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_L_nj6->Fill(MR, w);
        h_R2_nonW_L_nj6->Fill(R2, w);
        h_MET_nonW_L_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_L_nj6->Fill(nWMassTag,w);
      }
    }
#else
    if (apply_all_cuts_except('L', "1mTop")) {
      h_ht_AK4_nonTop_L->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_L->Fill(MR, w);
      h_R2_nonTop_L->Fill(R2, w);
      h_MET_nonTop_L->Fill(data.MET_pt,w);
      h_nTop_nonTop_L->Fill(nHadTop0BMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_L_nj35->Fill(MR, w);
        h_R2_nonTop_L_nj35->Fill(R2, w);
        h_MET_nonTop_L_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_L_nj35->Fill(nHadTop0BMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_L_nj6->Fill(MR, w);
        h_R2_nonTop_L_nj6->Fill(R2, w);
        h_MET_nonTop_L_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_L_nj6->Fill(nHadTop0BMassTag,w);
      }
    }
#endif
    if (apply_all_cuts_except('L', "0b")) {
      h_ht_AK4_no0b_L->Fill(AK4_Ht, w);
      h_jet1_pt_no0b_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no0b_L->Fill(MR, w);
      h_R2_no0b_L->Fill(R2_1vl, w);
      h_MET_no0b_L->Fill(MET_1vl,w);
      h_nb_no0b_L->Fill(nLooseBTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no0b_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no0b_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no0b_L_nj35->Fill(MR, w);
        h_R2_no0b_L_nj35->Fill(R2_1vl, w);
        h_MET_no0b_L_nj35->Fill(MET_1vl,w);
        h_nb_no0b_L_nj35->Fill(nLooseBTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_no0b_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no0b_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no0b_L_nj6->Fill(MR, w);
        h_R2_no0b_L_nj6->Fill(R2_1vl, w);
        h_MET_no0b_L_nj6->Fill(MET_1vl,w);
        h_nb_no0b_L_nj6->Fill(nLooseBTag,w);
      }
    }
    if (apply_all_cuts_except('L', "mDPhi")) {
      h_ht_AK4_nomDPhi_L->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_L->Fill(MR, w);
      h_R2_nomDPhi_L->Fill(R2_1vl, w);
      h_MET_nomDPhi_L->Fill(MET_1vl,w);
      h_mDPhi_nomDPhi_L->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nomDPhi_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_L_nj35->Fill(MR, w);
        h_R2_nomDPhi_L_nj35->Fill(R2_1vl, w);
        h_MET_nomDPhi_L_nj35->Fill(MET_1vl,w);
        h_mDPhi_nomDPhi_L_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
        h_ht_AK4_nomDPhi_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_L_nj6->Fill(MR, w);
        h_R2_nomDPhi_L_nj6->Fill(R2_1vl, w);
        h_MET_nomDPhi_L_nj6->Fill(MET_1vl,w);
        h_mDPhi_nomDPhi_L_nj6->Fill(dPhiRazor,w);
      }
    }
    if (apply_all_cuts_except('L', "MT")) {
      h_ht_AK4_noMT_L->Fill(AK4_Ht, w);
      h_jet1_pt_noMT_L->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_noMT_L->Fill(MR, w);
      h_R2_noMT_L->Fill(R2_1vl, w);
      h_MET_noMT_L->Fill(MET_1vl,w);
      h_MT_noMT_L->Fill(MT_vetolep,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_noMT_L_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_noMT_L_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMT_L_nj35->Fill(MR, w);
        h_R2_noMT_L_nj35->Fill(R2_1vl, w);
        h_MET_noMT_L_nj35->Fill(MET_1vl,w);
        h_MT_noMT_L_nj35->Fill(MT_vetolep,w);
      }
      if(nJet>=6){
        h_ht_AK4_noMT_L_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_noMT_L_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_noMT_L_nj6->Fill(MR, w);
        h_R2_noMT_L_nj6->Fill(R2_1vl, w);
        h_MET_noMT_L_nj6->Fill(MET_1vl,w);
        h_MT_noMT_L_nj6->Fill(MT_vetolep,w);
      }
    }
    // G enriched region
    w = sf_weight['G'];
    if (apply_all_cuts('G')) {
      h_ht_AK4_G->Fill(AK4_Ht, w);
      h_jet1_pt_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_G->Fill(MR, w);
      h_R2_G->Fill(R2_pho, w);
      h_tau21_G->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_G->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_G->Fill(MET_pho,w);
      h_R2_MR_G->Fill(MR, R2_pho, w);
      h_R2_MET_G->Fill(MET_pho, R2_pho, w);
      h_MR_MET_G->Fill(MET_pho, MR, w);
      h_AK8Jet1pT_MET_G->Fill(MET_pho, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_G->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_G->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_G->Fill(data.MET_phi,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_G_nj35->Fill(MR, w);
        h_R2_G_nj35->Fill(R2_pho, w);
        h_tau21_G_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_G_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_G_nj35->Fill(MET_pho,w);
        h_R2_MR_G_nj35->Fill(MR, R2_pho, w);
      h_AK8_jet1_pt_G_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_G_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_G_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_G_nj6->Fill(MR, w);
        h_R2_G_nj6->Fill(R2_pho, w);
        h_tau21_G_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_G_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_G_nj6->Fill(MET_pho,w);
        h_R2_MR_G_nj6->Fill(MR, R2_pho, w);
      h_AK8_jet1_pt_G_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_G_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_G_nj6->Fill(data.MET_phi,w);
      }
    }
    vect = {"0Ele", "0Mu"};
    if (apply_all_cuts_except('G', vect)) {
      h_ht_AK4_no0Lep_G->Fill(AK4_Ht, w);
      h_jet1_pt_no0Lep_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no0Lep_G->Fill(MR, w);
      h_R2_no0Lep_G->Fill(R2_pho, w);
      h_MET_no0Lep_G->Fill(MET_pho,w);
      h_nLep_no0Lep_G->Fill(nLepVeto,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no0Lep_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no0Lep_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no0Lep_G_nj35->Fill(MR, w);
        h_R2_no0Lep_G_nj35->Fill(R2_pho, w);
        h_MET_no0Lep_G_nj35->Fill(MET_pho,w);
        h_nLep_no0Lep_G_nj35->Fill(nLepVeto,w);
      }
      if(nJet>=6){
        h_ht_AK4_no0Lep_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no0Lep_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no0Lep_G_nj6->Fill(MR, w);
        h_R2_no0Lep_G_nj6->Fill(R2_pho, w);
        h_MET_no0Lep_G_nj6->Fill(MET_pho,w);
        h_nLep_no0Lep_G_nj6->Fill(nLepVeto,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('G', "1mW")) {
      h_ht_AK4_nonW_G->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_G->Fill(MR, w);
      h_R2_nonW_G->Fill(R2, w);
      h_MET_nonW_G->Fill(data.MET_pt,w);
      h_nW_nonW_G->Fill(nWMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_G_nj35->Fill(MR, w);
        h_R2_nonW_G_nj35->Fill(R2, w);
        h_MET_nonW_G_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_G_nj35->Fill(nWMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_G_nj6->Fill(MR, w);
        h_R2_nonW_G_nj6->Fill(R2, w);
        h_MET_nonW_G_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_G_nj6->Fill(nWMassTag,w);
      }
    }
#else
    if (apply_all_cuts_except('G', "1mTop")) {
      h_ht_AK4_nonTop_G->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_G->Fill(MR, w);
      h_R2_nonTop_G->Fill(R2, w);
      h_MET_nonTop_G->Fill(data.MET_pt,w);
      h_nTop_nonTop_G->Fill(nHadTopMassTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_G_nj35->Fill(MR, w);
        h_R2_nonTop_G_nj35->Fill(R2, w);
        h_MET_nonTop_G_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_G_nj35->Fill(nHadTopMassTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_G_nj6->Fill(MR, w);
        h_R2_nonTop_G_nj6->Fill(R2, w);
        h_MET_nonTop_G_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_G_nj6->Fill(nHadTopMassTag,w);
      }
    }
#endif
    if (apply_all_cuts_except('G', "1Pho")) {
      h_ht_AK4_no1Pho_G->Fill(AK4_Ht, w);
      h_jet1_pt_no1Pho_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no1Pho_G->Fill(MR, w);
      h_R2_no1Pho_G->Fill(R2_pho, w);
      h_MET_no1Pho_G->Fill(MET_pho,w);
      h_nPho_no1Pho_G->Fill(nPhotonSelect,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_no1Pho_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_no1Pho_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Pho_G_nj35->Fill(MR, w);
        h_R2_no1Pho_G_nj35->Fill(R2_pho, w);
        h_MET_no1Pho_G_nj35->Fill(MET_pho,w);
        h_nPho_no1Pho_G_nj35->Fill(nPhotonSelect,w);
      }
      if(nJet>=6){
        h_ht_AK4_no1Pho_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_no1Pho_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_no1Pho_G_nj6->Fill(MR, w);
        h_R2_no1Pho_G_nj6->Fill(R2_pho, w);
        h_MET_no1Pho_G_nj6->Fill(MET_pho,w);
        h_nPho_no1Pho_G_nj6->Fill(nPhotonSelect,w);
      }
    }
    if (apply_all_cuts_except('G', "mDPhi")) {
      h_ht_AK4_nomDPhi_G->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_G->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_G->Fill(MR, w);
      h_R2_nomDPhi_G->Fill(R2_pho, w);
      h_MET_nomDPhi_G->Fill(MET_pho,w);
      h_mDPhi_nomDPhi_G->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nomDPhi_G_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_G_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_G_nj35->Fill(MR, w);
        h_R2_nomDPhi_G_nj35->Fill(R2_pho, w);
        h_MET_nomDPhi_G_nj35->Fill(MET_pho,w);
        h_mDPhi_nomDPhi_G_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
        h_ht_AK4_nomDPhi_G_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nomDPhi_G_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nomDPhi_G_nj6->Fill(MR, w);
        h_R2_nomDPhi_G_nj6->Fill(R2_pho, w);
        h_MET_nomDPhi_G_nj6->Fill(MET_pho,w);
        h_mDPhi_nomDPhi_G_nj6->Fill(dPhiRazor,w);
      }
    }

    // QCD enriched region
    w = sf_weight['q'];
    if (apply_all_cuts('q')) {
      h_ht_AK4_q->Fill(AK4_Ht, w);
      h_jet1_pt_q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_q->Fill(MR, w);
      h_R2_q->Fill(R2, w);
      h_tau21_q->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_q->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_q->Fill(data.MET_pt,w);
      h_R2_MR_q->Fill(MR, R2, w);
      h_R2_MET_q->Fill(data.MET_pt, R2, w);
      h_MR_MET_q->Fill(data.MET_pt, MR, w);
      h_AK8Jet1pT_MET_q->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_q->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_q->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_q->Fill(data.MET_phi,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_q_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_q_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_q_nj35->Fill(MR, w);
        h_R2_q_nj35->Fill(R2, w);
        h_tau21_q_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_q_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_q_nj35->Fill(data.MET_pt,w);
        h_R2_MR_q_nj35->Fill(MR, R2, w);
      h_AK8_jet1_pt_q_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_q_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_q_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_q_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_q_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_q_nj6->Fill(MR, w);
        h_R2_q_nj6->Fill(R2, w);
        h_tau21_q_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_q_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_q_nj6->Fill(data.MET_pt,w);
        h_R2_MR_q_nj6->Fill(MR, R2, w);
      h_AK8_jet1_pt_q_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_q_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_q_nj6->Fill(data.MET_phi,w);
      }
    }

    // QCD enriched region
    w = sf_weight['Q'];
    if (apply_all_cuts('Q')) {
      h_ht_AK4_Q->Fill(AK4_Ht, w);
      h_jet1_pt_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_Q->Fill(MR, w);
      h_R2_Q->Fill(R2, w);
      h_tau21_Q->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_Q->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_Q->Fill(data.MET_pt,w);
      h_R2_MR_Q->Fill(MR, R2, w);
      h_R2_MET_Q->Fill(data.MET_pt, R2, w);
      h_MR_MET_Q->Fill(data.MET_pt, MR, w);
      h_AK8Jet1pT_MET_Q->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_Q->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Q->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Q->Fill(data.MET_phi,w);

      if(nJet>=3 && nJet<6){
      h_ht_AK4_Q_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_Q_nj35->Fill(MR, w);
      h_R2_Q_nj35->Fill(R2, w);
      h_tau21_Q_nj35->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_Q_nj35->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_Q_nj35->Fill(data.MET_pt,w);
      h_R2_MR_Q_nj35->Fill(MR, R2, w);
      h_AK8_jet1_pt_Q_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Q_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Q_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
      h_ht_AK4_Q_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_Q_nj6->Fill(MR, w);
      h_R2_Q_nj6->Fill(R2, w);
      h_tau21_Q_nj6->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_Q_nj6->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_Q_nj6->Fill(data.MET_pt,w);
      h_R2_MR_Q_nj6->Fill(MR, R2, w);
      h_AK8_jet1_pt_Q_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_Q_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_Q_nj6->Fill(data.MET_phi,w);
      }
    }
    if (apply_all_cuts_except('Q', "InvmDPhi")) {
      h_ht_AK4_nomDPhi_Q->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_Q->Fill(MR, w);
      h_R2_nomDPhi_Q->Fill(R2, w);
      h_MET_nomDPhi_Q->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_Q->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6){
      h_ht_AK4_nomDPhi_Q_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_Q_nj35->Fill(MR, w);
      h_R2_nomDPhi_Q_nj35->Fill(R2, w);
      h_MET_nomDPhi_Q_nj35->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_Q_nj35->Fill(dPhiRazor,w);
      }
      if(nJet>=6){
      h_ht_AK4_nomDPhi_Q_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_nomDPhi_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nomDPhi_Q_nj6->Fill(MR, w);
      h_R2_nomDPhi_Q_nj6->Fill(R2, w);
      h_MET_nomDPhi_Q_nj6->Fill(data.MET_pt,w);
      h_mDPhi_nomDPhi_Q_nj6->Fill(dPhiRazor,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('Q', "1aW")) {
      h_ht_AK4_nonW_Q->Fill(AK4_Ht, w);
      h_jet1_pt_nonW_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonW_Q->Fill(MR, w);
      h_R2_nonW_Q->Fill(R2, w);
      h_MET_nonW_Q->Fill(data.MET_pt,w);
      h_nW_nonW_Q->Fill(nTightWAntiTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonW_Q_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_Q_nj35->Fill(MR, w);
        h_R2_nonW_Q_nj35->Fill(R2, w);
        h_MET_nonW_Q_nj35->Fill(data.MET_pt,w);
        h_nW_nonW_Q_nj35->Fill(nTightWAntiTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonW_Q_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonW_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonW_Q_nj6->Fill(MR, w);
        h_R2_nonW_Q_nj6->Fill(R2, w);
        h_MET_nonW_Q_nj6->Fill(data.MET_pt,w);
        h_nW_nonW_Q_nj6->Fill(nTightWAntiTag,w);
      }
    }
#else
    if (apply_all_cuts_except('Q', "1aTop")) {
      h_ht_AK4_nonTop_Q->Fill(AK4_Ht, w);
      h_jet1_pt_nonTop_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonTop_Q->Fill(MR, w);
      h_R2_nonTop_Q->Fill(R2, w);
      h_MET_nonTop_Q->Fill(data.MET_pt,w);
      h_nTop_nonTop_Q->Fill(nHadTop0BAntiTag,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_nonTop_Q_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_Q_nj35->Fill(MR, w);
        h_R2_nonTop_Q_nj35->Fill(R2, w);
        h_MET_nonTop_Q_nj35->Fill(data.MET_pt,w);
        h_nTop_nonTop_Q_nj35->Fill(nHadTop0BAntiTag,w);
      }
      if(nJet>=6){
        h_ht_AK4_nonTop_Q_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_nonTop_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_nonTop_Q_nj6->Fill(MR, w);
        h_R2_nonTop_Q_nj6->Fill(R2, w);
        h_MET_nonTop_Q_nj6->Fill(data.MET_pt,w);
        h_nTop_nonTop_Q_nj6->Fill(nHadTop0BAntiTag,w);
      }
    }
#endif
    if (apply_all_cuts_except('Q', "0b")) {
      h_ht_AK4_nonb_Q->Fill(AK4_Ht, w);
      h_jet1_pt_nonb_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonb_Q->Fill(MR, w);
      h_R2_nonb_Q->Fill(R2, w);
      h_MET_nonb_Q->Fill(data.MET_pt,w);
      h_nb_nonb_Q->Fill(nLooseBTag,w);
      if(nJet>=3 && nJet<6){
      h_ht_AK4_nonb_Q_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_nonb_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonb_Q_nj35->Fill(MR, w);
      h_R2_nonb_Q_nj35->Fill(R2, w);
      h_MET_nonb_Q_nj35->Fill(data.MET_pt,w);
      h_nb_nonb_Q_nj35->Fill(nLooseBTag,w);
      }
      if(nJet>=6){
      h_ht_AK4_nonb_Q_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_nonb_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_nonb_Q_nj6->Fill(MR, w);
      h_R2_nonb_Q_nj6->Fill(R2, w);
      h_MET_nonb_Q_nj6->Fill(data.MET_pt,w);
      h_nb_nonb_Q_nj6->Fill(nLooseBTag,w);
      }
    }
    vect = {"0Ele", "0Mu"};
    if (apply_all_cuts_except('Q', vect)) {
      h_ht_AK4_no0Lep_Q->Fill(AK4_Ht, w);
      h_jet1_pt_no0Lep_Q->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no0Lep_Q->Fill(MR, w);
      h_R2_no0Lep_Q->Fill(R2, w);
      h_MET_no0Lep_Q->Fill(data.MET_pt,w);
      h_nLep_no0Lep_Q->Fill(nLepVeto,w);
      if(nJet>=3 && nJet<6){
      h_ht_AK4_no0Lep_Q_nj35->Fill(AK4_Ht, w);
      h_jet1_pt_no0Lep_Q_nj35->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no0Lep_Q_nj35->Fill(MR, w);
      h_R2_no0Lep_Q_nj35->Fill(R2, w);
      h_MET_no0Lep_Q_nj35->Fill(data.MET_pt,w);
      h_nLep_no0Lep_Q_nj35->Fill(nLepVeto,w);
      }
      if(nJet>=6){
      h_ht_AK4_no0Lep_Q_nj6->Fill(AK4_Ht, w);
      h_jet1_pt_no0Lep_Q_nj6->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_no0Lep_Q_nj6->Fill(MR, w);
      h_R2_no0Lep_Q_nj6->Fill(R2, w);
      h_MET_no0Lep_Q_nj6->Fill(data.MET_pt,w);
      h_nLep_no0Lep_Q_nj6->Fill(nLepVeto,w);
      }
    }


    // Signal' region
    w = sf_weight['s'];
    //if (apply_all_cuts_except('s', "HLT")) {
    if (apply_all_cuts('s')) {
      h_ht_AK4_s->Fill(AK4_Ht, w);
      h_jet1_pt_s->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_s->Fill(MR, w);
      h_R2_s->Fill(R2, w);
      h_tau21_s->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_s->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_s->Fill(data.MET_pt,w);
      h_R2_MR_s->Fill(MR, R2, w);
      h_R2_MET_s->Fill(data.MET_pt, R2, w);
      h_MR_MET_s->Fill(data.MET_pt, MR, w);
      h_AK8Jet1pT_MET_s->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_s->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_s->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_s->Fill(data.MET_phi,w);
      if(nJet>=3 && nJet<6){
        h_ht_AK4_s_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_s_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_s_nj35->Fill(MR, w);
        h_R2_s_nj35->Fill(R2, w);
        h_tau21_s_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_s_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_s_nj35->Fill(data.MET_pt,w);
        h_R2_MR_s_nj35->Fill(MR, R2, w);
      h_AK8_jet1_pt_s_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_s_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_s_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_s_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_s_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_s_nj6->Fill(MR, w);
        h_R2_s_nj6->Fill(R2, w);
        h_tau21_s_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_s_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_s_nj6->Fill(data.MET_pt,w);
        h_R2_MR_s_nj6->Fill(MR, R2, w);
      h_AK8_jet1_pt_s_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_s_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_s_nj6->Fill(data.MET_phi,w);
      }
    }
#if TOP == 0
    if (apply_all_cuts_except('s', "1W")) {
      h_nW_nonW_s->Fill(nTightWTag,w);
      if(nJet>=3 && nJet<6) h_nW_nonW_s_nj35->Fill(nTightWTag,w);
      if(nJet>=6) h_nW_nonW_s_nj6->Fill(nTightWTag,w);
    }
    if (apply_all_cuts_except('s', "1b")) {
      h_nb_nonb_s->Fill(nMediumBTag,w);
      if(nJet>=3 && nJet<6) h_nb_nonb_s_nj35->Fill(nMediumBTag,w);
      if(nJet>=6) h_nb_nonb_s_nj6->Fill(nMediumBTag,w);
    }
#else
    if (apply_all_cuts_except('s', "1Top")) {
      h_nTop_nonTop_s->Fill(nHadTopTag,w);
      if(nJet>=3 && nJet<6) h_nTop_nonTop_s_nj35->Fill(nHadTopTag,w);
      if(nJet>=6) h_nTop_nonTop_s_nj6->Fill(nHadTopTag,w);
    }
#endif
    if (apply_all_cuts_except('s', "InvmDPhi")) {
      h_mDPhi_nomDPhi_s->Fill(dPhiRazor,w);
      if(nJet>=3 && nJet<6) h_mDPhi_nomDPhi_s_nj35->Fill(dPhiRazor,w);
      if(nJet>=6) h_mDPhi_nomDPhi_s_nj6->Fill(dPhiRazor,w);
    }
#if TOP == 0
    if (apply_all_cuts_except('q', "1aW")) {
      h_nW_nonW_q->Fill(nTightWTag,w);
      if(nJet>=3 && nJet<6) h_nW_nonW_q_nj35->Fill(nTightWTag,w);
      if(nJet>=6) h_nW_nonW_q_nj6->Fill(nTightWTag,w);
    }
#else
    if (apply_all_cuts_except('q', "1aTop")) {
      h_nTop_nonTop_q->Fill(nHadTopTag,w);
      if(nJet>=3 && nJet<6) h_nTop_nonTop_q_nj35->Fill(nHadTopTag,w);
      if(nJet>=6) h_nTop_nonTop_q_nj6->Fill(nHadTopTag,w);
    }
#endif
    if (apply_all_cuts_except('q', "0b")) {
      h_nb_nonb_q->Fill(nMediumBTag,w);
      if(nJet>=3 && nJet<6) h_nb_nonb_q_nj35->Fill(nMediumBTag,w);
      if(nJet>=6) h_nb_nonb_q_nj6->Fill(nMediumBTag,w);
    }

    // Signal region
    w = sf_weight['S'];
    //if (apply_all_cuts_except('S', "HLT")) {
    if (apply_all_cuts('S')) {
      h_ht_AK4_S->Fill(AK4_Ht, w);
      h_jet1_pt_S->Fill(data.Jet[iJet[0]].pt, w);
      h_MR_S->Fill(MR, w);
      h_R2_S->Fill(R2, w);
      h_tau21_S->Fill(tau21.at(iJetAK8[0]),w);
      h_tau32_S->Fill(tau32.at(iJetAK8[0]),w);
      h_MET_S->Fill(data.MET_pt,w);
      h_R2_MR_S->Fill(MR, R2, w);
      h_R2_MET_S->Fill(data.MET_pt, R2, w);
      h_MR_MET_S->Fill(data.MET_pt, MR, w);
      h_R2_HT_S->Fill(AK4_Ht, R2, w);
      h_MR_HT_S->Fill(AK4_Ht, MR, w);
      h_HT_MET_S->Fill(data.MET_pt, AK4_Ht, w);
      h_AK8Jet1pT_MET_S->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_S->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_S->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_S->Fill(data.MET_phi,w);

      if(nJet>=3 && nJet<6){
        h_ht_AK4_S_nj35->Fill(AK4_Ht, w);
        h_jet1_pt_S_nj35->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_S_nj35->Fill(MR, w);
        h_R2_S_nj35->Fill(R2, w);
        h_tau21_S_nj35->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_S_nj35->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_S_nj35->Fill(data.MET_pt,w);
        h_R2_MR_S_nj35->Fill(MR, R2, w);
      h_R2_MET_S_nj35->Fill(data.MET_pt, R2, w);
      h_MR_MET_S_nj35->Fill(data.MET_pt, MR, w);
      h_R2_HT_S_nj35->Fill(AK4_Ht, R2, w);
      h_MR_HT_S_nj35->Fill(AK4_Ht, MR, w);
      h_HT_MET_S_nj35->Fill(data.MET_pt, AK4_Ht, w);
      h_AK8Jet1pT_MET_S_nj35->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_S_nj35->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_S_nj35->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_S_nj35->Fill(data.MET_phi,w);
      }
      if(nJet>=6){
        h_ht_AK4_S_nj6->Fill(AK4_Ht, w);
        h_jet1_pt_S_nj6->Fill(data.Jet[iJet[0]].pt, w);
        h_MR_S_nj6->Fill(MR, w);
        h_R2_S_nj6->Fill(R2, w);
        h_tau21_S_nj6->Fill(tau21.at(iJetAK8[0]),w);
        h_tau32_S_nj6->Fill(tau32.at(iJetAK8[0]),w);
        h_MET_S_nj6->Fill(data.MET_pt,w);
        h_R2_MR_S_nj6->Fill(MR, R2, w);
      h_R2_MET_S_nj6->Fill(data.MET_pt, R2, w);
      h_MR_MET_S_nj6->Fill(data.MET_pt, MR, w);
      h_R2_HT_S_nj6->Fill(AK4_Ht, R2, w);
      h_MR_HT_S_nj6->Fill(AK4_Ht, MR, w);
      h_HT_MET_S_nj6->Fill(data.MET_pt, AK4_Ht, w);
      h_AK8Jet1pT_MET_S_nj6->Fill(data.MET_pt, data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_pt_S_nj6->Fill(data.FatJet[iJetAK8[0]].pt, w);
      h_AK8_jet1_eta_S_nj6->Fill(data.FatJet[iJetAK8[0]].eta, w);
      h_MET_phi_S_nj6->Fill(data.MET_phi,w);
      }
      h_softDropMass->Fill(softDropMassW.at(0),w);
    }

    // Trigger efficiencies
    // No weighting
 
    // I guess these histots are for data only (trigger efficiencies)
    // One could just use the normal weight  for that ( = 1)
    // Since MC already has trigger efficiency weight applied already
    // one could use simply 1 as the weight there also
    // N-1 weights are not currently supported

    //w = isData ? 1 : sf_weight['S'];
    w = 1;


  bool iscomFFsim = TString(sample).Contains("TTJets_madgraphMLM");
  if(iscomFFsim){ 
    for (size_t i=0; i<data.FatJet.size(); ++i) {
      if (passTightWTag[i]) {
        h_AK8Jet1Pt_eta_GenW_no_W->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
        h_AK8Jet1Pt_eta_GenTop_no_Top->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
        if (hasGenW[i])    h_AK8Jet1Pt_eta_GenW_W->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
        if (hasGenTop[i])  h_AK8Jet1Pt_eta_GenTop_Top->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
        if (!hasGenW[i])   h_AK8Jet1Pt_eta_FakeW_W->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
        if (!hasGenTop[i]) h_AK8Jet1Pt_eta_FakeTop_Top->Fill(data.FatJet[i].pt,std::abs(data.FatJet[i].eta),w);
      }
    }
  }
    
  w=1;

  bool isSingleElectron	= TString(sample).Contains("SingleElectron");
  bool isSingleMuon			= TString(sample).Contains("SingleMuon");
  //bool isMET						= TString(sample).Contains("MET");

  bool trigger = false;

  if(isSingleElectron && data.HLT_Ele32_WPTight_Gsf==1 && nEleTight==1 && nMuVeto==0) trigger = true;
  else if(isSingleMuon			&& data.HLT_IsoMu27==1 && nMuTight==1 && nEleVeto==0) trigger= true;
  else if(!isSingleElectron && !isSingleMuon) trigger = true;

  if(trigger){
    if (apply_all_cuts_except('P', "HLT"))	h_HT_TrigNoMass_0->Fill(AK4_Ht,w);
    if (apply_all_cuts('P'))	h_HT_TrigNoMass_1->Fill(AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8JetpT_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].pt,w);
    if (apply_all_cuts('P'))	h_AK8JetpT_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].pt,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,w);
    if (apply_all_cuts('P'))	h_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,w);

    if (apply_all_cuts_except('P', "HLT"))	h_MET_TrigNoMass_0->Fill(data.MET_pt,w);
    if (apply_all_cuts('P'))	h_MET_TrigNoMass_1->Fill(data.MET_pt,w);

    if (apply_all_cuts_except('P', "HLT"))	h_MR_TrigNoMass_0->Fill(MR,w);
    if (apply_all_cuts('P'))	h_MR_TrigNoMass_1->Fill(MR,w);

    if (apply_all_cuts_except('P', "HLT"))	h_R2_TrigNoMass_0->Fill(R2,w);
    if (apply_all_cuts('P'))	h_R2_TrigNoMass_1->Fill(R2,w);

    if (apply_all_cuts_except('P', "HLT"))	h_R2_MR_TrigNoMass_0->Fill(MR,R2,w);
    if (apply_all_cuts('P'))	h_R2_MR_TrigNoMass_1->Fill(MR,R2,w);

    if (apply_all_cuts_except('P', "HLT"))	h_HT_MET_TrigNoMass_0->Fill(data.MET_pt,AK4_Ht,w);
    if (apply_all_cuts('P'))	h_HT_MET_TrigNoMass_1->Fill(data.MET_pt,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_HT_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK4_Ht,w);
    if (apply_all_cuts('P'))	h_HT_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_MET_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,w);
    if (apply_all_cuts('P'))	h_MET_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,w);

    if (apply_all_cuts_except('P', "HLT"))	h_HT_MET_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK4_Ht,w);
    if (apply_all_cuts('P'))	h_HT_MET_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8HT_TrigNoMass_0->Fill(AK8_Ht,w);
    if (apply_all_cuts('P'))			h_AK8HT_TrigNoMass_1->Fill(AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8HT_MET_TrigNoMass_0->Fill(data.MET_pt,AK8_Ht,w);
    if (apply_all_cuts('P'))			h_AK8HT_MET_TrigNoMass_1->Fill(data.MET_pt,AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8HT_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK8_Ht,w);
    if (apply_all_cuts('P'))			h_AK8HT_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT"))	h_AK8HT_MET_AK8JetMass_TrigNoMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK8_Ht,w);
    if (apply_all_cuts('P'))			h_AK8HT_MET_AK8JetMass_TrigNoMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_HT_TrigMass_0->Fill(AK4_Ht,w);
    if (apply_all_cuts('P') )			h_HT_TrigMass_1->Fill(AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8HT_TrigMass_0->Fill(AK8_Ht,w);
    if (apply_all_cuts('P') )			h_AK8HT_TrigMass_1->Fill(AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8JetpT_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].pt,w);
    if (apply_all_cuts('P') )			h_AK8JetpT_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].pt,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,w);
    if (apply_all_cuts('P') )			h_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,w);

    if (apply_all_cuts_except('P', "HLT") )	h_MET_TrigMass_0->Fill(data.MET_pt,w);
    if (apply_all_cuts('P') )			h_MET_TrigMass_1->Fill(data.MET_pt,w);

    if (apply_all_cuts_except('P', "HLT") )	h_MR_TrigMass_0->Fill(MR,w);
    if (apply_all_cuts('P') )			h_MR_TrigMass_1->Fill(MR,w);

    if (apply_all_cuts_except('P', "HLT") )	h_R2_TrigMass_0->Fill(R2,w);
    if (apply_all_cuts('P') )			h_R2_TrigMass_1->Fill(R2,w);

    if (apply_all_cuts_except('P', "HLT") )	h_R2_MR_TrigMass_0->Fill(MR,R2,w);
    if (apply_all_cuts('P') )			h_R2_MR_TrigMass_1->Fill(MR,R2,w);

    if (apply_all_cuts_except('P', "HLT") )	h_HT_MET_TrigMass_0->Fill(data.MET_pt,AK4_Ht,w);
    if (apply_all_cuts('P') )			h_HT_MET_TrigMass_1->Fill(data.MET_pt,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_HT_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK4_Ht,w);
    if (apply_all_cuts('P') )			h_HT_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8HT_MET_TrigMass_0->Fill(data.MET_pt,AK8_Ht,w);
    if (apply_all_cuts('P') )			h_AK8HT_MET_TrigMass_1->Fill(data.MET_pt,AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8HT_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK8_Ht,w);
    if (apply_all_cuts('P') )			h_AK8HT_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,AK8_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_MET_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,w);
    if (apply_all_cuts('P') )			h_MET_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,w);

    if (apply_all_cuts_except('P', "HLT") )	h_HT_MET_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK4_Ht,w);
    if (apply_all_cuts('P') )			h_HT_MET_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK4_Ht,w);

    if (apply_all_cuts_except('P', "HLT") )	h_AK8HT_MET_AK8JetMass_TrigMass_0->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK8_Ht,w);
    if (apply_all_cuts('P') )			h_AK8HT_MET_AK8JetMass_TrigMass_1->Fill(data.FatJet[iJetAK8[0]].msoftdrop,data.MET_pt,AK8_Ht,w);
  }

    /*
      Other examples to use analysis_cuts object

      if (apply_cut('S',"1W"))                          --> 1 Cut from S region
      if (apply_cut('W',"1Wpre"))                       --> 1 Cut from W region
      if (apply_all_cuts('T'))                          --> All cuts in T region
      if (apply_all_cuts_except('Q', "mDPhi<0.25"))     --> N-1 cut
      if (apply_all_cuts_except('S', {"0Ele", "0Mu" })) --> S without Lep veto

      But be aware: Whatever is defined in the baseline_cuts will apply to all histograms
      Also if you use skimmed ntuples (very likely) then those cuts are already applied
      This is because unskimmed ntuple is 4.3 TB in size, and we cannot have them on EOS
    */
  }
  
/*
    h_njet_pre[syst_index]->Fill(nJet,        w);
    h_nb_pre[syst_index]->Fill(nMediumBTag, w);
    h_nw_pre[syst_index]->Fill(nTightWTag,  w);
    h_j1_pt_pre[syst_index]->Fill(data.FatJet[iJetAK8[0]].pt,w);
    h_j2_pt_pre[syst_index]->Fill(data.Jet[iJet[1]].pt, w);
    h_j3_pt_pre[syst_index]->Fill(data.Jet[iJet[2]].pt, w);
    h_MR_pre[syst_index]->Fill(MR, w);
    h_R2_pre[syst_index]->Fill(R2, w);
    h_tau21_pre[syst_index]->Fill(tau21.at(iJetAK8[0]),w);
    h_MET_pre[syst_index]->Fill(data.MET_pt,w);
    h_softDropMass_pre[syst_index]->Fill(softDropMassW.at(0),w);
    h_HT_pre[syst_index]->Fill(AK4_Ht,w);
    h_HT_j1pt_pre[syst_index]->Fill(AK4_Ht,data.FatJet[iJetAK8[0]].pt,w);

    if(pass){
      h_njet_pre_pass[syst_index]->Fill(nJet,        w);
      h_nb_pre_pass[syst_index]->Fill(nMediumBTag, w);
      h_nw_pre_pass[syst_index]->Fill(nTightWTag,  w);
      h_j1 pt_pre_pass[syst_index]->Fill(data.FatJet[iJetAK8[0]].pt,w);
      h_j2_pt_pre_pass[syst_index]->Fill(data.Jet[iJet[1]].pt, w);
      h_j3_pt_pre_pass[syst_index]->Fill(data.Jet[iJet[2]].pt, w);
      h_MR_pre_pass[syst_index]->Fill(MR, w);
      h_R2_pre_pass[syst_index]->Fill(R2, w);
      h_tau21_pre_pass[syst_index]->Fill(tau21.at(iJetAK8[0]),w);
      h_MET_pre_pass[syst_index]->Fill(data.MET_pt,w);
      h_softDropMass_pre_pass[syst_index]->Fill(softDropMassW.at(0),w);
      h_HT_pre_pass[syst_index]->Fill(AK4_Ht,w);
      h_HT_j1pt_pre_pass[syst_index]->Fill(AK4_Ht,data.FatJet[iJetAK8[0]].pt,w);
    }
*/

  // Vary systematics and save each variation into a different historgam
  // Switch on settings.varySystematics to be effective
  //if (apply_all_cuts('S')) vh_jet1_pt[syst_index]->Fill(data.Jet[iJet[0]].pt, w);

    w = sf_weight['S'];
    if (apply_all_cuts('S')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_S_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_S_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['s'];
    if (apply_all_cuts('s')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_s_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_s_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['Q'];
    if (apply_all_cuts('Q')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_Q_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_Q_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['q'];
    if (apply_all_cuts('q')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_q_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_q_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['T'];
    if (apply_all_cuts('T')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_T_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_T_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['W'];
    if (apply_all_cuts('W')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_W_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_W_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['Z'];
    if (apply_all_cuts('Z')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_Z_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_Z_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['G'];
    if (apply_all_cuts('G')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_G_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_G_nj6[syst_index]->Fill(MR, R2, w);
    }
    w = sf_weight['L'];
    if (apply_all_cuts('L')) {
      if(nJet>=3 && nJet<6) vh_R2_MR_L_nj35[syst_index]->Fill(MR, R2, w);
      if(nJet>=6)						vh_R2_MR_L_nj6[syst_index]->Fill(MR, R2, w);
    }

}

// Methods used by SmartHistos (Plotter)
// Can leave them empty
void
Analysis::define_histo_options(const double& w, const eventBuffer& d, const unsigned int& syst_nSyst,
			       const unsigned int& syst_index, bool runOnSkim=false)
{
}

void
Analysis::load_analysis_histos(std::string inputfile)
{
}

void
Analysis::save_analysis_histos(bool draw=0)
{
}
