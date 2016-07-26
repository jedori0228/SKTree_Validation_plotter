#include "canvas_margin.h"

void set_bin_label(TAxis *axis, TString *labels);
void draw_hist_marker(TH1F* hist);
void draw_validation(){
  
  gStyle->SetOptStat(0);
  TH1::SetDefaultSumw2(true);
  
  TFile *file = new TFile("./rootfiles/validation_data_cat_v7-6-4.root");
  TString plotpath = "./plots/";
  
  TString lumi_bin_name[] = {"LumiBlock 1", "LumiBlock 2", "LumiBlock 3"};
  
  //==== sum lumi
  TCanvas *c_sum_lumi = new TCanvas("c_sum_lumi", "", 800, 600);
  canvas_margin(c_sum_lumi);
  c_sum_lumi->cd();
  TH1F *hist_sum_lumi = new TH1F("hist_sum_lumi", "Sum Lumi of each block", 3, 0., 3);
  hist_sum_lumi->SetBinContent(1, 739.115);
  hist_sum_lumi->SetBinContent(2, 733.593);
  hist_sum_lumi->SetBinContent(3, 826.197);
  set_bin_label(hist_sum_lumi->GetXaxis(),lumi_bin_name);
  hist_sum_lumi->Draw("ltext0");
  //draw_hist_marker(hist_sum_lumi);
  hist_sum_lumi->GetYaxis()->SetRangeUser(0, 1000);
  hist_sum_lumi->SetYTitle("Luminosity [pb^{-1}]");
  c_sum_lumi->SaveAs(plotpath+"sum_lumi.png");
  c_sum_lumi->Close();
  
  //==== number of events
  TCanvas *c_n_events_perLumiBlock = new TCanvas("c_n_events_perLumiBlock", "", 800, 600);
  canvas_margin(c_n_events_perLumiBlock);
  c_n_events_perLumiBlock->cd();
  TH1F *hist_n_events_perLumiBlock = (TH1F*)file->Get("3muon_perLumiBlock")->Clone();
  hist_n_events_perLumiBlock->Draw("ltext0");
  set_bin_label(hist_n_events_perLumiBlock->GetXaxis(), lumi_bin_name);
  hist_n_events_perLumiBlock->GetYaxis()->SetRangeUser(0, 250);
  hist_n_events_perLumiBlock->SetTitle("# of events");
  hist_n_events_perLumiBlock->SetYTitle("# of events");
  c_n_events_perLumiBlock->SaveAs(plotpath+"n_events.png");
  c_n_events_perLumiBlock->Close();
  
  //==== (2muons/3muons)
  TCanvas *c_2mu_over_3mu = new TCanvas("c_2mu_over_3mu", "", 800, 600);
  canvas_margin(c_2mu_over_3mu);
  c_2mu_over_3mu->cd();
  TH1F *hist_2muon = (TH1F*)file->Get("2muon_perLumiBlock");
  TH1F *hist_3muon = (TH1F*)file->Get("3muon_perLumiBlock");
  hist_2muon->Divide(hist_3muon);
  hist_2muon->GetYaxis()->SetRangeUser(2000, 4000);
  hist_2muon->Draw("ltext0");
  //draw_hist_marker(hist_2muon);
  hist_2muon->SetTitle("(# of events with 2 muons) / (# of events with 3 muons)");
  set_bin_label(hist_2muon->GetXaxis(), lumi_bin_name);
  c_2mu_over_3mu->SaveAs(plotpath+"2mu_over_3mu.png");
  c_2mu_over_3mu->Close();
  
  //==== (number of events) / (sum lumi) = cross section
  TCanvas *c_xsec = new TCanvas("c_xsec", "", 800, 600);
  canvas_margin(c_xsec);
  c_xsec->cd();
  TH1F *hist_xsec = (TH1F*)file->Get("3muon_perLumiBlock");
  hist_xsec->Divide(hist_sum_lumi);
  hist_xsec->Draw("ltext0");
  hist_xsec->SetMinimum(0);
  hist_xsec->SetTitle("Effective Cross Section");
  hist_xsec->SetYTitle("xsec [pb]");
  set_bin_label(hist_xsec->GetXaxis(), lumi_bin_name);
  c_xsec->SaveAs(plotpath+"xsec.png");
  c_xsec->Close();
  
  //==== variables
  vector<TString> variables = {"PFMET", "n_jets", "first_Pt", "second_Pt", "third_Pt", "MuonSumPt"};
  vector<TString> hist_titles = {"PFMET", "# of jets", "First Muon p_{T}", "Second Muon p_{T}", "Third Muon p_{T}", "Sum of three muons' p_{T}"};
  vector<TString> y_titles = {"<MET> [GeV]", "<# of jets>", "<p_{T}> [GeV]", "<p_{T}> [GeV]", "<p_{T}> [GeV]", "<p_{T}> [GeV]"};
  vector<double> y_maxs_for_distrib = {20, 120, 20, 20, 20, 20};
  vector<double> y_maxs_for_mean = {100, 2, 100, 100, 100, 150};
  
  for(unsigned int i=0; i<variables.size(); i++){

    TH1F *hist_mean = new TH1F("hist_mean", "Mean of "+hist_titles.at(i), 3, 0., 3);
    set_bin_label(hist_mean->GetXaxis(), lumi_bin_name);
    //=== each lumi block
    for(int j=0; j<3; j++){
      TString this_variable = variables.at(i)+"_LumiBlock_"+TString::Itoa(j,10);
      TCanvas *c1 = new TCanvas("c1", "", 800, 600);
      canvas_margin(c1);
      c1->cd();
      TH1F *hist1 = (TH1F *)file->Get(this_variable);
      hist1->Draw("hist");
      hist1->SetTitle(hist_titles.at(i)+" @ LumiBlock "+TString::Itoa(j, 10));
      hist1->GetYaxis()->SetRangeUser(0, y_maxs_for_distrib.at(i));
      hist1->SetYTitle(y_titles.at(i));
      c1->SaveAs(plotpath+this_variable+".png");
      hist_mean->SetBinContent(j+1, hist1->GetMean());
      hist_mean->SetBinError(j+1, hist1->GetMeanError());
      c1->Close();
      delete hist1;
    }
    //==== mean
    TCanvas *c_mean = new TCanvas("c_mean", "", 800, 600);
    canvas_margin(c_mean);
    c_mean->cd();
    hist_mean->Draw("ltext0");
    hist_mean->SetYTitle(y_titles.at(i));
    hist_mean->GetYaxis()->SetRangeUser(0, y_maxs_for_mean.at(i));
    c_mean->SaveAs(plotpath+variables.at(i)+"_mean.png");
    c_mean->Close();
    delete hist_mean;
    
  }
  
  
  
  
}

void set_bin_label(TAxis *axis, TString *labels){
  for(int i=0; i<3; i++){
    axis->SetBinLabel(i+1, labels[i]);
  }
}

void draw_hist_marker(TH1F* hist){
  
  hist->SetMarkerStyle(5);
  hist->SetMarkerSize(3);
  hist->Draw("ptext0");
  
}

