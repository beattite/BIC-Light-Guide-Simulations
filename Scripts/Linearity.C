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

void Linearity(TString SHAPE = "", TString LAYER = "", TString LENGTH = "50", TString GAP = "cookie", TString DETECTOR = "S13-BIC") {
  if (SHAPE == "" || (SHAPE != "g" && SHAPE != "b" && SHAPE != "bm" && SHAPE != "bt" && SHAPE != "bh")) { cout << endl << "Argument 1 (SHAPE) is \"g\" (GlueX LGs), \"b\" (BIC LGs), \"bm\" (BIC mini LGs), \"bt\" (BIC trapezoidal LGs), or \"bh\" (BIC hexagonal LGs)" << endl; return; }
  if (DETECTOR != "S12-GlueX" && DETECTOR != "S13-BIC" && DETECTOR != "S13360-6025CS") { cout << endl << "Argument 3 (DETECTOR) is \"S12-GlueX,\" \"S13-BIC,\" or \"S13360-6025CS\"" << endl; return; }
  TString lgshape;
  if (SHAPE == "g") lgshape = "GlueX";
  else if (SHAPE == "b") lgshape = "BIC";
  else if (SHAPE == "bm") lgshape = "BICmini";
  else if (SHAPE == "bt") lgshape = "BICtrapezoidal";
  else if (SHAPE == "bh") lgshape = "BIChexagonal";

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

  double photons[] = {100, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000, 30000, 40000, 50000};
  int NPHOTONS = sizeof(photons)/sizeof(double);
  TTree *tree;
  double npe[NPHOTONS], linearity[NPHOTONS], pixels[NPHOTONS];
  TCanvas *C;
  TH2F *yvsx_detection, *temphist;
  
  tree = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-400000events-%s-mm-lightguide-%s%s-%s-%s.root",LENGTH.Data(),SHAPE.Data(),LAYER.Data(),GAP.Data(),DETECTOR.Data()))->Get("lightguides"));
  if (tree->IsZombie()) return;

  int currentevent = 0;
  for (int i = 0; i < NPHOTONS; i++) {
    tree->Draw("detectionY:detectionX>>h(252,-6.3,6.3,252,-6.3,6.3)",Form("detected == 1 && eventNumber >= %i && eventNumber < %f && (detectionX <= -3.3 || (detectionX >= -3.1 && detectionX <= -0.1) || (detectionX >= 0.1 && detectionX <= 3.1) || detectionX >= 3.3) && (detectionY <= -3.3 || (detectionY >= -3.1 && detectionY <= -0.1) || (detectionY >= 0.1 && detectionY <= 3.1) || detectionY >= 3.3)", currentevent, currentevent+photons[i]));
    currentevent += photons[i];
    temphist = (TH2F*)gDirectory->Get("h"); yvsx_detection = (TH2F*)temphist->Clone("yvsx_detection");
    npe[i] = (double)yvsx_detection->GetEntries(); // Number of photons reaching the SiPM
    int npixels = 0;
    int nmultiple = 0;
    for (int m = 0; m < 252*252; m++) {
      if (yvsx_detection->GetBinContent(m) > 0) {
        npixels++;
        if (yvsx_detection->GetBinContent(m) > 1) nmultiple += (yvsx_detection->GetBinContent(m) - 1);
      }
    }
    pixels[i] = (double)npixels;
    linearity[i] = 1.0 - (double)nmultiple/npe[i];
    cout << endl << photons[i] << "	" << currentevent << "	" << npe[i] << "	" << pixels[i] << "	" << nmultiple;
  }

  TGraph *g_linearity, *g_npelinearity;
  
  C = new TCanvas();
  g_linearity = new TGraph(NPHOTONS,photons,linearity);
  g_linearity->SetTitle(Form("Quadrant Source %s Layer %s %s mm %s %s Linearity vs. Thrown Photons; Thrown Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_linearity->SetMarkerStyle(20); g_linearity->SetMarkerSize(2); g_linearity->SetMarkerColor(kBlue-1); g_linearity->SetLineWidth(4); g_linearity->SetLineColor(kBlue-1);
  C->cd();
  gPad->SetLogx();
  g_linearity->Draw();

  C->Print(Form("SingleClad-QuadrantSource-Linearity-%s%s-%smm-%s-%s.pdf(",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:Quadrant Source %s%s-%smm-%s-%s Linearity vs. Thrown Photons",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  C->Clear();
  
  C = new TCanvas();
  g_npelinearity = new TGraph(NPHOTONS,npe,linearity);
  g_npelinearity->SetTitle(Form("Quadrant Source %s Layer %s %s mm %s %s Linearity vs. Npe; Detected Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_npelinearity->SetMarkerStyle(20); g_npelinearity->SetMarkerSize(2); g_npelinearity->SetMarkerColor(kBlue-1); g_npelinearity->SetLineWidth(4); g_npelinearity->SetLineColor(kBlue-1);
  C->cd();
  gPad->SetLogx();
  g_npelinearity->Draw();

  C->Print(Form("SingleClad-QuadrantSource-Linearity-%s%s-%smm-%s-%s.pdf",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:Quadrant Source %s%s-%smm-%s-%s Linearity vs. Npe",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  
  C = new TCanvas();
  for (int i = 0; i < NPHOTONS; i++) npe[i] *= 2;
  g_npelinearity = new TGraph(NPHOTONS,npe,linearity);
  g_npelinearity->SetTitle(Form("Quadrant Source %s Layer %s %s mm %s %s Linearity vs. Incident Photons (50%% PDE); Incident Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_npelinearity->SetMarkerStyle(20); g_npelinearity->SetMarkerSize(2); g_npelinearity->SetMarkerColor(kBlue-1); g_npelinearity->SetLineWidth(4); g_npelinearity->SetLineColor(kBlue-1);
  C->cd();
  gPad->SetLogx();
  g_npelinearity->Draw();

  C->Print(Form("SingleClad-QuadrantSource-Linearity-%s%s-%smm-%s-%s.pdf)",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:Quadrant Source %s%s-%smm-%s-%s Linearity vs. Npe (50%% PDE)",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
}
