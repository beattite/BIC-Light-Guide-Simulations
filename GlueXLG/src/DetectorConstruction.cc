//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "Materials.hh"
#include <G4NistManager.hh>
#include "detectors/DetectorProvider.hh"
#include "detectors/AbsMPPC.hh"

#include <G4RunManager.hh>
//#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4Cons.hh>
#include <G4Orb.hh>
#include <G4Sphere.hh>
#include <G4Trd.hh>
#include <G4SubtractionSolid.hh>
#include <G4IntersectionSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4PVPlacement.hh>
#include <G4VPhysicalVolume.hh>
#include <G4Trap.hh>
#include <G4GenericTrap.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <G4OpticalSurface.hh>
#include <G4LogicalSkinSurface.hh>
#include <G4RotationMatrix.hh>
#include <G4LogicalBorderSurface.hh>
#include <G4VisAttributes.hh>

#include <algorithm>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::TrapParamAdjustment(const double (&in)[11], double (&out)[11])
{
  // get input
  double dy1 = in[3], dx1 = in[4], dx2 = in[5], alp1 = in[6];
  double dy2 = in[7], dx3 = in[8], dx4 = in[9], alp2 = in[10];

  // adjust alpha
  //alp1 = alp2 = (alp1 + alp2)/2.;
  alp1 = alp2 = std::atan((std::tan(alp1) + std::tan(alp2))/2.);

  // adjust dx
  double k1 = (dx1 - dx2)/(dy1 + dy1);
  double k2 = (dx3 - dx4)/(dy2 + dy2) ;
  double k  = (k1 + k2)/2.;
  double m1 = (dx1 + dx2)/2.;
  double m2 = (dx3 + dx4)/2.;
  dx1 = m1 + k*dy1; dx2 = m1 - k*dy1; 
  dx3 = m2 + k*dy2; dx4 = m2 - k*dy2; 

  // set output
  out[0] = in[0]; out[1] = in[1]; out[2] = in[2];
  out[3] = in[3]; out[4] = dx1; out[5] = dx2; out[6]  = alp1;
  out[7] = in[7]; out[8] = dx3; out[9] = dx4; out[10] = alp2;
}

DetectorConstruction::DetectorConstruction() {
  // Initialize default user parameter values
  lightguideSizeAbottom = DetectorMessenger::lightguideSizeAbottomCmdDefaultValue;
  lightguideSizeAtop = DetectorMessenger::lightguideSizeAtopCmdDefaultValue;
  lightguideSizeAheight = DetectorMessenger::lightguideSizeAheightCmdDefaultValue;
  lightguideSizeBbottom = DetectorMessenger::lightguideSizeBbottomCmdDefaultValue;
  lightguideSizeBtop = DetectorMessenger::lightguideSizeBtopCmdDefaultValue;
  lightguideSizeBheight = DetectorMessenger::lightguideSizeBheightCmdDefaultValue;
  lightguideLength = DetectorMessenger::lightguideLengthCmdDefaultValue;
  detectorType = DetectorMessenger::detectorTypeCmdDefaultValue;
  lightguideShape = DetectorMessenger::lightguideShapeCmdDefaultValue;
  hasWrap = DetectorMessenger::hasWrapCmdDefaultValue;
  hasGrease = DetectorMessenger::hasGreaseCmdDefaultValue;
  hasCookie = DetectorMessenger::hasCookieCmdDefaultValue;

  // Initialize default values
  wrapThickness = 65 * micrometer;
  greaseThickness = 0.3 * mm;
  cookieThickness = 1.0 * mm;
  airThickness = 0.5 * mm;

  // Initialize private objects
  detector = DetectorProvider::getDetector(detectorType);

  // Initialize the detector messenger
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
}

// Setters for geometry parameters

void DetectorConstruction::setLightGuideSizeAbottom(G4double value) {
  lightguideSizeAbottom = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideSizeAtop(G4double value) {
  lightguideSizeAtop = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideSizeAheight(G4double value) {
  lightguideSizeAheight = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideSizeBbottom(G4double value) {
  lightguideSizeBbottom = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideSizeBtop(G4double value) {
  lightguideSizeBtop = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideSizeBheight(G4double value) {
  lightguideSizeBheight = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideLength(G4double value) {
  lightguideLength = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setHasWrap(G4bool value) {
  hasWrap = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setHasGrease(G4bool value) {
  hasGrease = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setHasCookie(G4bool value) {
  hasCookie = value;
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setDetectorType(G4String value) {
  detectorType = value;
  detector = DetectorProvider::getDetector(detectorType);
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

void DetectorConstruction::setLightGuideShape(G4String value) {
  lightguideShape = value;
  if (lightguideShape == "g1") {
    lightguideSizeAbottom = 20.96 * mm;
    lightguideSizeAtop = 21.64 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g2") {
    lightguideSizeAbottom = 21.64 * mm;
    lightguideSizeAtop = 22.30 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g3") {
    lightguideSizeAbottom = 22.30 * mm;
    lightguideSizeAtop = 22.99 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g4") {
    lightguideSizeAbottom = 22.99 * mm;
    lightguideSizeAtop = 23.65 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g5") {
    lightguideSizeAbottom = 23.67 * mm;
    lightguideSizeAtop = 24.33 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g6") {
    lightguideSizeAbottom = 24.33 * mm;
    lightguideSizeAtop = 25.02 * mm;
    lightguideSizeAheight = 20.57 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g7") {
    lightguideSizeAbottom = 25.02 * mm;
    lightguideSizeAtop = 25.83 * mm;
    lightguideSizeAheight = 24.64 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g8") {
    lightguideSizeAbottom = 25.83 * mm;
    lightguideSizeAtop = 26.62 * mm;
    lightguideSizeAheight = 24.64 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g9") {
    lightguideSizeAbottom = 26.64 * mm;
    lightguideSizeAtop = 27.43 * mm;
    lightguideSizeAheight = 24.64 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "g10") {
    lightguideSizeAbottom = 27.46 * mm;
    lightguideSizeAtop = 27.84 * mm;
    lightguideSizeAheight = 24.64 * mm;
    lightguideSizeBbottom = 13.19 * mm;
    lightguideSizeBtop = 13.61 * mm;
    lightguideSizeBheight = 13.19 * mm;
  }
  else if (lightguideShape == "b1" || lightguideShape == "bh1") {
    lightguideSizeAbottom = 20.691 * mm;
    lightguideSizeAtop = 23.505 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b2" || lightguideShape == "bh2") {
    lightguideSizeAbottom = 21.707 * mm;
    lightguideSizeAtop = 24.521 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b3" || lightguideShape == "bh3") {
    lightguideSizeAbottom = 22.723 * mm;
    lightguideSizeAtop = 25.537 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b4" || lightguideShape == "bh4") {
    lightguideSizeAbottom = 23.739 * mm;
    lightguideSizeAtop = 26.553 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b5" || lightguideShape == "bh5") {
    lightguideSizeAbottom = 24.755 * mm;
    lightguideSizeAtop = 27.569 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b6" || lightguideShape == "bh6") {
    lightguideSizeAbottom = 25.801 * mm;
    lightguideSizeAtop = 28.504 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b7" || lightguideShape == "bh7") {
    lightguideSizeAbottom = 26.360 * mm;
    lightguideSizeAtop = 29.063 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b8" || lightguideShape == "bh8") {
    lightguideSizeAbottom = 26.893 * mm;
    lightguideSizeAtop = 29.597 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b9" || lightguideShape == "bh9") {
    lightguideSizeAbottom = 27.452 * mm;
    lightguideSizeAtop = 30.155 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b10" || lightguideShape == "bh10") {
    lightguideSizeAbottom = 28.011 * mm;
    lightguideSizeAtop = 30.714 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b11" || lightguideShape == "bh11") {
    lightguideSizeAbottom = 28.544 * mm;
    lightguideSizeAtop = 31.248 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "b12" || lightguideShape == "bh12") {
    lightguideSizeAbottom = 29.103 * mm;
    lightguideSizeAtop = 31.806 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBtop = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
  }
  else if (lightguideShape == "bm1") {
    lightguideSizeAbottom = 9.404869 * mm;
    lightguideSizeAtop = 10.81211 * mm;
    lightguideSizeAheight = 10.7315 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm2") {
    lightguideSizeAbottom = 9.866687 * mm;
    lightguideSizeAtop = 11.27393 * mm;
    lightguideSizeAheight = 10.7315 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm3") {
    lightguideSizeAbottom = 10.32851 * mm;
    lightguideSizeAtop = 11.73575 * mm;
    lightguideSizeAheight = 10.7315 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm4") {
    lightguideSizeAbottom = 10.79032 * mm;
    lightguideSizeAtop = 12.19756 * mm;
    lightguideSizeAheight = 10.7315 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm5") {
    lightguideSizeAbottom = 11.25214 * mm;
    lightguideSizeAtop = 12.65938 * mm;
    lightguideSizeAheight = 10.7315 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm6") {
    lightguideSizeAbottom = 11.73791 * mm;
    lightguideSizeAtop = 13.07908 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm7") {
    lightguideSizeAbottom = 11.982 * mm;
    lightguideSizeAtop = 13.33317 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm8") {
    lightguideSizeAbottom = 12.224 * mm;
    lightguideSizeAtop = 13.57617 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm9") {
    lightguideSizeAbottom = 12.47836 * mm;
    lightguideSizeAtop = 13.82953 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm10") {
    lightguideSizeAbottom = 12.73245 * mm;
    lightguideSizeAtop = 14.08362 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm11") {
    lightguideSizeAbottom = 12.97445 * mm;
    lightguideSizeAtop = 14.32662 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bm12") {
    lightguideSizeAbottom = 13.22882 * mm;
    lightguideSizeAtop = 14.57998 * mm;
    lightguideSizeAheight = 10.3125 * mm;
    lightguideSizeBbottom = 6.0 * mm;
    lightguideSizeBtop = 6.0 * mm;
    lightguideSizeBheight = 6.0 * mm;
  }
  else if (lightguideShape == "bt1") {
    lightguideSizeAbottom = 20.691 * mm;
    lightguideSizeAtop = 23.505 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt2") {
    lightguideSizeAbottom = 21.707 * mm;
    lightguideSizeAtop = 24.521 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt3") {
    lightguideSizeAbottom = 22.723 * mm;
    lightguideSizeAtop = 25.537 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt4") {
    lightguideSizeAbottom = 23.739 * mm;
    lightguideSizeAtop = 26.553 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt5") {
    lightguideSizeAbottom = 24.755 * mm;
    lightguideSizeAtop = 27.569 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt6") {
    lightguideSizeAbottom = 25.801 * mm;
    lightguideSizeAtop = 28.504 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt7") {
    lightguideSizeAbottom = 26.360 * mm;
    lightguideSizeAtop = 29.063 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt8") {
    lightguideSizeAbottom = 26.893 * mm;
    lightguideSizeAtop = 29.597 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt9") {
    lightguideSizeAbottom = 27.452 * mm;
    lightguideSizeAtop = 30.155 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt10") {
    lightguideSizeAbottom = 28.011 * mm;
    lightguideSizeAtop = 30.714 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt11") {
    lightguideSizeAbottom = 28.544 * mm;
    lightguideSizeAtop = 31.248 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bt12") {
    lightguideSizeAbottom = 29.103 * mm;
    lightguideSizeAtop = 31.806 * mm;
    lightguideSizeAheight = 20.625 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg1") { // These are layer 5, 1 through 6 here are various alterations
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 12.8 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg2") {
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg3") {
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.2 * mm;
    lightguideSizeBheight = 13.0 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg4") {
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 12.8 * mm;
    lightguideSizeBheight = 13.2 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg5") {
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.0 * mm;
    lightguideSizeBheight = 13.2 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else if (lightguideShape == "bg6") {
    lightguideSizeAbottom = 25.8806 * mm;
    lightguideSizeAtop = 26.4434 * mm;
    lightguideSizeAheight = 21.463 * mm;
    lightguideSizeBbottom = 13.2 * mm;
    lightguideSizeBheight = 13.2 * mm;
    lightguideSizeBtop = (lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom;
  }
  else{
    lightguideShape = "";
  }
  G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct() {
  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;
  if (hasGrease && hasCookie) this->setHasCookie(false); // Possibly pointless check just in case I missed something

  // █░█░█ █▀█ █▀█ █░░ █▀▄
  // ▀▄▀▄▀ █▄█ █▀▄ █▄▄ █▄▀
  // https://fsymbols.com/generators/carty/

  G4double assemblyLength = lightguideLength + (hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getZSize();
  G4double maxXY = std::max(lightguideSizeAtop, detector->getXYSize());
  G4double worldSizeXY = maxXY;
  G4double worldSizeZ = assemblyLength;
  worldSizeXY *= 4.;
  worldSizeZ *= 2.;
  G4Material* worldMaterial = Materials::getInstance()->getMaterial("G4_AIR");
  G4Box *worldSolid = new G4Box("World" /* name */, 0.5 * worldSizeXY /* size x */, 0.5 * worldSizeXY /* size y */,
                                0.5 * worldSizeZ /* size z */);
  G4LogicalVolume *worldLogical = new G4LogicalVolume(worldSolid /* its solid */, worldMaterial /* its material */,
                                                      "World" /* its name */);
  G4VPhysicalVolume *worldPhysical = new G4PVPlacement(0 /* rotation */, G4ThreeVector() /* position */,
                                                       worldLogical /* logical volume */, "World" /* name */,
                                                       0 /* parent logical */, false /* boolean operation */,
                                                       0 /* copy number */, checkOverlaps);

  // Calculate position of the front assembly edge along Z axis
  G4double z0 = -assemblyLength / 2;

  // █░░ █ █▀▀ █░█ ▀█▀   █▀▀ █░█ █ █▀▄ █▀▀
  // █▄▄ █ █▄█ █▀█ ░█░   █▄█ █▄█ █ █▄▀ ██▄
  if (lightguideShape == "b1" || lightguideShape == "b2" || lightguideShape == "b3" || lightguideShape == "b4" || lightguideShape == "b5" || lightguideShape == "b6" || lightguideShape == "b7" || lightguideShape == "b8" || lightguideShape == "b9" || lightguideShape == "b10" || lightguideShape == "b11" || lightguideShape == "b12" || lightguideShape == "bm1" || lightguideShape == "bm2" || lightguideShape == "bm3" || lightguideShape == "bm4" || lightguideShape == "bm5" || lightguideShape == "bm6" || lightguideShape == "bm7" || lightguideShape == "bm8" || lightguideShape == "bm9" || lightguideShape == "bm10" || lightguideShape == "bm11" || lightguideShape == "bm12") {
    std::vector<G4TwoVector> vertices;
    vertices.push_back(G4TwoVector(-lightguideSizeAtop/2.,lightguideSizeAheight/2.));
    vertices.push_back(G4TwoVector(lightguideSizeAtop/2.,lightguideSizeAheight/2.));
    vertices.push_back(G4TwoVector(lightguideSizeAbottom/2.,-lightguideSizeAheight/2.));
    vertices.push_back(G4TwoVector(-lightguideSizeAbottom/2.,-lightguideSizeAheight/2.));
    vertices.push_back(G4TwoVector(-lightguideSizeBtop/2.,lightguideSizeBheight/2.));
    vertices.push_back(G4TwoVector(lightguideSizeBtop/2.,lightguideSizeBheight/2.));
    vertices.push_back(G4TwoVector(lightguideSizeBbottom/2.,-lightguideSizeBheight/2.));
    vertices.push_back(G4TwoVector(-lightguideSizeBbottom/2.,-lightguideSizeBheight/2.));
    G4GenericTrap *lightGuideSolid = new G4GenericTrap("lightGuideSolid", lightguideLength / 2., vertices);
    G4Material *lightGuideMaterial = Materials::getInstance()->getMaterial("PMMA");

    G4LogicalVolume *lightGuideLogical = new G4LogicalVolume(lightGuideSolid, lightGuideMaterial, "lightGuideLogical");

    G4ThreeVector lightGuidePosition = G4ThreeVector(0, 0, z0 + lightguideLength / 2.);
    G4VPhysicalVolume *lightGuidePhysical = new G4PVPlacement(0 /* rotation */, lightGuidePosition /* position */,
                                                              lightGuideLogical /* logical volume */,
                                                              "lightGuide" /* name */,
                                                              worldLogical /* parent logical */,
                                                              false /* boolean operation */, 0 /* copy number */,
                                                              checkOverlaps);
    // Set LG volume (for Stepping Action)
    fLGVolume = lightGuideLogical;

    // WRAP
    if (hasWrap) {
    std::vector<G4TwoVector> verticesA;
    verticesA.push_back(G4TwoVector(-lightguideSizeAtop/2.-wrapThickness,lightguideSizeAheight/2.+wrapThickness));
    verticesA.push_back(G4TwoVector(lightguideSizeAtop/2.+wrapThickness,lightguideSizeAheight/2.+wrapThickness));
    verticesA.push_back(G4TwoVector(lightguideSizeAbottom/2.+wrapThickness,-lightguideSizeAheight/2.-wrapThickness));
    verticesA.push_back(G4TwoVector(-lightguideSizeAbottom/2.-wrapThickness,-lightguideSizeAheight/2.-wrapThickness));
    verticesA.push_back(G4TwoVector(-lightguideSizeBtop/2.-wrapThickness,lightguideSizeBheight/2.+wrapThickness));
    verticesA.push_back(G4TwoVector(lightguideSizeBtop/2.+wrapThickness,lightguideSizeBheight/2.+wrapThickness));
    verticesA.push_back(G4TwoVector(lightguideSizeBbottom/2.+wrapThickness,-lightguideSizeBheight/2.-wrapThickness));
    verticesA.push_back(G4TwoVector(-lightguideSizeBbottom/2.-wrapThickness,-lightguideSizeBheight/2.-wrapThickness));
    G4GenericTrap *lightGuideWrapASolid = new G4GenericTrap("lightGuideWrapASolid", lightguideLength / 2., verticesA);
    // Tip: added 1 nanometer because "Stuck Track: potential geometry or navigation problem." between "lightGuideWrap" and "grease"
    std::vector<G4TwoVector> verticesB;
    verticesB.push_back(G4TwoVector(-lightguideSizeAtop/2./*-1*nanometer*/,lightguideSizeAheight/2./*+1*nanometer*/));
    verticesB.push_back(G4TwoVector(lightguideSizeAtop/2./*+1*nanometer*/,lightguideSizeAheight/2./*+1*nanometer*/));
    verticesB.push_back(G4TwoVector(lightguideSizeAbottom/2./*+1*nanometer*/,-lightguideSizeAheight/2./*-1*nanometer*/));
    verticesB.push_back(G4TwoVector(-lightguideSizeAbottom/2./*-1*nanometer*/,-lightguideSizeAheight/2./*-1*nanometer*/));
    verticesB.push_back(G4TwoVector(-lightguideSizeBtop/2./*-1*nanometer*/,lightguideSizeBheight/2./*+1*nanometer*/));
    verticesB.push_back(G4TwoVector(lightguideSizeBtop/2./*+1*nanometer*/,lightguideSizeBheight/2./*+1*nanometer*/));
    verticesB.push_back(G4TwoVector(lightguideSizeBbottom/2./*+1*nanometer*/,-lightguideSizeBheight/2./*-1*nanometer*/));
    verticesB.push_back(G4TwoVector(-lightguideSizeBbottom/2./*-1*nanometer*/,-lightguideSizeBheight/2./*-1*nanometer*/));
    G4GenericTrap *lightGuideWrapBSolid = new G4GenericTrap("lightGuideWrapBSolid", lightguideLength / 2., verticesB);
    
    G4RotationMatrix *noRotation = new G4RotationMatrix();
    G4SubtractionSolid *lightGuideWrapSolid = new G4SubtractionSolid("lightGuideWrapSolid", lightGuideWrapASolid,
                                                                     lightGuideWrapBSolid, noRotation,
                                                                     G4ThreeVector(0, 0, 0));
                                                                     
    G4Material *wrapMaterial = Materials::getInstance()->getMaterial("C10H8O4");
    G4LogicalVolume *lightGuideWrapLogical = new G4LogicalVolume(lightGuideWrapSolid, wrapMaterial,
                                                                 "lightGuideWrapLogical");
    G4VPhysicalVolume *lightGuideWrapPhysical = new G4PVPlacement(0 /* rotation */, lightGuidePosition /* position */,
                                                                  lightGuideWrapLogical /* logical volume */,
                                                                  "lightGuideWrap" /* name */,
                                                                  worldLogical /* parent logical */,
                                                                  false /* boolean operation */, 0 /* copy number */,
                                                                  checkOverlaps);
    G4VisAttributes *wrapLogicalVisAttr = new G4VisAttributes();
    lightGuideWrapLogical->SetVisAttributes(wrapLogicalVisAttr);
    wrapLogicalVisAttr->SetForceWireframe();
    addReflectiveBorder(lightGuidePhysical, lightGuideWrapPhysical);
    }
  }
  else if (lightguideShape == "bh1" || lightguideShape == "bh2" || lightguideShape == "bh3" || lightguideShape == "bh4" || lightguideShape == "bh5" || lightguideShape == "bh6" || lightguideShape == "bh7" || lightguideShape == "bh8" || lightguideShape == "bh9" || lightguideShape == "bh10" || lightguideShape == "bh11" || lightguideShape == "bh12") { // Hexagonal light guides
    in[0] = lightguideLength / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeAheight / 2.; in[4] = lightguideSizeAbottom / 2.; in[5] = lightguideSizeAtop / 2.; in[6] = 0; in[7] = lightguideSizeBheight / 2.; in[8] = lightguideSizeBbottom / 2.; in[9] = ((lightguideSizeAtop-lightguideSizeAbottom)/lightguideSizeAheight*lightguideSizeBheight + lightguideSizeBbottom) / 2.; in[10] = 0;
    TrapParamAdjustment(in, out);
    lightguideSizeAbottom = out[4]*2.; lightguideSizeAtop = out[5]*2.; lightguideSizeBbottom = out[8]*2.; lightguideSizeBtop = out[8]*2.; // Update to adjusted dimensions for particle gun generation
    G4Trap *lightGuideASolid = new G4Trap("lightGuideASolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    G4Trap *lightGuideBSolid = new G4Trap("lightGuideBSolid", out[0], out[1], out[2], out[3], out[5], out[5], out[6], out[7], out[8], out[8], out[10]);
    G4RotationMatrix *noRotation = new G4RotationMatrix();
    G4IntersectionSolid *lightGuideSolid = new G4IntersectionSolid("lightGuideSolid", lightGuideASolid,
                                                                     lightGuideBSolid, noRotation,
                                                                     G4ThreeVector(0, 0, 0));

    G4Material *lightGuideMaterial = Materials::getInstance()->getMaterial("PMMA");
    G4LogicalVolume *lightGuideLogical = new G4LogicalVolume(lightGuideSolid, lightGuideMaterial, "lightGuideLogical");

    G4ThreeVector lightGuidePosition = G4ThreeVector(0, 0, z0 + lightguideLength / 2.);
    G4VPhysicalVolume *lightGuidePhysical = new G4PVPlacement(0 /* rotation */, lightGuidePosition /* position */,
                                                              lightGuideLogical /* logical volume */,
                                                              "lightGuide" /* name */,
                                                              worldLogical /* parent logical */,
                                                              false /* boolean operation */, 0 /* copy number */,
                                                              checkOverlaps);
    // Set LG volume (for Stepping Action)
    fLGVolume = lightGuideLogical;
    
    // WRAP (I'm not coding this right now, I don't think we need it)
  }
  else { // Normal trapezoidal light guides
    // LIGHT GUIDE
    in[0] = lightguideLength / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeAheight / 2.; in[4] = lightguideSizeAbottom / 2.; in[5] = lightguideSizeAtop / 2.; in[6] = 0; in[7] = lightguideSizeBheight / 2.; in[8] = lightguideSizeBbottom / 2.; in[9] = lightguideSizeBtop / 2.; in[10] = 0;
    TrapParamAdjustment(in, out);
    lightguideSizeAbottom = out[4]*2.; lightguideSizeAtop = out[5]*2.; lightguideSizeBbottom = out[8]*2.; lightguideSizeBtop = out[9]*2.; // Update to adjusted dimensions for particle gun generation
    G4Trap *lightGuideSolid = new G4Trap("lightGuideSolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    G4Material *lightGuideMaterial = Materials::getInstance()->getMaterial("PMMA");

    G4LogicalVolume *lightGuideLogical = new G4LogicalVolume(lightGuideSolid, lightGuideMaterial, "lightGuideLogical");

    G4ThreeVector lightGuidePosition = G4ThreeVector(0, 0, z0 + lightguideLength / 2.);
    G4VPhysicalVolume *lightGuidePhysical = new G4PVPlacement(0 /* rotation */, lightGuidePosition /* position */,
                                                              lightGuideLogical /* logical volume */,
                                                              "lightGuide" /* name */,
                                                              worldLogical /* parent logical */,
                                                              false /* boolean operation */, 0 /* copy number */,
                                                              checkOverlaps);
    // Set LG volume (for Stepping Action)
    fLGVolume = lightGuideLogical;

    // WRAP
    if (hasWrap) {
    in[0] = lightguideLength / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeAheight / 2. + wrapThickness; in[4] = lightguideSizeAbottom / 2. + wrapThickness; in[5] = lightguideSizeAtop / 2. + wrapThickness; in[6] = 0; in[7] = lightguideSizeBheight / 2. + wrapThickness; in[8] = lightguideSizeBbottom / 2. + wrapThickness; in[9] = lightguideSizeBtop / 2. + wrapThickness; in[10] = 0;
    TrapParamAdjustment(in, out);
    G4Trap *lightGuideWrapASolid = new G4Trap("lightGuideWrapASolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    // Tip: added 1 nanometer because "Stuck Track: potential geometry or navigation problem." between "lightGuideWrap" and "grease"
    in[0] = lightguideLength / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeAheight / 2. /*+ 1 * nanometer*/; in[4] = lightguideSizeAbottom / 2. /*+ 1 * nanometer*/; in[5] = lightguideSizeAtop / 2. /*+ 1 * nanometer*/; in[6] = 0; in[7] = lightguideSizeBheight / 2. /*+ 1 * nanometer*/; in[8] = lightguideSizeBbottom / 2. /*+ 1 * nanometer*/; in[9] = lightguideSizeBtop / 2. /*+ 1 * nanometer*/; in[10] = 0;
    TrapParamAdjustment(in, out);
    G4Trap *lightGuideWrapBSolid = new G4Trap("lightGuideWrapBSolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    
    G4RotationMatrix *noRotation = new G4RotationMatrix();
    G4SubtractionSolid *lightGuideWrapSolid = new G4SubtractionSolid("lightGuideWrapSolid", lightGuideWrapASolid,
                                                                     lightGuideWrapBSolid, noRotation,
                                                                     G4ThreeVector(0, 0, 0));
                                                                     
    G4Material *wrapMaterial = Materials::getInstance()->getMaterial("C10H8O4");
    G4LogicalVolume *lightGuideWrapLogical = new G4LogicalVolume(lightGuideWrapSolid, wrapMaterial,
                                                                 "lightGuideWrapLogical");
    G4VPhysicalVolume *lightGuideWrapPhysical = new G4PVPlacement(0 /* rotation */, lightGuidePosition /* position */,
                                                                  lightGuideWrapLogical /* logical volume */,
                                                                  "lightGuideWrap" /* name */,
                                                                  worldLogical /* parent logical */,
                                                                  false /* boolean operation */, 0 /* copy number */,
                                                                  checkOverlaps);
    G4VisAttributes *wrapLogicalVisAttr = new G4VisAttributes();
    lightGuideWrapLogical->SetVisAttributes(wrapLogicalVisAttr);
    wrapLogicalVisAttr->SetForceWireframe();
    addReflectiveBorder(lightGuidePhysical, lightGuideWrapPhysical);
    }
  }

  // █▀▄ █▀▀ ▀█▀ █▀▀ █▀▀ ▀█▀ █▀█ █▀█
  // █▄▀ ██▄ ░█░ ██▄ █▄▄ ░█░ █▄█ █▀▄

  // coordinate of front edge of the detector
  G4double zDet = z0 + lightguideLength;

  if (AbsMPPC *mppc = dynamic_cast<AbsMPPC*>(detector)) {

    // GREASE
    if (hasGrease) {
      G4Trap *greaseSolid = new G4Trap("greaseSolid", greaseThickness / 2., 0, 0, lightguideSizeBheight / 2., lightguideSizeBbottom / 2., lightguideSizeBtop / 2., 0, lightguideSizeBheight / 2., lightguideSizeBbottom / 2., lightguideSizeBtop / 2., 0);
      G4Material *greaseMaterial = Materials::getInstance()->getMaterial("BC630");
      G4LogicalVolume *greaseLogical = new G4LogicalVolume(greaseSolid, greaseMaterial, "greaseLogical");
      G4ThreeVector greasePosition = G4ThreeVector(0, 0, zDet + greaseThickness / 2.);
      /* G4VPhysicalVolume *greasePhysical = */new G4PVPlacement(0 /* rotation */, greasePosition /* position */,
                                                                 greaseLogical /* logical volume */,
                                                                 "grease" /* name */, worldLogical /* parent logical */,
                                                                 false /* boolean operation */, 0 /* copy number */,
                                                                 checkOverlaps);
    }
    // COOKIE
    else if (hasCookie) {
      G4Trap *cookieSolid = new G4Trap("cookieSolid", cookieThickness / 2., 0, 0, lightguideSizeBheight / 2., lightguideSizeBbottom / 2., lightguideSizeBtop / 2., 0, lightguideSizeBheight / 2., lightguideSizeBbottom / 2., lightguideSizeBtop / 2., 0);
      G4Material *cookieMaterial = Materials::getInstance()->getMaterial("Cookie");
      G4LogicalVolume *cookieLogical = new G4LogicalVolume(cookieSolid, cookieMaterial, "cookieLogical");
      G4ThreeVector cookiePosition = G4ThreeVector(0, 0, zDet + cookieThickness / 2.);
      /* G4VPhysicalVolume *greasePhysical = */new G4PVPlacement(0 /* rotation */, cookiePosition /* position */,
                                                                 cookieLogical /* logical volume */,
                                                                 "cookie" /* name */, worldLogical /* parent logical */,
                                                                 false /* boolean operation */, 0 /* copy number */,
                                                                 checkOverlaps);
    }
    
    // MPPC WINDOW
    G4Box *mppcWindowSolid = new G4Box("mppcWindowSolid", 0.5 * mppc->getXYSize(), 0.5 * mppc->getXYSize(),
                                       0.5 * mppc->getWindowThickness());
    G4LogicalVolume *mppcWindowLogical = new G4LogicalVolume(mppcWindowSolid, mppc->getWindowMaterial(),
                                                             "mppcWindowLogical");
    G4ThreeVector mppcWindowPosition = G4ThreeVector(0, 0, zDet + (hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getWindowThickness() / 2.);
    /*G4VPhysicalVolume* mppcWindowPhysical =*/new G4PVPlacement(0, mppcWindowPosition, mppcWindowLogical, "mppcWindow",
                                                                 worldLogical, false, 0, checkOverlaps);

    // MPPC CATHODE
    G4Box *mppcCathodeSolid = new G4Box("mppcCathodeSolid", 0.5 * mppc->getXYSize(), 0.5 * mppc->getXYSize(),
                                        0.5 * mppc->getCathodeThickness());
    G4LogicalVolume *mppcCathodeLogical = new G4LogicalVolume(mppcCathodeSolid, mppc->getCathodeMaterial(),
                                                              "mppcCathodeLogical");
    G4ThreeVector mppcCathodePosition = G4ThreeVector(
        0, 0, zDet + (hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getWindowThickness() + mppc->getCathodeThickness() / 2.);
    /*G4VPhysicalVolume* mppcCathodePhysical =*/new G4PVPlacement(0, mppcCathodePosition, mppcCathodeLogical,
                                                                  "mppcCathode", worldLogical, false, 0, checkOverlaps);
    // Set scoring volume (for Stepping Action)
    fScoringVolume = mppcCathodeLogical;

    // MPPC SHIELD
    // This shield stops photons exiting the sensitive detector volume from anywhere other than the window (killed in stepping action)
    in[0] = ((hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getWindowThickness() + detector->getCathodeThickness()) / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeBheight / 2. + wrapThickness; in[4] = lightguideSizeBbottom / 2. + wrapThickness; in[5] = lightguideSizeBtop / 2. + wrapThickness; in[6] = 0; in[7] = lightguideSizeBheight / 2. + wrapThickness; in[8] = lightguideSizeBbottom / 2. + wrapThickness; in[9] = lightguideSizeBtop / 2. + wrapThickness; in[10] = 0;
    TrapParamAdjustment(in, out);
    G4Trap *mppcShieldOuter = new G4Trap("lightGuideWrapASolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    in[0] = ((hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getWindowThickness() + detector->getCathodeThickness()) / 2.; in[1] = 0; in[2] = 0; in[3] = lightguideSizeBheight / 2.; in[4] = lightguideSizeBbottom / 2.; in[5] = lightguideSizeBtop / 2.; in[6] = 0; in[7] = lightguideSizeBheight / 2.; in[8] = lightguideSizeBbottom / 2.; in[9] = lightguideSizeBtop / 2.; in[10] = 0;
    TrapParamAdjustment(in, out);
    G4Trap *mppcShieldInner = new G4Trap("lightGuideWrapBSolid", out[0], out[1], out[2], out[3], out[4], out[5], out[6], out[7], out[8], out[9], out[10]);
    G4RotationMatrix *noRotation = new G4RotationMatrix();
    G4SubtractionSolid *mppcShieldSolid = new G4SubtractionSolid("mppcShieldSolid", mppcShieldOuter,
                                                                    mppcShieldInner, noRotation, G4ThreeVector());

    G4Material *shieldMaterial = Materials::getInstance()->getMaterial("MuMetal");
    G4LogicalVolume *mppcShieldLogical = new G4LogicalVolume(mppcShieldSolid, shieldMaterial,
                                                                "mppcShieldLogical");
    G4ThreeVector mppcShieldPosition = G4ThreeVector(0, 0, zDet + ((hasGrease || hasCookie ? (hasGrease ? greaseThickness : cookieThickness) : airThickness) + detector->getWindowThickness() + mppc->getCathodeThickness()) / 2.);
    /*G4VPhysicalVolume* mppcShieldPhysical =*/new G4PVPlacement(0, mppcShieldPosition, mppcShieldLogical,
                                                                    "mppcShield", worldLogical, false, 0,
                                                                    checkOverlaps);
    // Set shield volume (for Stepping Action)
    fShieldVolume = mppcShieldLogical;
  }
  // always return the physical World
  return worldPhysical;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::addReflectiveSkin(G4LogicalVolume *volumeLogical) {
  G4String surfaceName = volumeLogical->GetName() + "Surface";
  G4OpticalSurface *surface = new G4OpticalSurface(surfaceName);
  surface->SetType(dielectric_LUT);
  surface->SetModel(LUT);
  surface->SetFinish(polishedvm2000air);

  G4String skinSurfaceName = volumeLogical->GetName() + "SkinSurface";
  /*G4LogicalSkinSurface* skinSurface = */new G4LogicalSkinSurface(skinSurfaceName, volumeLogical, surface);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::addReflectiveBorder(G4VPhysicalVolume *volumePhysical1, G4VPhysicalVolume *volumePhysical2) {
  G4String surfaceName = volumePhysical1->GetName() + volumePhysical2->GetName() + "Surface";
  G4OpticalSurface *surface = new G4OpticalSurface(surfaceName);
  surface->SetType(dielectric_LUT);
  surface->SetModel(LUT);
  surface->SetFinish(polishedvm2000air);

  G4String borderSurfaceName = volumePhysical1->GetName() + volumePhysical2->GetName() + "BorderSurface";
  /*G4LogicalBorderSurface* borderSurface = */new G4LogicalBorderSurface(borderSurfaceName, volumePhysical1,
                                                                         volumePhysical2, surface);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AbsDetector* DetectorConstruction::getDetector() {
  return detector;
}

G4double DetectorConstruction::getLightGuideSizeAbottom() {
  return lightguideSizeAbottom;
}

G4double DetectorConstruction::getLightGuideSizeAtop() {
  return lightguideSizeAtop;
}

G4double DetectorConstruction::getLightGuideSizeAheight() {
  return lightguideSizeAheight;
}

G4double DetectorConstruction::getLightGuideSizeBbottom() {
  return lightguideSizeBbottom;
}

G4double DetectorConstruction::getLightGuideSizeBtop() {
  return lightguideSizeBtop;
}

G4double DetectorConstruction::getLightGuideSizeBheight() {
  return lightguideSizeBheight;
}

G4double DetectorConstruction::getLightGuideLength() {
  return lightguideLength;
}

G4String DetectorConstruction::getLightGuideShape() {
  return lightguideShape;
}

G4double DetectorConstruction::getGreaseThickness() {
  return greaseThickness;
}

G4double DetectorConstruction::getCookieThickness() {
  return cookieThickness;
}

G4double DetectorConstruction::getAirThickness() {
  return airThickness;
}

G4bool DetectorConstruction::getHasGrease() {
  return hasGrease;
}

G4bool DetectorConstruction::getHasCookie() {
  return hasCookie;
}

G4bool DetectorConstruction::getHasWrap() {
  return hasWrap;
}
