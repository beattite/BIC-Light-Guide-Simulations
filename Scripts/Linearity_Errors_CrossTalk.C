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
#include <TGaxis.h>
#include <TRandom3.h>

void Linearity_Errors_CrossTalk(TString SHAPE = "", TString LAYER = "", TString LENGTH = "50", TString GAP = "cookie", TString DETECTOR = "S13-BIC") {
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
gStyle->SetPadTopMargin(0.14);
gStyle->SetPadRightMargin(0.12);
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

//gStyle->SetPadTickX(1);
//gStyle->SetPadTickY(1);
gStyle->SetPadGridX(true);
gStyle->SetPadGridY(true);
gStyle->SetGridColor(13);
gStyle->SetOptStat(0);
//gStyle->SetLegendTextSize(0.05);
gStyle->SetLegendFillColor(19);
gStyle->SetFillStyle(3144);
//gStyle->SetStatStyle(3001);
gROOT->ForceStyle();

//  double photons[] = {100, 100};
  double photons[] = {100, 500, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000, 30000, 40000, 50000, 80000, 100000};
  const int NPHOTONS = sizeof(photons)/sizeof(double);
  TTree *tree;
  double npe[NPHOTONS] = {0};
  double linearity[NPHOTONS] = {0};
  double pixels[NPHOTONS] = {0};
  double npe_error[NPHOTONS] = {0};
  double linearity_error[NPHOTONS] = {0};
  double pixels_error[NPHOTONS] = {0};
  TCanvas *C;
  TH2F *yvsx_detection, *temphist;

//  tree = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-200000events-%s-mm-lightguide-%s%s-%s-%s.root",LENGTH.Data(),SHAPE.Data(),LAYER.Data(),GAP.Data(),DETECTOR.Data()))->Get("lightguides"));
  tree = (TTree*)(TFile::Open(Form("~/BIC/GlueXLG-build/output/SingleClad-450nm-51060000events-%s-mm-lightguide-%s%s-%s-%s.root",LENGTH.Data(),SHAPE.Data(),LAYER.Data(),GAP.Data(),DETECTOR.Data()))->Get("lightguides"));
  if (tree->IsZombie()) return;

  int currentevent = 0;
  int NTRIALS = 100;
  for (int i = 0; i < NPHOTONS; i++) {
    double npe_trial[NTRIALS], linearity_trial[NTRIALS], pixels_trial[NTRIALS];
    for (int j = 0; j < NTRIALS; j++) { // Ten samples of each
      tree->Draw("detectionY:detectionX>>h(252,-6.3,6.3,252,-6.3,6.3)",Form("detected == 1 && eventNumber >= %i && eventNumber < %f && (detectionX <= -3.3 || (detectionX >= -3.1 && detectionX <= -0.1) || (detectionX >= 0.1 && detectionX <= 3.1) || detectionX >= 3.3) && (detectionY <= -3.3 || (detectionY >= -3.1 && detectionY <= -0.1) || (detectionY >= 0.1 && detectionY <= 3.1) || detectionY >= 3.3)", currentevent, currentevent+photons[i]));
      currentevent += photons[i];
      temphist = (TH2F*)gDirectory->Get("h"); yvsx_detection = (TH2F*)temphist->Clone("yvsx_detection");

      TH2F *yvsx_crosstalk[5], *yvsx_current;
      bool crosstalk = false;
      int crosstalklevel = 0;
      if ((double)yvsx_detection->GetEntries() > 0) { crosstalk = true; yvsx_current = (TH2F*)yvsx_detection->Clone("yvsx_current"); }
      while (crosstalk) { // Calculate cross-talk histograms
//   cout << endl << (double)yvsx_current->GetEntries();
        int ctm, ctn;
        yvsx_crosstalk[crosstalklevel] = new TH2F(Form("Crosstalk_%i",crosstalklevel),Form("Crosstalk_%i",crosstalklevel),252,-6.3,6.3,252,-6.3,6.3);
        for (int m = 0; m < 252; m++) {
          for (int n = 0; n < 252; n++) {
            if ((int)yvsx_current->GetBinContent(m,n) > 0) { // Generate cross-talk for this cascade
     //cout << endl << "EVENT PRESENT	" << crosstalklevel << "	" << m << "	" << n << "	" << (int)yvsx_current->GetBinContent(m,n);
              if (gRandom->Uniform(1.0) <= 0.07) { // 7% chance of cross-talk
                bool validbin = false;
                double ctx, cty;
                while (!validbin) {
                  double ctbin = gRandom->Uniform(0.8);
                  if (ctbin < 0.1) { ctm = m - 1; ctn = n - 1; }
                  else if (ctbin < 0.2) { ctm = m; ctn = n - 1; }
                  else if (ctbin < 0.3) { ctm = m + 1; ctn = n - 1; }
                  else if (ctbin < 0.4) { ctm = m - 1; ctn = n; }
                  else if (ctbin < 0.5) { ctm = m + 1; ctn = n; }
                  else if (ctbin < 0.6) { ctm = m - 1; ctn = n + 1; }
                  else if (ctbin < 0.7) { ctm = m; ctn = n + 1; }
                  else { ctm = m + 1; ctn = n + 1; }
      //cout << endl << endl << crosstalklevel << "	" << m << "	" << n << "	" << ctbin << "	" << ctm << "	" << ctn;
                  if (ctm < 0 || ctm >= 252 || ctn < 0 || ctn >= 252) continue; // Cross-talk pixel is out of range
                  ctx = (double)yvsx_crosstalk[crosstalklevel]->GetXaxis()->GetBinCenter(ctm);
                  cty = (double)yvsx_crosstalk[crosstalklevel]->GetYaxis()->GetBinCenter(ctn);
      //cout << endl << ctx << "	" << cty;
                  if ((ctx <= -3.3 || (ctx >= -3.1 && ctx <= -0.1) || (ctx >= 0.1 && ctx <= 3.1) || ctx >= 3.3) && (cty <= -3.3 || (cty >= -3.1 && cty <= -0.1) || (cty >= 0.1 && cty <= 3.1) || cty >= 3.3)) validbin = true; // Cross-talk pixel avoids dead crosses and is a valid pixel
      //if (!validbin) cout << endl << "INVALID";
                }
                yvsx_crosstalk[crosstalklevel]->Fill(ctx,cty);
              }
            }
          }
        }
//    cout << endl << "CROSS TALK	" << crosstalklevel << "	" << (double)yvsx_crosstalk[crosstalklevel]->GetEntries();
        if ((double)yvsx_crosstalk[crosstalklevel]->GetEntries() == 0 || crosstalklevel == 4) crosstalk = false;
        else {
          yvsx_current = (TH2F*)yvsx_crosstalk[crosstalklevel]->Clone("yvsx_current");
          crosstalklevel += 1;
        }
      }

      TH2F *yvsx_total = (TH2F*)yvsx_detection->Clone("yvsx_total"); // Sum detection and cross-talk histograms
      for (int i = 0; i <= crosstalklevel; i++) yvsx_total->Add(yvsx_crosstalk[i]);

      npe_trial[j] = (double)yvsx_detection->GetEntries(); // Number of photoelectrons caused by incident photons
      int npixels = 0;
//      int nmultiple = 0;
      for (int m = 0; m < 252*252; m++) {
        if (yvsx_total->GetBinContent(m) > 0) {
          npixels++;
//          nmultiple += (yvsx_total->GetBinContent(m) - 1);
        }
      }
// cout << endl << (int)yvsx_total->GetEntries() << "	" << npixels << "	" << (int)yvsx_detection->GetEntries() << "	" << (int)yvsx_crosstalk[0]->GetEntries() << "	" << yvsx_crosstalk[1]->GetEntries();
      pixels_trial[j] = (double)npixels;
      linearity_trial[j] = npixels/npe_trial[j]; 
//      linearity_trial[j] = 1.0 - (double)nmultiple/npe_trial[j]; // Hamamatsu uses Linearity = pixels fired / NPe, with NPe = incident photons * PDE, so I could remove any dependency on the multiplicity, then it doesn't matter about order or if an avalanche is cased by a cross-talk or a photon, it's just a pixel fired regardless of how many hits, real or not, are in the bin.
      pixels[i] += pixels_trial[j];
      linearity[i] += linearity_trial[j];
      npe[i] += npe_trial[j];
//      cout << endl << photons[i] << "	" << currentevent << "	" << npe_trial[j] << "	" << pixels_trial[j] << "	" << nmultiple << "	" << linearity_trial[j];
    }
    pixels[i] /= (double)NTRIALS;
    linearity[i] /= (double)NTRIALS;
    npe[i] /= (double)NTRIALS;
    for (int j = 0; j < NTRIALS; j++) {
      pixels_error[i] += (pixels_trial[j] - pixels[i])*(pixels_trial[j] - pixels[i]);
      linearity_error[i] += (linearity_trial[j] - linearity[i])*(linearity_trial[j] - linearity[i]);
      npe_error[i] += (npe_trial[j] - npe[i])*(npe_trial[j] - npe[i]);
    }
    pixels_error[i] = sqrt(pixels_error[i]/((double)NTRIALS - 1.0));
    linearity_error[i] = sqrt(linearity_error[i]/((double)NTRIALS - 1.0));
    npe_error[i] = sqrt(npe_error[i]/((double)NTRIALS - 1.0));
  }

  TGraphErrors *g_linearity, *g_npelinearity, *g_pixels, *g_npepixels;
  TGaxis *pixels_axis;

  ofstream outfile;
  outfile.open(Form("SingleClad-Linearity-CrossTalk-%s%s-%smm-%s-%s.txt",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  outfile << "Throwns	NPe	Incident_Photons_50%_PDE	Linearity	Pixels_Fired" << endl;
  for (int i = 0; i < NPHOTONS; i++) outfile << photons[i] << "	" << npe[i] << " +/- " << npe_error[i] << "	" << npe[i]*2.0 << " +/- " << npe_error[i]*2.0 << "	" << linearity[i] << " +/- " << linearity_error[i] << "	" << pixels[i] << " +/- " << pixels_error[i] << endl;
  outfile.close();

  C = new TCanvas();
  TPad *pad1 = new TPad("pad1","",0,0,1,1);
  pad1->SetLogx();
  pad1->SetGrid(0,0);
  pad1->cd();
  g_linearity = new TGraphErrors(NPHOTONS,photons,linearity,0,linearity_error);
  g_linearity->SetTitle(Form("%s Layer %s %s mm %s %s Linearity and Pixels Fired; Thrown Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_linearity->SetMarkerStyle(20); g_linearity->SetMarkerSize(1); g_linearity->SetMarkerColor(kBlue-1); g_linearity->SetLineWidth(4); g_linearity->SetLineColor(kBlue-1); g_linearity->GetYaxis()->SetLabelColor(kBlue-1); g_linearity->GetYaxis()->SetAxisColor(kBlue-1);
  g_pixels = new TGraphErrors(NPHOTONS,photons,pixels,0,pixels_error);
  g_pixels->SetTitle(";;Pixels Fired");
  g_pixels->SetMarkerStyle(20); g_pixels->SetMarkerSize(1); g_pixels->SetMarkerColor(kRed+1); g_pixels->SetLineWidth(4); g_pixels->SetLineColor(kRed+1); g_pixels->GetXaxis()->SetLabelOffset(0.02); g_pixels->GetYaxis()->SetLabelColor(kRed+1); g_pixels->GetYaxis()->SetAxisColor(kRed+1);
  g_linearity->Draw();
  C->cd();
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","",0,0,1,1);
  pad2->SetFillStyle(4000);
  pad2->SetFrameFillStyle(4000);
  pad2->cd();
  pad2->SetLogx();
  pad2->SetLogy();
//  pad2->SetGrid(1,1);
  g_pixels->Draw("APLY+X+");
  C->cd();
  pad2->Draw();

  C->Print(Form("SingleClad-Linearity-CrossTalk-%s%s-%smm-%s-%s.pdf(",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%s%s-%smm-%s-%s Linearity vs. Thrown Photons",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  C->Clear();

  C = new TCanvas();
  pad1 = new TPad("pad1","",0,0,1,1);
  pad1->SetLogx();
  pad1->SetGrid(0,0);
  pad1->cd();
  g_npelinearity = new TGraphErrors(NPHOTONS,npe,linearity,npe_error,linearity_error);
  g_npelinearity->SetTitle(Form("%s Layer %s %s mm %s %s Linearity and Pixels Fired; Detected Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_npelinearity->SetMarkerStyle(20); g_npelinearity->SetMarkerSize(1); g_npelinearity->SetMarkerColor(kBlue-1); g_npelinearity->SetLineWidth(4); g_npelinearity->SetLineColor(kBlue-1); g_npelinearity->GetYaxis()->SetLabelColor(kBlue-1); g_npelinearity->GetYaxis()->SetAxisColor(kBlue-1);
  g_npepixels = new TGraphErrors(NPHOTONS,npe,pixels,npe_error,pixels_error);
  g_npepixels->SetTitle(";;Pixels Fired");
  g_npepixels->SetMarkerStyle(20); g_npepixels->SetMarkerSize(1); g_npepixels->SetMarkerColor(kRed+1); g_npepixels->SetLineWidth(4); g_npepixels->SetLineColor(kRed+1); g_npepixels->GetXaxis()->SetLabelOffset(0.02); g_npepixels->GetYaxis()->SetLabelColor(kRed+1); g_npepixels->GetYaxis()->SetAxisColor(kRed+1);
  g_npelinearity->Draw();
  C->cd();
  pad1->Draw();
  pad2 = new TPad("pad2","",0,0,1,1);
  pad2->SetFillStyle(4000);
  pad2->SetFrameFillStyle(4000);
  pad2->cd();
  pad2->SetLogx();
  pad2->SetLogy();
  g_npepixels->Draw("APLY+X+");
  C->cd();
  pad2->Draw();

  C->Print(Form("SingleClad-Linearity-CrossTalk-%s%s-%smm-%s-%s.pdf",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%s%s-%smm-%s-%s Linearity vs. Npe",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));

  C = new TCanvas();
  for (int i = 0; i < NPHOTONS; i++) { npe[i] *= 2.0; npe_error[i] *= 2.0; }
  pad1 = new TPad("pad1","",0,0,1,1);
  pad1->SetLogx();
  pad1->SetGrid(0,0);
  pad1->cd();
  g_npelinearity = new TGraphErrors(NPHOTONS,npe,linearity,npe_error,linearity_error);
  g_npelinearity->SetTitle(Form("%s Layer %s %s mm %s %s Linearity and Pixels Fired (50%% PDE); Incident Photons; Linearity",lgshape.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
  g_npelinearity->SetMarkerStyle(20); g_npelinearity->SetMarkerSize(1); g_npelinearity->SetMarkerColor(kBlue-1); g_npelinearity->SetLineWidth(4); g_npelinearity->SetLineColor(kBlue-1); g_npelinearity->GetYaxis()->SetLabelColor(kBlue-1); g_npelinearity->GetYaxis()->SetAxisColor(kBlue-1);
  g_npepixels = new TGraphErrors(NPHOTONS,npe,pixels,npe_error,pixels_error);
  g_npepixels->SetTitle(";;Pixels Fired");
  g_npepixels->SetMarkerStyle(20); g_npepixels->SetMarkerSize(1); g_npepixels->SetMarkerColor(kRed+1); g_npepixels->SetLineWidth(4); g_npepixels->SetLineColor(kRed+1); g_npepixels->GetXaxis()->SetLabelOffset(0.02); g_npepixels->GetYaxis()->SetLabelColor(kRed+1); g_npepixels->GetYaxis()->SetAxisColor(kRed+1);
  g_npelinearity->Draw();
  C->cd();
  pad1->Draw();
  pad2 = new TPad("pad2","",0,0,1,1);
  pad2->SetFillStyle(4000);
  pad2->SetFrameFillStyle(4000);
  pad2->cd();
  pad2->SetLogx();
  pad2->SetLogy();
  g_npepixels->Draw("APLY+X+");
  C->cd();
  pad2->Draw();

  C->Print(Form("SingleClad-Linearity-CrossTalk-%s%s-%smm-%s-%s.pdf)",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()),Form("Title:%s%s-%smm-%s-%s Linearity vs. Npe (50%% PDE)",SHAPE.Data(),LAYER.Data(),LENGTH.Data(),GAP.Data(),DETECTOR.Data()));
}
