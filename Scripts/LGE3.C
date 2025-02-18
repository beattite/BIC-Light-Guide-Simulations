#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPaveLabel.h>
#include <TLatex.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TLine.h>
#include <TMath.h>
#include <TGraphErrors.h>
#include <TEllipse.h>
#include <TBox.h>
#include <iostream>
#include <fstream>
#include <TColor.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TLegendEntry.h>

void LGE3(TString SHAPE = "", TString DETECTOR = "S13-BIC") {
  if (SHAPE == "" || (SHAPE != "g" && SHAPE != "b" && SHAPE != "bm" && SHAPE != "bt" && SHAPE != "bh")) { cout << endl << "Argument 1 (SHAPE) is \"g\" (GlueX LGs), \"b\" (BIC LGs), \"bm\" (BIC mini LGs), \"bt\" (BIC trapezoidal LGs), or \"bh\" (BIC hexagonal LGs)" << endl; return; }
  if (DETECTOR != "S12-GlueX" && DETECTOR != "S13-BIC" && DETECTOR != "S13360-6025CS") { cout << endl << "Argument 3 (DETECTOR) is \"S12-GlueX,\" \"S13-BIC,\" or \"S13360-6025CS\"" << endl; return; }
  int NLAYERS;
  TString lgshape;
  if (SHAPE == "g") { NLAYERS = 10; lgshape = "GlueX"; }
  else if (SHAPE == "b") { NLAYERS = 12; lgshape = "BIC"; }
  else if (SHAPE == "bm") { NLAYERS = 12; lgshape = "BICmini"; }
  else if (SHAPE == "bt") { NLAYERS = 12; lgshape = "BICtrapezoidal"; }
  else if (SHAPE == "bh") { NLAYERS = 12; lgshape = "BIChexagonal"; }
  double layers[NLAYERS]; for (int j = 0; j < NLAYERS; j++) layers[j] = j+1;
  TString GAP[2];
  double nGAP[2], nPMMA;
  nPMMA = 1.499906;
  GAP[0] = "air"; nGAP[0] = 1.00028;
  GAP[1] = "cookie"; nGAP[1] = 1.43;

// set the paper & margin sizes
gStyle->SetPaperSize(20,26);
gStyle->SetPadTopMargin(0.12);
gStyle->SetPadRightMargin(0.05);
gStyle->SetPadBottomMargin(0.16);
gStyle->SetPadLeftMargin(0.12);

// use large Times-Roman fonts
//gStyle->SetTextFont(132);
gStyle->SetTextSize(0.08);
//gStyle->SetLabelFont(132,"x");
//gStyle->SetLabelFont(132,"y");
//gStyle->SetLabelFont(132,"z");
gStyle->SetLabelSize(0.05,"x");
gStyle->SetTitleSize(0.07,"x");
gStyle->SetLabelSize(0.05,"y");
gStyle->SetTitleSize(0.07,"y");
gStyle->SetLabelSize(0.05,"z");
gStyle->SetTitleSize(0.07,"z");
gStyle->SetTitleSize(0.08,"t");
//gStyle->SetTitleSize(0.065,"xy");
gStyle->SetTitleOffset(0.8,"y");

gStyle->SetPadTickX(1);
gStyle->SetPadTickY(1);
gStyle->SetPadGridX(true);
gStyle->SetPadGridY(true);
gStyle->SetGridColor(13);
gStyle->SetOptStat(0);
//gStyle->SetLegendTextSize(0.05);
gStyle->SetLegendFillColor(19);
gStyle->SetFillStyle(3144);
//gStyle->SetStatStyle(3001);
gROOT->ForceStyle();

  int colours[] = {kBlue+1, kRed+1, kGreen+1, kGray+2, kViolet, kOrange+1, kCyan+1};
  int markers[] = {20, 21, 22, 23, 47, 33, 43};
  double lengths[] = {20, 30, 40, 45, 50, 60, 80};
  //double lengths[] = {10, 15, 20, 25, 30, 35, 40}; // BIC mini
  int NLENGTHS = sizeof(lengths)/sizeof(double);
  TTree *tree[NLENGTHS][NLAYERS];
  double efficiencyratio[NLENGTHS][NLAYERS];
  ofstream o_efficiencies[2], o_efficiencyratio;
  o_efficiencies[0].open(Form("SingleClad-Eff-vs-Layer-%s-%s-%s.txt",lgshape.Data(),GAP[0].Data(),DETECTOR.Data())); o_efficiencies[0] << "L (mm)	eff_L1	eff_L2	...	(air gap)"; o_efficiencies[0].precision(5); o_efficiencies[0].setf(ios::fixed); o_efficiencies[0].setf(ios::showpoint);
  o_efficiencies[1].open(Form("SingleClad-Eff-vs-Layer-%s-%s-%s.txt",lgshape.Data(),GAP[1].Data(),DETECTOR.Data())); o_efficiencies[1] << "L (mm)	eff_L1	eff_L2	...	(silicone cookie)"; o_efficiencies[1].precision(5); o_efficiencies[1].setf(ios::fixed); o_efficiencies[1].setf(ios::showpoint);
  o_efficiencyratio.open(Form("SingleClad-EffRatio-vs-Layer-%s-%s.txt",lgshape.Data(),DETECTOR.Data())); o_efficiencyratio << "L (mm)	eff_L1	eff_L2	...	(air gap / silicone cookie)";
  TCanvas *C, *C2; o_efficiencyratio.precision(5); o_efficiencyratio.setf(ios::fixed); o_efficiencyratio.setf(ios::showpoint);
  C2 = new TCanvas("","",900,600);
  TH2F *yvsx_s, *yvsx_f, *yvsz_f, *xvsz_f, *yvsx_detection, *x_exit_x_entrance, *x_exit_y_entrance, *y_exit_x_entrance, *y_exit_y_entrance, *theta_exit_theta_entrance, *temphist;
  TH1F *theta_entrance, *theta_s, *phi_s, *x_eff, *y_eff, *theta_eff, *temphist1D;
for (int k = 0; k < 2; k++) {
  for (int i = 0; i < NLENGTHS; i++) {
    for (int j = 0; j < NLAYERS; j++) {
      tree[i][j] = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-200000events-%i-mm-lightguide-%s%i-%s-%s.root",(int)lengths[i],SHAPE.Data(),j+1,GAP[k].Data(),DETECTOR.Data()))->Get("lightguides"));
      if (tree[i][j]->IsZombie()) return;
    }
  }

  C2->Print(Form("SingleClad-LightMixing-%s-%s-%s.pdf[",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
  double efficiencies[NLENGTHS][NLAYERS];
  double UNAAD[NLENGTHS][NLAYERS];
  for (int i = 0; i < NLENGTHS; i++) {
    o_efficiencies[k] << endl << (int)lengths[i];
    if (k == 1) o_efficiencyratio << endl << (int)lengths[i];
    for (int j = 0; j < NLAYERS; j++) {
      efficiencies[i][j] = (double)tree[i][j]->GetEntries("detected")/(double)tree[i][j]->GetEntries();
      if (k == 0) efficiencyratio[i][j] = efficiencies[i][j];
      else if (k == 1) efficiencyratio[i][j] /= efficiencies[i][j];

      o_efficiencies[k] << "	" << efficiencies[i][j];
      if (k == 1) o_efficiencyratio << "	" << efficiencyratio[i][j];
      
    tree[i][j]->Draw("detectionY:detectionX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
    temphist = (TH2F*)gDirectory->Get("h"); yvsx_detection = (TH2F*)temphist->Clone("yvsx_detection");
    double ybar = yvsx_detection->Integral(20,41,20,41)/22./22.; // Average inner-SiPM pixel value
    //double ybar = yvsx_detection->Integral(26,35,26,35)/10./10.; // BIC mini
    UNAAD[i][j] = 0.;
    for (int m = 20; m < 42; m++) {
      for (int n = 20; n < 42; n++) {
    //for (int m = 26; m < 36; m++) { // BIC mini
    //  for (int n = 26; n < 36; n++) { // BIC mini
        UNAAD[i][j] += abs(yvsx_detection->GetBinContent(m,n) - ybar);
      }
    }
    UNAAD[i][j] = 1 - (UNAAD[i][j]/ybar/22./22.);
    //UNAAD[i][j] = 1 - (UNAAD[i][j]/ybar/10./10.); // BIC mini
    
    if (j == 4) { // If we only want to output plots for a particular layer or length
    C2->Clear();
    C2->Divide(3,2);
    C2->cd(1);
     tree[i][j]->Draw("exitY:exitX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); yvsx_s = (TH2F*)temphist->Clone("yvsx_s");
     yvsx_s->SetTitle(Form("%s%i %i mm Exit y vs. x (Success);Exit x (mm);Exit y (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     yvsx_s->Draw("COLZ");
    C2->cd(2);
     tree[i][j]->Draw("exitY:exitX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 0");
     temphist = (TH2F*)gDirectory->Get("h"); yvsx_f = (TH2F*)temphist->Clone("yvsx_f");
     yvsx_f->SetTitle(Form("%s%i %i mm Exit y vs. x (Failure);Exit x (mm);Exit y (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     yvsx_f->Draw("COLZ");
    C2->cd(3);
     tree[i][j]->Draw(Form("exitX:exitZ>>h(%i,%f,%f.0,60,-16.0,16.0)",lengths[i] < 40.0 ? 40 : (int)lengths[i]+10,-lengths[i]/2-8,lengths[i]/2+2),"detected == 0");
     temphist = (TH2F*)gDirectory->Get("h"); xvsz_f = (TH2F*)temphist->Clone("xvsz_f");
     xvsz_f->SetTitle(Form("%s%i %i mm Exit x vs. z (Failure);Exit z (mm);Exit x (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     xvsz_f->Draw("COLZ");
    C2->cd(4);
     yvsx_detection->SetTitle(Form("%s%i %i mm Detection y vs. x (Success);Detection x (mm);Detection y (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     yvsx_detection->Draw("COLZ");
    C2->cd(5);
     tree[i][j]->Draw("exitX:thrownX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); x_exit_x_entrance = (TH2F*)temphist->Clone("x_exit_x_entrance");
     x_exit_x_entrance->SetTitle(Form("%s%i %i mm Exit x vs. Entrance x (Success);Entrance x (mm);Exit x (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     x_exit_x_entrance->Draw("COLZ");
    C2->cd(6);
     tree[i][j]->Draw("exitX:thrownY>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); x_exit_y_entrance = (TH2F*)temphist->Clone("x_exit_y_entrance");
     x_exit_y_entrance->SetTitle(Form("%s%i %i mm Exit x vs. Entrance y (Success);Entrance y (mm);Exit x (mm)",SHAPE.Data(),j+1,(int)lengths[i]));
     x_exit_y_entrance->Draw("COLZ");
    C2->Print(Form("SingleClad-LightMixing-%s-%s-%s.pdf",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()),Form("Title:%s%i %i-mm %s-%s-%s Efficiencies",SHAPE.Data(),j+1,(int)lengths[i],lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
    }
    }
  }

  C = new TCanvas();
  TGraph *g_lge[NLENGTHS];
  TLegend *legend = new TLegend(0.8,0.85-0.05*NLENGTHS,0.93,0.85);
  for (int i = 0; i < NLENGTHS; i++) {
    g_lge[i] = new TGraph(NLAYERS,layers,efficiencies[i]);
    if (i == 0) g_lge[i]->SetTitle(Form("%s %s %s Efficiency vs. Layer; Light Guide Layer; Efficiency",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
    g_lge[i]->SetMarkerStyle(markers[i]); g_lge[i]->SetMarkerSize(1.5); g_lge[i]->SetMarkerColorAlpha(colours[i],0.8); g_lge[i]->SetLineWidth(4); g_lge[i]->SetLineColorAlpha(colours[i],0.8);
    legend->AddEntry(g_lge[i],Form("%i mm",(int)lengths[i]),"pl");
    C->cd();
    //if (TMath::MaxElement(NLAYERS,g_lge[i]->GetY()) > g_lge[0]->GetMaximum()) g_lge[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_lge[i]->GetY()));
    //if (i == NLENGTHS-1) g_lge[0]->SetMaximum(g_lge[0]->GetMaximum()*1.02);
    if (i == 0) { g_lge[i]->SetMaximum(1.0); g_lge[i]->SetMinimum(0.3); }
    if (i == 0) g_lge[i]->Draw();
    else g_lge[i]->Draw("PLSAME");
  }
  legend->Draw("SAME");

  C2->Print(Form("SingleClad-LightMixing-%s-%s-%s.pdf]",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
  C->Print(Form("SingleClad-Eff-vs-Layer-%s-%s-%s.pdf",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()),Form("Title:%s-%s-%s Efficiency vs. Layer",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
  
  C->Clear();
  TGraph *g_unaad[NLENGTHS];
  for (int i = 0; i < NLENGTHS; i++) {
    g_unaad[i] = new TGraph(NLAYERS,layers,UNAAD[i]);
    if (i == 0) g_unaad[i]->SetTitle(Form("%s %s %s NAAD vs. Layer; Light Guide Layer; NAAD",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));
    g_unaad[i]->SetMarkerStyle(markers[i]); g_unaad[i]->SetMarkerSize(1.5); g_unaad[i]->SetMarkerColorAlpha(colours[i],0.8); g_unaad[i]->SetLineWidth(4); g_unaad[i]->SetLineColorAlpha(colours[i],0.8);
    C->cd();
    //if (TMath::MaxElement(NLAYERS,g_unaad[i]->GetY()) > g_unaad[0]->GetMaximum()) g_unaad[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_unaad[i]->GetY()));
    //if (i == NLENGTHS-1) g_unaad[0]->SetMaximum(g_unaad[0]->GetMaximum()*1.02);
    if (i == 0) { g_unaad[i]->SetMaximum(1.0); g_unaad[i]->SetMinimum(0.4); }
    if (i == 0) g_unaad[i]->Draw();
    else g_unaad[i]->Draw("PLSAME");
  }
  legend->Draw("SAME");

  C->Print(Form("SingleClad-UNAAD-vs-Layer-%s-%s-%s.pdf",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()),Form("Title:%s-%s-%s UNAAD vs. Layer",lgshape.Data(),GAP[k].Data(),DETECTOR.Data()));

  if (k == 1) {
    C->Clear();
    for (int i = 0; i < NLENGTHS; i++) {
      g_lge[i] = new TGraph(NLAYERS,layers,efficiencyratio[i]);
      if (i == 0) g_lge[i]->SetTitle(Form("%s %s Efficiency Ratio (Air/Cookie) vs. Layer; Light Guide Layer; Efficiency Ratio",lgshape.Data(),DETECTOR.Data()));
      g_lge[i]->SetMarkerStyle(markers[i]); g_lge[i]->SetMarkerSize(1.5); g_lge[i]->SetMarkerColorAlpha(colours[i],0.8); g_lge[i]->SetLineWidth(4); g_lge[i]->SetLineColorAlpha(colours[i],0.8);
      C->cd();
      //if (TMath::MaxElement(NLAYERS,g_lge[i]->GetY()) > g_lge[0]->GetMaximum()) g_lge[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_lge[i]->GetY()));
      //if (i == NLENGTHS-1) g_lge[0]->SetMaximum(g_lge[0]->GetMaximum()*1.02);
      if (i == 0) { g_lge[i]->SetMaximum(1.0); g_lge[i]->SetMinimum(0.3); }
      if (i == 0) g_lge[i]->Draw();
      else g_lge[i]->Draw("PLSAME");
    }
    legend->Draw("SAME");

    C->Print(Form("SingleClad-EffRatio-vs-Layer-%s-%s.pdf",lgshape.Data(),DETECTOR.Data()),Form("Title:%s-%s Efficiency Ratio (Air/Cookie) vs. Layer",lgshape.Data(),DETECTOR.Data()));
  }
}
  o_efficiencies[0].close();
  o_efficiencies[1].close();
  o_efficiencyratio.close();
}
