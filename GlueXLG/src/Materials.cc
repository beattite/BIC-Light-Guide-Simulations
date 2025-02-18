/*
 * Materials.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: petrstepanov
 */

#include <G4SystemOfUnits.hh>
#include <G4ios.hh>
#include <G4MaterialPropertiesTable.hh>
#include <G4NistManager.hh>
#include <G4PhysicsVector.hh>
#include <G4String.hh>
#include <G4Types.hh>
#include <stdlib.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include "Materials.hh"
#include "Elements.hh"
#include "Utils.hh"

Materials* Materials::fgInstance = nullptr;

Materials::Materials() : fMaterialsList{}
                         // fOpticalPhotonWavelength{400., 440., 480., 520., 560., 600., 640., 680.},
                         // fOpticalPhotonEnergy{}
                         // fCrystalLength(200*mm)
{
  // fMaterialsList.clear();
  // Instantiate elements database
  Elements* elements = Elements::getInstance();

  // PMMA
  G4Material* PMMA = new G4Material("PMMA", 1.18*g/cm3, 3);
  PMMA->AddElement(elements->getElement("C"), 5);
  PMMA->AddElement(elements->getElement("H"), 8);
  PMMA->AddElement(elements->getElement("O"), 2);

  // https://refractiveindex.info/?shelf=organic&book=poly%28methyl_methacrylate%29&page=Szczurowski
  std::vector<G4double> refractiveIndexPMMA = {404.70, 1.50512, 459.0, 1.49887, 506.40, 1.49504, 567.50, 1.491513, 655.70, 1.488096, 737.10, 1.485975, 859.20, 1.4838282, 988.0, 1.4823095, 1083.0, 1.48146};
  //std::vector<G4double> refractiveIndexPMMA = {100, 1.49, 1000, 1.49}; // Test with Elton's numbers
  G4MaterialPropertyVector* refractiveIndexPMMAMPV = nmToMPV(refractiveIndexPMMA);

  // Set to 240, based on Elton's old paper: https://halldweb.jlab.org/DocDB/0017/001784/003/bcal_prod_guides.pdf
  std::vector<G4double> absorptionLengthPMMA = {100, 240.*cm, 1000, 240.*cm};
  G4MaterialPropertyVector* absorptionLengthPMMAMPV = nmToMPV(absorptionLengthPMMA);

  G4MaterialPropertiesTable* PMMAMPT = new G4MaterialPropertiesTable();
  PMMAMPT->AddProperty("RINDEX", refractiveIndexPMMAMPV ,true);
  PMMAMPT->AddProperty("ABSLENGTH", absorptionLengthPMMAMPV, true);

  PMMA->SetMaterialPropertiesTable(PMMAMPT);
  fMaterialsList.push_back(PMMA);
  
  // Silicone Cookie
  G4Material* Cookie = new G4Material("Cookie", 1.03*g/cm3, 4);
  Cookie->AddElement(elements->getElement("C"), 0.355);
  Cookie->AddElement(elements->getElement("H"), 0.08);
  Cookie->AddElement(elements->getElement("O"), 0.22);
  Cookie->AddElement(elements->getElement("Si"),0.345);

  // https://eljentechnology.com/images/products/data_sheets/EJ-560.pdf
  std::vector<G4double> refractiveIndexCookie = {300, 1.43, 1000, 1.43};
  G4MaterialPropertyVector* refractiveIndexCookieMPV = nmToMPV(refractiveIndexCookie);

  // Set to 240, same as light guide
  std::vector<G4double> absorptionLengthCookie = {300, 240.*cm, 1000, 240.*cm};
  G4MaterialPropertyVector* absorptionLengthCookieMPV = nmToMPV(absorptionLengthCookie);

  G4MaterialPropertiesTable* CookieMPT = new G4MaterialPropertiesTable();
  CookieMPT->AddProperty("RINDEX", refractiveIndexCookieMPV ,true);
  CookieMPT->AddProperty("ABSLENGTH", absorptionLengthCookieMPV, true);

  Cookie->SetMaterialPropertiesTable(CookieMPT);
  fMaterialsList.push_back(Cookie);
  
  // VM2000
  G4Material* C10H8O4 = new G4Material("C10H8O4", 1.38*g/cm3, 3);
  C10H8O4->AddElement(elements->getElement("C"), 10);
  C10H8O4->AddElement(elements->getElement("H"), 8);
  C10H8O4->AddElement(elements->getElement("O"), 4);
  fMaterialsList.push_back(C10H8O4);

  // Aluminum
  G4Material* Aluminum = new G4Material("Aluminum", 2.70*g/cm3, 1);
  Aluminum->AddElement(elements->getElement("Al"), 1);
  fMaterialsList.push_back(Aluminum);

  // Frame
  // PS: why frame is made of carbon?
  G4Material* Frame = new G4Material("Frame", 1.55*g/cm3, 1);
  Frame->AddElement(elements->getElement("C"), 1);
  fMaterialsList.push_back(Frame);

  // Mu-metal
  // Wikipedia: composition is approximately 77% nickel, 16% iron, 5% copper, and 2% chromium or molybdenum
  // http://www.mu-metal.com/technical-data.html
  G4Material* MuMetal = new G4Material("MuMetal", 8.7 * g/cm3, 4);
  MuMetal->AddElement(elements->getElement("Ni"), 77 * perCent);
  MuMetal->AddElement(elements->getElement("Fe"), 16 * perCent);
  MuMetal->AddElement(elements->getElement("Cu"), 5 * perCent);
  MuMetal->AddElement(elements->getElement("Cr"), 2 * perCent);
  fMaterialsList.push_back(MuMetal);

  // ASCII font
  // https://fsymbols.com/generators/carty/

  // █░█ ▄▀█ █▀▀ █░█ █░█ █▀▄▀█
  // ▀▄▀ █▀█ █▄▄ █▄█ █▄█ █░▀░█

  // Vacuum how-to: http://hurel.hanyang.ac.kr/Geant4/Doxygen/10.03/html/d2/dd7/_d_m_x_detector_material_8icc_source.html
  {
    G4Material* material = new G4Material("vacuum", 1., 1.*g/mole, 1.e-20*g/cm3, kStateGas, 0.1*kelvin, 1.e-20*bar);
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // Refractive Index of Vacuum
    std::vector<G4double> rIndex = {200, 1., 800, 1.};
    G4MaterialPropertyVector* rIndexMPV = nmToMPV(rIndex);
    mpt->AddProperty("RINDEX", rIndexMPV, true);

    // Absorption length of Vacuum
    std::vector<G4double> absLength  = {200, 1000*km, 800, 1000*km};
    G4MaterialPropertyVector* absLengthMPV = nmToMPV(absLength);
    mpt->AddProperty("ABSLENGTH", absLengthMPV, true);

    // mpt->DumpTable();
    material->SetMaterialPropertiesTable(mpt);

    printMaterialProperties(material);
    fMaterialsList.push_back(material);
    // saveMaterial(material);
  }


  // ▄▀█ █ █▀█
  // █▀█ █ █▀▄

  //G4Material* air = new G4Material("air", 1.20479*mg/cm3, 4, kStateGas, 293*kelvin, 1*atmosphere);
  //air->AddElement(elements->getElement("C"), 0.000124);
  //air->AddElement(elements->getElement("N"), 0.755268);
  //air->AddElement(elements->getElement("O"), 0.231781);
  //air->AddElement(elements->getElement("Ar"),0.012827);
  G4Material* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();

    // Refractive Index of Air
    // https://refractiveindex.info/?shelf=other&book=air&page=Ciddor
    std::vector<G4double> rIndex = { 400, 1.00028276, 440, 1.00028091, 480, 1.00027954, 520, 1.00027848, 560, 1.00027765, 600, 1.00027698, 640, 1.00027644, 680, 1.00027600};
    //std::vector<G4double> rIndex = { 100, 1.0, 1000, 1.0}; // TEST
    G4MaterialPropertyVector* rIndexMPV = nmToMPV(rIndex);
    mpt->AddProperty("RINDEX", rIndexMPV, true);

    // Absorption length of Air
    // https://web.physik.rwth-aachen.de/~hebbeker/theses/dietz-laursonn_phd.pdf
    //std::vector<G4double> absLength  = {100, 50*m, 1000, 50*m};
    std::vector<G4double> absLength  = {100, 300*m, 1000, 300*m};
    G4MaterialPropertyVector* absLengthMPV = nmToMPV(absLength);
    mpt->AddProperty("ABSLENGTH", absLengthMPV, true);

    // mpt->DumpTable();
    air->SetMaterialPropertiesTable(mpt);
  }
  printMaterialProperties(air);
  fMaterialsList.push_back(air);
  // saveMaterial(air);

  // ▀█▀ █▀▀ █▀▀ █░░ █▀█ █▄░█
  // ░█░ ██▄ █▀░ █▄▄ █▄█ █░▀█

  G4Material* teflon = G4NistManager::Instance()->FindOrBuildMaterial("G4_TEFLON");
  {
    G4MaterialPropertiesTable *mpt = new G4MaterialPropertiesTable();
    // Refrafctive index taken from this paper for AF1601
    // https://engineering.case.edu/centers/sdle/sites/engineering.case.edu.centers.sdle/files/optical_properties_of_teflon_r_af_amorphous_fluo.pdf
    std::vector<G4double> rIndex = { 145.861, 1.399, 149.377, 1.427, 153.114, 1.430, 156.996, 1.417, 161.099, 1.399, 165.348, 1.388, 179.780, 1.364, 180.162, 1.344, 203.017, 1.328, 232.905, 1.318, 264.551, 1.312, 301.471, 1.308, 354.214, 1.305, 459.701, 1.301, 653.092, 1.297, 955.486, 1.295, 1379.190, 1.292, 1549.726, 1.292 };
    G4MaterialPropertyVector *rIndexMPV = nmToMPV(rIndex);
    mpt->AddProperty("RINDEX", rIndexMPV, true);

    std::vector<G4double> absLength = {157.902, 1/4513.216*cm, 159.285, 1/3295.154*cm, 161.524, 1/2200.441*cm, 164.224, 1/1321.586*cm, 168.636, 1/674.009*cm, 174.167, 1/303.965*cm, 181.477, 1/165.198*cm, 187.404, 1/134.361*cm, 195.108, 1/72.687*cm, 206.105, 1/41.850*cm, 220, 1/1*cm};
    G4MaterialPropertyVector* absLengthMPV = nmToMPV(absLength);
    mpt->AddProperty("ABSLENGTH", absLengthMPV, true);

    // TODO: add emission p. 321
    // https://sci-hub.do/10.1016/0168-9002(96)00286-0
    teflon->SetMaterialPropertiesTable(mpt);
  }
  printMaterialProperties(teflon);
  fMaterialsList.push_back(teflon);
  // saveMaterial(teflon);

  // █▄▄ █▀▀ ▄▄ █▄▄ ▀▀█ █▀█
  // █▄█ █▄▄ ░░ █▄█ ▄██ █▄█

  // Composition: https://www.crystals.saint-gobain.com/sites/imdf.crystals.com/files/documents/bc-630.pdf
  G4Material* bc630 = new G4Material("BC630", 1.04*g/cm3, 4, kStateLiquid);
  bc630->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 2);
  bc630->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 6);
  bc630->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 1);
  bc630->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);

  G4MaterialPropertiesTable* bc630MPT = new G4MaterialPropertiesTable();

  // Found nice article here: https://sci-hub.do/10.3390/s20216092
  std::vector<G4double> refractiveIndexBC630 = {350, 1.465, 650, 1.465};
  G4MaterialPropertyVector* refractiveIndexBC630MPV = nmToMPV(refractiveIndexBC630);
  bc630MPT->AddProperty("RINDEX", refractiveIndexBC630MPV, true);

  std::vector<G4double> absorptionLengthBC630 = {350, 1/0.004*cm,
                                                 400, 1/0.002*cm,
                                                 450, 1/0.001*cm,
                                                 650, 1/0.0005*cm};
  G4MaterialPropertyVector* absorptionLengthBC630MPV = nmToMPV(absorptionLengthBC630);
  bc630MPT->AddProperty("ABSLENGTH", absorptionLengthBC630MPV, true);
  // bc630MPT->DumpTable();
  bc630->SetMaterialPropertiesTable(bc630MPT);

  printMaterialProperties(bc630);
  fMaterialsList.push_back(bc630);
  // saveMaterial(bc630);

  // █▄▄ █▀█ █▀█ █▀█ █▀ █ █░░ █ █▀▀ ▄▀█ ▀█▀ █▀▀   █▀▀ █░░ ▄▀█ █▀ █▀
  // █▄█ █▄█ █▀▄ █▄█ ▄█ █ █▄▄ █ █▄▄ █▀█ ░█░ ██▄   █▄█ █▄▄ █▀█ ▄█ ▄█

  // ~ 90% SiO2, 10% B2O3: https://en.wikipedia.org/wiki/Borosilicate_glass
  G4Material* SiO2 = G4NistManager::Instance()->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
  G4Material* B2O3 = G4NistManager::Instance()->FindOrBuildMaterial("G4_BORON_OXIDE");
  G4Material* borosilicate = new G4Material("borosilicate", 2.23*g/cm3, 2, kStateSolid);
  borosilicate->AddMaterial(SiO2, 0.9);
  borosilicate->AddMaterial(B2O3, 0.1);

  // https://www.hamamatsu.com/resources/pdf/etd/PMT_handbook_v3aE.pdf
  // Borosilicate window with Bialkali photocathode work within 300-650 nm range, pp.35
  // https://www.hamamatsu.com/jp/en/product/optical-sensors/photodiodes/si-photodiodes/si-photodiode-faq/index.html
  // LXeDetectorConstruction: 1.49

  std::vector<G4double> refractiveIndexBorosilicate = {300, 1.49,
//                                                       650, 1.49};
  1000, 1.49};
  G4MaterialPropertyVector* refractiveIndexBorosilicateMPV = nmToMPV(refractiveIndexBorosilicate);

  // https://refractiveindex.info/?shelf=glass&book=SUMITA-BK&page=K-BK7
  // alpha = 0.0034706 cm-1 => 1/0.0034706 = 288 cm
  // LXeDetectorConstruction: 420.*cm
  std::vector<G4double> absorptionLengthBorosilicate = {300, 420.*cm,
//      650, 420.*cm};
                                                       1000, 420.*cm};
  G4MaterialPropertyVector* absorptionLengthBorosilicateMPV = nmToMPV(absorptionLengthBorosilicate);

  G4MaterialPropertiesTable* borosilicateMPT = new G4MaterialPropertiesTable();
  borosilicateMPT->AddProperty("RINDEX", refractiveIndexBorosilicateMPV ,true);
  borosilicateMPT->AddProperty("ABSLENGTH", absorptionLengthBorosilicateMPV, true);
  // borosilicateMPT->DumpTable();
  borosilicate->SetMaterialPropertiesTable(borosilicateMPT);

  printMaterialProperties(borosilicate);
  fMaterialsList.push_back(borosilicate);
  // saveMaterial(borosilicate);

  // █▀▀ █░█ █▀ █▀▀ █▀▄   █▀█ █░█ ▄▀█ █▀█ ▀█▀ ▀█   █▀▀ █░░ ▄▀█ █▀ █▀
  // █▀░ █▄█ ▄█ ██▄ █▄▀   ▀▀█ █▄█ █▀█ █▀▄ ░█░ █▄   █▄█ █▄▄ █▀█ ▄█ ▄█

  // Density found here
  // https://en.wikipedia.org/wiki/Fused_quartz#Typical_properties_of_clear_fused_silica
  G4Material* fusedSilica = new G4Material("fused-silica", 2.203*g/cm3, 2, kStateSolid);
  fusedSilica->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);
  fusedSilica->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 2);

  // Refractive index
  // https://en.wikipedia.org/wiki/Fused_quartz#Typical_properties_of_clear_fused_silica
  std::vector<G4double> refractiveIndexSilica = {300, 1.4585, 650, 1.4585};
  G4MaterialPropertyVector* refractiveIndexSilicaMPV = nmToMPV(refractiveIndexSilica);

  // Absorption length - copied from borosilicate
  std::vector<G4double> absorptionLengthSilica = {300, 420.*cm, 650, 420.*cm};
  G4MaterialPropertyVector* absorptionLengthSilicaMPV = nmToMPV(absorptionLengthSilica);

  G4MaterialPropertiesTable* silicaMPT = new G4MaterialPropertiesTable();
  silicaMPT->AddProperty("RINDEX", refractiveIndexSilicaMPV ,true);
  silicaMPT->AddProperty("ABSLENGTH", absorptionLengthSilicaMPV, true);
  // silicaMPT->DumpTable();
  fusedSilica->SetMaterialPropertiesTable(silicaMPT);

  printMaterialProperties(fusedSilica);
  fMaterialsList.push_back(fusedSilica);
  // saveMaterial(fusedSilica);

  // █▀ █ █░░ █ █▀▀ █▀█ █▄░█ █▀▀   █▀█ █▀▀ █▀ █ █▄░█
  // ▄█ █ █▄▄ █ █▄▄ █▄█ █░▀█ ██▄   █▀▄ ██▄ ▄█ █ █░▀█

  // For MPPC S13360-****CS window
  // https://en.wikipedia.org/wiki/Silicone_resin
  G4Material* siliconeResin = new G4Material("silicone-resin", 1.42*g/cm3, 2, kStateSolid);
  siliconeResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);
  siliconeResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 4);

  // Refractive index - found in Hamamatsu docs
  // https://www.hamamatsu.com/resources/pdf/ssd/s13360_series_kapd1052e.pdf
  // https://www.hamamatsu.com/us/en/product/optical-sensors/photodiodes/si-photodiodes/si-photodiode-faq/index.html
  std::vector<G4double> refractiveIndexResin = {300, 1.41, 650, 1.41};
  G4MaterialPropertyVector* refractiveIndexResinMPV = nmToMPV(refractiveIndexResin);

  // Absorption length - copied from borosilicate
  std::vector<G4double> absorptionLengthResin = {300, 420.*cm, 650, 420.*cm};
  G4MaterialPropertyVector* absorptionLengthResinMPV = nmToMPV(absorptionLengthResin);

  G4MaterialPropertiesTable* resinMPT = new G4MaterialPropertiesTable();
  resinMPT->AddProperty("RINDEX", refractiveIndexResinMPV, true);
  resinMPT->AddProperty("ABSLENGTH", absorptionLengthResinMPV, true);
  // resinMPT->DumpTable();
  siliconeResin->SetMaterialPropertiesTable(resinMPT);

  printMaterialProperties(siliconeResin);
  fMaterialsList.push_back(siliconeResin);
  // saveMaterial(siliconeResin);

  // For MPPC S13-BIC window
  // https://pubchem.ncbi.nlm.nih.gov/compound/Epoxy-resin
  G4Material* epoxyResin = new G4Material("epoxy-resin", 1.16*g/cm3, 4, kStateSolid);
  epoxyResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("C"), 21);
  epoxyResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("H"), 25);
  epoxyResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("Cl"), 1);
  epoxyResin->AddElement(G4NistManager::Instance()->FindOrBuildElement("O"), 5);

  // Refractive index - found in Hamamatsu docs
  // https://www.hamamatsu.com/resources/pdf/ssd/s13360_series_kapd1052e.pdf
  // https://www.hamamatsu.com/us/en/product/optical-sensors/photodiodes/si-photodiodes/si-photodiode-faq/index.html
  std::vector<G4double> refractiveIndexEpoxyResin = {300, 1.55, 650, 1.55};
  //std::vector<G4double> refractiveIndexEpoxyResin = {100, 1.49, 1000, 1.49}; // Test with Elton's numbers
  G4MaterialPropertyVector* refractiveIndexEpoxyResinMPV = nmToMPV(refractiveIndexEpoxyResin);

  // Absorption length - copied from borosilicate
  std::vector<G4double> absorptionLengthEpoxyResin = {100, 420.*cm, 1000, 420.*cm};
  G4MaterialPropertyVector* absorptionLengthEpoxyResinMPV = nmToMPV(absorptionLengthEpoxyResin);

  G4MaterialPropertiesTable* epoxyresinMPT = new G4MaterialPropertiesTable();
  epoxyresinMPT->AddProperty("RINDEX", refractiveIndexEpoxyResinMPV, true);
  epoxyresinMPT->AddProperty("ABSLENGTH", absorptionLengthEpoxyResinMPV, true);
  // epoxyresinMPT->DumpTable();
  epoxyResin->SetMaterialPropertiesTable(epoxyresinMPT);

  printMaterialProperties(epoxyResin);
  fMaterialsList.push_back(epoxyResin);
  // saveMaterial(epoxyResin);

  // █▄▄ █ ▄▀█ █░░ █▄▀ ▄▀█ █░░ █
  // █▄█ █ █▀█ █▄▄ █░█ █▀█ █▄▄ █

  // Technically PMT Bialkali photocathode has stainless steel substrate
  // https://sci-hub.do/https://doi.org/10.1016/S0168-9002(96)00809-1
  // https://en.wikipedia.org/wiki/Photocathode
  G4Material* bialkaliCathode = new G4Material("bialkali", 3*g/cm3, 3, kStateSolid);
  bialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Sb"), 1);
  bialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Rb"), 1);
  bialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Cs"), 1);

  G4MaterialPropertiesTable* bialkaliMPT = new G4MaterialPropertiesTable();
  // Use same refraction index as the borosilicate glass (window) like here: https://core.ac.uk/download/pdf/276263522.pdf
  std::vector<G4double> refractiveIndexBialkali = {300, 1.49,
                                                   650, 1.49};
  G4MaterialPropertyVector* refractiveIndexBialkaliMPV = nmToMPV(refractiveIndexBialkali);

  bialkaliMPT->AddProperty("RINDEX", refractiveIndexBialkaliMPV, true);
  bialkaliCathode->SetMaterialPropertiesTable(bialkaliMPT);

  printMaterialProperties(bialkaliCathode);
  fMaterialsList.push_back(bialkaliCathode);
  // saveMaterial(bialkaliCathode);

  // █▀▄▀█ █░█ █░░ ▀█▀ █ ▄▀█ █░░ █▄▀ ▄▀█ █░░ █
  // █░▀░█ █▄█ █▄▄ ░█░ █ █▀█ █▄▄ █░█ █▀█ █▄▄ █

  // multialkali, page 4 here SbNa2KCs
  // https://psec.uchicago.edu/library/photomultipliers/Photonis_PMT_basics.pdf
  G4Material* multialkaliCathode = new G4Material("multialkali", 3*g/cm3, 4, kStateSolid);
  multialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Sb"), 1);
  multialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Na"), 2);
  multialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("K"), 1);
  multialkaliCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Cs"), 1);

  G4MaterialPropertiesTable* multialkaliMPT = new G4MaterialPropertiesTable();
  // Use same refraction index as the borosilicate glass (window) like here: https://core.ac.uk/download/pdf/276263522.pdf
  // And R2257 with multialkali photocathode has borosilicate window: https://www.hamamatsu.com/resources/pdf/etd/R2257_TPMH1141E.pdf
  std::vector<G4double> refractiveIndexMultialkali = {300, 1.49, 650, 1.49};
  G4MaterialPropertyVector* refractiveIndexMultialkaliMPV = nmToMPV(refractiveIndexMultialkali);

  multialkaliMPT->AddProperty("RINDEX", refractiveIndexMultialkaliMPV ,true);
  multialkaliCathode->SetMaterialPropertiesTable(multialkaliMPT);

  printMaterialProperties(multialkaliCathode);
  fMaterialsList.push_back(multialkaliCathode);
  // saveMaterial(multialkaliCathode);

  // █▀ █ █░░ █ █▀▀ █▀█ █▄░█ ▄▄ █▀▀ █▀█ █▄█ █▀ ▀█▀ ▄▀█ █░░
  // ▄█ █ █▄▄ █ █▄▄ █▄█ █░▀█ ░░ █▄▄ █▀▄ ░█░ ▄█ ░█░ █▀█ █▄▄

  // Some info here: https://www.hamamatsu.com/resources/pdf/ssd/mppc_kapd9005e.pdf
  // Density taken from wikipedia: https://en.wikipedia.org/wiki/Silicon
  G4Material* siCathode = new G4Material("silicon-crystal", 2.32*g/cm3, 1, kStateSolid);
  siCathode->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    // Use same refraction index as the silicone resin (S13360-...) like here: https://core.ac.uk/download/pdf/276263522.pdf
    std::vector<G4double> ri = {300, 1.41, 650, 1.41};
    G4MaterialPropertyVector* riMPV = nmToMPV(ri);

    mpt->AddProperty("RINDEX", riMPV, true);
    siCathode->SetMaterialPropertiesTable(mpt);
  }

  printMaterialProperties(siCathode);
  fMaterialsList.push_back(siCathode);
  // saveMaterial(siCathode);
  
  G4Material* siCathodeS13 = new G4Material("silicon-crystal-S13", 2.32*g/cm3, 1, kStateSolid);
  siCathodeS13->AddElement(G4NistManager::Instance()->FindOrBuildElement("Si"), 1);
  {
    G4MaterialPropertiesTable* mpt = new G4MaterialPropertiesTable();
    // Use same refraction index as the S13-BIC epoxy resin
    std::vector<G4double> ri = {100, 1.55, 1000, 1.55};
    //std::vector<G4double> ri = {100, 1.49, 1000, 1.49}; // TEST
    G4MaterialPropertyVector* riMPV = nmToMPV(ri);

    mpt->AddProperty("RINDEX", riMPV, true);
    siCathodeS13->SetMaterialPropertiesTable(mpt);
  }

  printMaterialProperties(siCathodeS13);
  fMaterialsList.push_back(siCathodeS13);
  // saveMaterial(siCathodeS13);
}

Materials::~Materials() {
  // TODO Auto-generated destructor stub
}

G4Material* Materials::getMaterial(const char* materialId){
  for (int i=0; i<(int)fMaterialsList.size(); i++){
    if (fMaterialsList[i]->GetName() == materialId){
      return fMaterialsList[i];
    }
  }
  G4cout << "ERROR: Materials::getMaterial material " << materialId << " not found." << G4endl;
  return NULL;
}

typedef std::map< G4int, G4MaterialPropertyVector*,
                  std::less<G4int> >::const_iterator MPiterator;

void Materials::printMaterialProperties(G4Material* material){
  std::cout << "\nMaterial name: " << material->GetName() << std::endl;
  G4MaterialPropertiesTable* mpt = material->GetMaterialPropertiesTable();
  if (!mpt) return;


  std::vector<G4double> fOpticalPhotonWavelength = {360, 400., 440., 480., 520., 560., 600., 640., 680., 720., 760};

  // Print photon wavelengths
  std::cout << std::left << std::setw(TAB_COLUMN_1) << "PHOTON_WAVELEGTH, nm";
  for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++){
    std::cout << std::left << std::setw(TAB_COLUMN) << fOpticalPhotonWavelength[i];
  }

  std::cout << std::endl;

  // Print photon energy
  std::cout << std::left << std::setw(TAB_COLUMN_1) << "PHOTON_ENERGY, eV";
  for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++){
    std::cout << std::left << std::setw(TAB_COLUMN) << wavelengthNmToEnergy(fOpticalPhotonWavelength[i])/eV;
  }
  std::cout << std::endl;

  // Print material properties
  std::vector<G4String> propertyNames = mpt->GetMaterialPropertyNames();

  // Replaced in Geant4 v.11 with const std::vector<G4MaterialPropertyVector*>& GetProperties() const
  //const std::map<G4int, G4MaterialPropertyVector*, std::less<G4int> >* pMapNew = mpt->GetPropertyMap();
  std::vector<G4String> materialPropertiesNames = mpt->GetMaterialPropertyNames();

  for(G4String propertyName : materialPropertiesNames){
    G4MaterialPropertyVector* property = mpt->GetProperty(propertyName);
    if (!property) continue;

    // Print property name
    std::cout << std::left << std::setw(TAB_COLUMN_1) << propertyName;
    // Print property values
    for (G4int i = 0; i < (G4int)fOpticalPhotonWavelength.size(); i++){
      G4bool b;
      G4double value = property->GetValue(wavelengthNmToEnergy(fOpticalPhotonWavelength[i]), b);
      std::cout << std::left << std::setw(TAB_COLUMN) << value;
    }

    std::cout << std::endl;
  }

  // Print material constant properties
  std::vector<G4String> constPropertyNames = mpt->GetMaterialConstPropertyNames();
  for(G4String constPropertyName : constPropertyNames){
    if (!mpt->ConstPropertyExists(constPropertyName)) continue;
    // Print property name and value
    G4double constPropertyValue = mpt->GetConstProperty(constPropertyName);
    std::cout << std::left << std::setw(TAB_COLUMN_1) << constPropertyName << constPropertyValue << std::endl;
  }
}

void Materials::printMaterialProperties(const char* materialId){
  G4Material* material = getMaterial(materialId);
  if (material == NULL) return;
  printMaterialProperties(material);
}

Materials* Materials::getInstance() {
  static Materials materials;
  if (fgInstance == nullptr)
  {
    fgInstance = &materials;
  }
  return fgInstance;
}

G4MaterialPropertyVector* Materials::evToMPV(std::vector<G4double> digitizedArray){
  assert(digitizedArray.size()%2 == 0);
  G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector(); // true for spline
  for (G4int i = 0; i < (G4int)digitizedArray.size(); i+=2){
   G4double energy = digitizedArray[i]*eV; // E (eV) = 1239.8 / l (nm)
   G4double value = digitizedArray[i+1];
   mpv->InsertValues(energy, value);
  }
  return mpv;
}

G4MaterialPropertyVector* Materials::nmToMPV(std::vector<G4double> digitizedArrayNm){
  assert(digitizedArrayNm.size()%2 == 0);
  G4MaterialPropertyVector* mpv = new G4MaterialPropertyVector(); // true for spline
  // for (G4int i = 0; i < (G4int)digitizedArrayNm.size(); i+=2){
  for (G4int i = (G4int)digitizedArrayNm.size()-2; i >=0 ; i-=2){
   G4double hc = 1239.84193;
   G4double energy = hc/digitizedArrayNm[i]*eV; // E (eV) = 1239.8 / l (nm)
   G4double value = digitizedArrayNm[i+1];
   mpv->InsertValues(energy, value);
  }
  return mpv;
}

// PS: when having a static std::vector member
//std::vector<G4Material*> Materials::fMaterialsList = {};

G4double Materials::energyToWavelangthNm(G4double energy) {
  G4double hc = 1239.84193;
  return hc*eV/energy;
}

G4double Materials::wavelengthNmToEnergy(G4double wavelength){
  G4double hc = 1239.84193;
  return hc/wavelength*eV; // E (eV) = 1239.8 / l (nm)
}

//TGraph* Materials::drawMPV(G4MaterialPropertyVector *mpv) {
//  // G4double eMin = mpv->GetMinLowEdgeEnergy();
//  // G4double eMax = mpv->GetMaxLowEdgeEnergy();
//  TGraph* graph = new TGraph();
//  // G4int i = 0;
//  // for (G4int i = (G4int)mpv->GetVectorLength()-1; i >=0 ; i--){
//  G4int vectorLength = (G4int)mpv->GetVectorLength();
//  for (G4int i=0; i<vectorLength; i++){
//    G4double e = mpv->GetLowEdgeEnergy(i);
//    G4double wavelength = energyToWavelangthNm(e);
//
//    G4bool b;
//    G4double value = mpv->GetValue(e, b);
//
//    graph->AddPoint(wavelength, value);
//  }
//  return graph;
//}
