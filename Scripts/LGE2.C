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

void LGE2(TString SHAPE = "", TString GAP = "cookie", TString DETECTOR = "S13-BIC") {
  if (SHAPE == "" || (SHAPE != "g" && SHAPE != "b")) { cout << endl << "Argument 1 (SHAPE) is \"g\" (GlueX LGs) or \"b\" (BIC LGs)" << endl; return; }
  if (GAP != "air" && GAP != "grease" && GAP != "cookie") { cout << endl << "Argument 2 (GAP) is \"air\", \"grease\", or \"cookie\"" << endl; return; }
  if (DETECTOR != "S12-GlueX" && DETECTOR != "S13-BIC") { cout << endl << "Argument 3 (DETECTOR) is \"S12-GlueX\" or \"S13-BIC\"" << endl; return; }
  int NLAYERS;
  if (SHAPE == "g") NLAYERS = 10;
  else if (SHAPE == "b") NLAYERS = 12;
  double layers[NLAYERS]; for (int j = 0; j < NLAYERS; j++) layers[j] = j+1;
  double nGAP, nPMMA;
  nPMMA = 1.499906;
  if (GAP == "air") nGAP = 1.00028;
  else if (GAP == "grease") nGAP = 1.465;
  else if (GAP == "cookie") nGAP = 1.43;
  else { cout << endl << "Argument 2 (GAP) is \"air\", \"grease\", or \"cookie\"" << endl; return; } // Should never happen

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
//gStyle->SetStatStyle(3001);
gROOT->ForceStyle();

  int colours[] = {kBlue+1, kRed+1, kGreen+1, kViolet, kOrange+1, kCyan+1};
  int markers[] = {20, 21, 22, 47, 33, 43};
  double lengths[] = {20, 30, 40, 50, 60, 80};
  int NLENGTHS = sizeof(lengths)/sizeof(double);
  TTree *tree[NLENGTHS][NLAYERS];
  TCanvas *C, *C2;
//  C2 = new TCanvas("","",600,600);
  TH2F *yvsx_s, *yvsx_f, *yvsz_f, *xvsz_f, *yvsx_detection, *x_exit_x_entrance, *x_exit_y_entrance, *y_exit_x_entrance, *y_exit_y_entrance, *theta_exit_theta_entrance, *temphist;
  TH1F *theta_entrance, *theta_s, *phi_s, *x_eff, *y_eff, *theta_eff, *temphist1D;
  for (int i = 0; i < NLENGTHS; i++) {
    for (int j = 0; j < NLAYERS; j++) {
      tree[i][j] = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/450nm-100000events-%i-mm-lightguide-%s%i-%s-%s.root",(int)lengths[i],SHAPE.Data(),j+1,GAP.Data(),DETECTOR.Data()))->Get("lightguides"));
      if (tree[i][j]->IsZombie()) return;
    }
  }

//  C2->Print(Form("Extra-%s-%s-%s.pdf[",SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
  double efficiencies[NLENGTHS][NLAYERS];
  for (int i = 0; i < NLENGTHS; i++) {
    for (int j = 0; j < NLAYERS; j++) {
      efficiencies[i][j] = (double)tree[i][j]->GetEntries("detected")/(double)tree[i][j]->GetEntries();





/*    C2->Divide(2,2);
    C2->cd(1);
     tree[i]->Draw("exitY:exitX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); yvsx_s = (TH2F*)temphist->Clone("yvsx_s");
     yvsx_s->SetTitle(Form("%i mm Exit y vs. x (Success);Exit x (mm);Exit y (mm)",(int)lengths[i]));
     yvsx_s->Draw("COLZ");
    C2->cd(2);
     tree[i]->Draw("exitY:exitX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 0");
     temphist = (TH2F*)gDirectory->Get("h"); yvsx_f = (TH2F*)temphist->Clone("yvsx_f");
     yvsx_f->SetTitle(Form("%i mm Exit y vs. x (Failure);Exit x (mm);Exit y (mm)",(int)lengths[i]));
     yvsx_f->Draw("COLZ");
    C2->cd(3);
     tree[i]->Draw(Form("exitX:exitZ>>h(%i,%f,%f.0,60,-16.0,16.0)",lengths[i] < 40.0 ? 40 : (int)lengths[i]+10,-lengths[i]/2-8,lengths[i]/2+2),"detected == 0");
     temphist = (TH2F*)gDirectory->Get("h"); xvsz_f = (TH2F*)temphist->Clone("xvsz_f");
     xvsz_f->SetTitle(Form("%i mm Exit x vs. z (Failure);Exit z (mm);Exit x (mm)",(int)lengths[i]));
     xvsz_f->Draw("COLZ");
    C2->cd(4);
     tree[i]->Draw(Form("exitY:exitZ>>h(%i,%f,%f.0,60,-16.0,16.0)",lengths[i] < 40.0 ? 40 : (int)lengths[i]+10,-lengths[i]/2-8,lengths[i]/2+2),"detected == 0");
     temphist = (TH2F*)gDirectory->Get("h"); yvsz_f = (TH2F*)temphist->Clone("yvsz_f");
     yvsz_f->SetTitle(Form("%i mm Exit y vs. z (Failure);Exit z (mm);Exit y (mm)",(int)lengths[i]));
     yvsz_f->Draw("COLZ");
    C2->Print(Form("Extra-%s-%s-%s.pdf",SHAPE.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%i-mm %s-%s-%s Exit Distributions",(int)lengths[i],SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
    C2->Clear();

    C2->Divide(2,2);
    C2->cd(1);
     tree[i]->Draw("thrownTheta*180.0/TMath::Pi()>>h(75,0,75)");
     temphist1D = (TH1F*)gDirectory->Get("h"); theta_entrance = (TH1F*)temphist1D->Clone("theta_entrance");
     theta_entrance->SetTitle(Form("%i mm Entrance Theta;Entrance Theta (deg)",(int)lengths[i]));
     theta_entrance->Draw();
    C2->cd(2);
     tree[i]->Draw(Form("asin(sin(exitTheta)*%f/%f)*180.0/TMath::Pi()>>h(75,0,75)",nGAP,nPMMA),"detected == 1");
     temphist1D = (TH1F*)gDirectory->Get("h"); theta_s = (TH1F*)temphist1D->Clone("theta_s");
     theta_s->SetTitle(Form("%i mm Exit Theta (Success);Exit Theta (deg)",(int)lengths[i]));
     theta_s->Draw();
    C2->cd(3);
     tree[i]->Draw("exitPhi*180.0/TMath::Pi()>>h(180,-180,180)","detected == 1");
     temphist1D = (TH1F*)gDirectory->Get("h"); phi_s = (TH1F*)temphist1D->Clone("phi_s");
     phi_s->SetTitle(Form("%i mm Exit Phi (Success);Exit Phi (deg)",(int)lengths[i]));
     phi_s->Draw();
    C2->cd(4);
     tree[i]->Draw(Form("asin(sin(exitTheta)*%f/%f)*180.0/TMath::Pi():thrownTheta*180.0/TMath::Pi()>>h(75,0,75,75,0,75)",nGAP,nPMMA),"detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); theta_exit_theta_entrance = (TH2F*)temphist->Clone("theta_exit_theta_entrance");
     theta_exit_theta_entrance->SetTitle(Form("%i mm Exit Theta vs. Entrance Theta (Success);Entrance Theta (deg);Exit Theta (deg)",(int)lengths[i]));
     theta_exit_theta_entrance->Draw("COLZ");
    C2->Print(Form("Extra-%s-%s-%s.pdf",SHAPE.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%i-mm %s-%s-%s Angles",(int)lengths[i],SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
    C2->Clear();

    C2->Divide(2,2);
    C2->cd(1);
     tree[i]->Draw("thrownX>>h(60,-16.0,16.0)","detected == 1");
     temphist1D = (TH1F*)gDirectory->Get("h"); x_eff = (TH1F*)temphist1D->Clone("x_eff");
     tree[i]->Draw("thrownX>>h(60,-16.0,16.0)");
     temphist1D = (TH1F*)gDirectory->Get("h"); x_eff->Divide(temphist1D);
     x_eff->SetTitle(Form("%i mm Entrance x Efficiency;Entrance x (mm)",(int)lengths[i]));
     x_eff->Draw();
    C2->cd(2);
     tree[i]->Draw("thrownY>>h(60,-16.0,16.0)","detected == 1");
     temphist1D = (TH1F*)gDirectory->Get("h"); y_eff = (TH1F*)temphist1D->Clone("y_eff");
     tree[i]->Draw("thrownY>>h(60,-16.0,16.0)");
     temphist1D = (TH1F*)gDirectory->Get("h"); y_eff->Divide(temphist1D);
     y_eff->SetTitle(Form("%i mm Entrance y Efficiency;Entrance y (mm)",(int)lengths[i]));
     y_eff->Draw();
    C2->cd(3);
     tree[i]->Draw("thrownTheta*180.0/TMath::Pi()>>h(50,0,50)","detected == 1");
     temphist1D = (TH1F*)gDirectory->Get("h"); theta_eff = (TH1F*)temphist1D->Clone("theta_eff");
     tree[i]->Draw("thrownTheta*180.0/TMath::Pi()>>h(50,0,50)");
     temphist1D = (TH1F*)gDirectory->Get("h"); theta_eff->Divide(temphist1D);
     theta_eff->SetTitle(Form("%i mm Entrance Theta Efficiency;Entrance Theta (deg)",(int)lengths[i]));
     theta_eff->Draw();
    C2->cd(4);
     tree[i]->Draw("detectionY:detectionX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); yvsx_detection = (TH2F*)temphist->Clone("yvsx_detection");
     yvsx_detection->SetTitle(Form("%i mm Detection y vs. x (Success);Detection x (mm);Detection y (mm)",(int)lengths[i]));
     yvsx_detection->Draw("COLZ");
    C2->Print(Form("Extra-%s-%s-%s.pdf",SHAPE.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%i-mm %s-%s-%s Efficiencies",(int)lengths[i],SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
    C2->Clear();

    C2->Divide(2,2);
    C2->cd(1);
     tree[i]->Draw("exitX:thrownX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); x_exit_x_entrance = (TH2F*)temphist->Clone("x_exit_x_entrance");
     x_exit_x_entrance->SetTitle(Form("%i mm Exit x vs. Entrance x (Success);Entrance x (mm);Exit x (mm)",(int)lengths[i]));
     x_exit_x_entrance->Draw("COLZ");
    C2->cd(2);
     tree[i]->Draw("exitX:thrownY>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); x_exit_y_entrance = (TH2F*)temphist->Clone("x_exit_y_entrance");
     x_exit_y_entrance->SetTitle(Form("%i mm Exit x vs. Entrance y (Success);Entrance y (mm);Exit x (mm)",(int)lengths[i]));
     x_exit_y_entrance->Draw("COLZ");
    C2->cd(3);
     tree[i]->Draw("exitY:thrownX>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); y_exit_x_entrance = (TH2F*)temphist->Clone("y_exit_x_entrance");
     y_exit_x_entrance->SetTitle(Form("%i mm Exit y vs. Entrance x (Success);Entrance x (mm);Exit y (mm)",(int)lengths[i]));
     y_exit_x_entrance->Draw("COLZ");
    C2->cd(4);
     tree[i]->Draw("exitY:thrownY>>h(60,-16.0,16.0,60,-16.0,16.0)","detected == 1");
     temphist = (TH2F*)gDirectory->Get("h"); y_exit_y_entrance = (TH2F*)temphist->Clone("y_exit_y_entrance");
     y_exit_y_entrance->SetTitle(Form("%i mm Exit y vs. Entrance y (Success);Entrance y (mm);Exit y (mm)",(int)lengths[i]));
     y_exit_y_entrance->Draw("COLZ");
    C2->Print(Form("Extra-%s-%s-%s.pdf",SHAPE.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%i-mm %s-%s-%s Light Mixing",(int)lengths[i],SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
    C2->Clear();
*/    
    
    
    
    }
  }

  C = new TCanvas();
  TGraph *g_lge[NLENGTHS];
  TString lgshape;
  TLegend *legend = new TLegend(0.75,0.65,0.93,0.85);
  if (SHAPE == "g") lgshape = "GlueX";
  else if (SHAPE == "b") lgshape = "BIC";
  for (int i = 0; i < NLENGTHS; i++) {
    g_lge[i] = new TGraph(NLAYERS,layers,efficiencies[i]);
    if (i == 0) g_lge[i]->SetTitle(Form("%s %s %s Efficiency vs. Layer; Light Guide Layer; Efficiency",lgshape.Data(),GAP.Data(),DETECTOR.Data()));
    g_lge[i]->SetMarkerStyle(markers[i]); g_lge[i]->SetMarkerSize(1.5); g_lge[i]->SetMarkerColorAlpha(colours[i],0.8); g_lge[i]->SetLineWidth(4); g_lge[i]->SetLineColorAlpha(colours[i],0.8);
    legend->AddEntry(g_lge[i],Form("%i mm",(int)lengths[i]),"pl");
    C->cd();
    if (i == 0) g_lge[i]->Draw();
    else {
      if (TMath::MaxElement(NLAYERS,g_lge[i]->GetY()) > TMath::MaxElement(NLAYERS,g_lge[0]->GetY())) g_lge[0]->SetMaximum(TMath::MaxElement(NLAYERS,g_lge[i]->GetY())*1.02);
      g_lge[i]->Draw("PLSAME");
    }
  }
  legend->Draw("SAME");

//  C2->Print(Form("Extra-%s-%s-%s.pdf]",SHAPE.Data(),GAP.Data(),DETECTOR.Data()));
  C->Print(Form("Eff-vs-Layer-%s-%s-%s.pdf",lgshape.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%s-%s-%s Efficiency vs. Layer",lgshape.Data(),GAP.Data(),DETECTOR.Data()));
}
