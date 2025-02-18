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

void LGE4(TString LENGTH = "", TString DETECTOR = "S13-BIC") {
  if (DETECTOR != "S12-GlueX" && DETECTOR != "S13-BIC" && DETECTOR != "S13360-6025CS") { cout << endl << "Argument 3 (DETECTOR) is \"S12-GlueX,\" \"S13-BIC,\" or \"S13360-6025CS\"" << endl; return; }
  int NLAYERS = 12;
  double layers[NLAYERS]; for (int j = 0; j < NLAYERS; j++) layers[j] = j+1;
  TString GAP[2];
  double nGAP[2], nPMMA;
  nPMMA = 1.499906;
  GAP[0] = "air"; nGAP[0] = 1.00028;
  GAP[1] = "cookie"; nGAP[1] = 1.43;
  TString SHAPE[3], lgshape[3];
  SHAPE[0] = "b"; lgshape[0] = "Nominal";
  SHAPE[1] = "bt"; lgshape[1] = "Trapezoidal";
  SHAPE[2] = "bh"; lgshape[2] = "Hexagonal";

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
  TTree *tree[3][NLAYERS];
  TTree *treeQS[3][NLAYERS];
  TCanvas *C, *C2;
  C2 = new TCanvas("","",900,600);
  TH2F *yvsx_s, *yvsx_f, *yvsz_f, *xvsz_f, *yvsx_detection, *x_exit_x_entrance, *x_exit_y_entrance, *y_exit_x_entrance, *y_exit_y_entrance, *theta_exit_theta_entrance, *temphist;
  TH1F *theta_entrance, *theta_s, *phi_s, *x_eff, *y_eff, *theta_eff, *temphist1D;
for (int k = 0; k < 2; k++) {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NLAYERS; j++) {
      tree[i][j] = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-200000events-%i-mm-lightguide-%s%i-%s-%s.root",atoi(LENGTH),SHAPE[i].Data(),j+1,GAP[k].Data(),DETECTOR.Data()))->Get("lightguides"));
      if (tree[i][j]->IsZombie()) return;
      treeQS[i][j] = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-400000events-%i-mm-lightguide-%s%i-%s-%s.root",atoi(LENGTH),SHAPE[i].Data(),j+1,GAP[k].Data(),DETECTOR.Data()))->Get("lightguides"));
      if (treeQS[i][j]->IsZombie()) return;
    }
  }

  double efficiencies[3][NLAYERS];
  double UNAAD[3][NLAYERS];
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NLAYERS; j++) {
      efficiencies[i][j] = (double)tree[i][j]->GetEntries("detected")/(double)tree[i][j]->GetEntries();
      
    treeQS[i][j]->Draw("detectionY:detectionX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
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
    }
  }

  C = new TCanvas();
  TGraph *g_lge[3];
  TLegend *legend = new TLegend(0.8,0.85-0.05*3,0.93,0.85);
  for (int i = 0; i < 3; i++) {
    g_lge[i] = new TGraph(NLAYERS,layers,efficiencies[i]);
    if (i == 0) g_lge[0]->SetMinimum(1.0);
    if (i == 0) g_lge[i]->SetTitle(Form("%s mm %s %s Efficiency vs. Layer; Light Guide Layer; Efficiency",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()));
    g_lge[i]->SetMarkerStyle(markers[i]); g_lge[i]->SetMarkerSize(1.5); g_lge[i]->SetMarkerColorAlpha(colours[i],0.8); g_lge[i]->SetLineWidth(4); g_lge[i]->SetLineColorAlpha(colours[i],0.8);
    legend->AddEntry(g_lge[i],Form("%s",lgshape[i].Data()),"pl");
    C->cd();
    if (TMath::MaxElement(NLAYERS,g_lge[i]->GetY()) > g_lge[0]->GetMaximum()) g_lge[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_lge[i]->GetY()));
    if (i == 3-1) g_lge[0]->SetMaximum(g_lge[0]->GetMaximum()*1.02);
    if (TMath::MinElement(NLAYERS,g_lge[i]->GetY()) < g_lge[0]->GetMinimum()) g_lge[0]->SetMinimum(TMath::MinElement(NLAYERS,g_lge[i]->GetY()));
    if (i == 3-1) g_lge[0]->SetMinimum(g_lge[0]->GetMinimum()*0.98);
    //if (i == 0) { g_lge[i]->SetMaximum(1.0); g_lge[i]->SetMinimum(0.3); }
    if (i == 0) g_lge[i]->Draw();
    else g_lge[i]->Draw("PLSAME");
  }
  legend->Draw("SAME");

  C->Print(Form("SingleClad-AlternateShapes-Eff-vs-Layer-%s-mm-%s-%s.pdf",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()),Form("Title:%s-%s-%s Alternate Shapes Efficiency vs. Layer",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()));
  
  C->Clear();
  TGraph *g_unaad[3];
  for (int i = 0; i < 3; i++) {
    g_unaad[i] = new TGraph(NLAYERS,layers,UNAAD[i]);
    if (i == 0) g_unaad[0]->SetMinimum(1.0);
    if (i == 0) g_unaad[i]->SetTitle(Form("%s mm %s %s NAAD vs. Layer; Light Guide Layer; NAAD",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()));
    g_unaad[i]->SetMarkerStyle(markers[i]); g_unaad[i]->SetMarkerSize(1.5); g_unaad[i]->SetMarkerColorAlpha(colours[i],0.8); g_unaad[i]->SetLineWidth(4); g_unaad[i]->SetLineColorAlpha(colours[i],0.8);
    C->cd();
    if (TMath::MaxElement(NLAYERS,g_unaad[i]->GetY()) > g_unaad[0]->GetMaximum()) g_unaad[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_unaad[i]->GetY()));
    if (i == 3-1) g_unaad[0]->SetMaximum(g_unaad[0]->GetMaximum()*1.02);
    if (TMath::MinElement(NLAYERS,g_unaad[i]->GetY()) < g_unaad[0]->GetMinimum()) g_unaad[0]->SetMinimum(TMath::MinElement(NLAYERS,g_unaad[i]->GetY()));
    if (i == 3-1) g_unaad[0]->SetMinimum(g_unaad[0]->GetMinimum()*0.98);
    //if (i == 0) { g_unaad[i]->SetMaximum(1.0); g_unaad[i]->SetMinimum(0.4); }
    if (i == 0) g_unaad[i]->Draw();
    else g_unaad[i]->Draw("PLSAME");
  }
  legend->Draw("SAME");

  C->Print(Form("SingleClad-AlternateShapes-UNAAD-vs-Layer-%s-mm-%s-%s.pdf",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()),Form("Title:%s-%s-%s Alternate Shapes UNAAD vs. Layer",LENGTH.Data(),GAP[k].Data(),DETECTOR.Data()));
}
}
