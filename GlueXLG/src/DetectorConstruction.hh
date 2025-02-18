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
/// \file DetectorConstruction.hh
/// \brief Definition of the B1::DetectorConstruction class

#ifndef B1DetectorConstruction_h
#define B1DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "DetectorMessenger.hh"
#include "detectors/AbsDetector.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

class DetectorConstruction: public G4VUserDetectorConstruction {
  public:
    DetectorConstruction();
    ~DetectorConstruction() override;

    G4VPhysicalVolume* Construct() override;

    G4LogicalVolume* GetScoringVolume() const {
      return fScoringVolume;
    }
    G4LogicalVolume* GetShieldVolume() const {
      return fShieldVolume;
    }
    G4LogicalVolume* GetLGVolume() const {
      return fLGVolume;
    }

  protected:
    G4LogicalVolume *fScoringVolume = nullptr;
    G4LogicalVolume *fShieldVolume = nullptr;
    G4LogicalVolume *fLGVolume = nullptr;
    AbsDetector *detector;
    G4double greaseThickness;
    G4double cookieThickness;
    G4double wrapThickness;
    G4double airThickness;
    void addReflectiveSkin(G4LogicalVolume* volume);
    void addReflectiveBorder(G4VPhysicalVolume* volume1, G4VPhysicalVolume* volume2);
    double in[11], out[11];
    void TrapParamAdjustment(const double (&in)[11], double (&out)[11]);

    // Define the messenger of the detector component
    DetectorMessenger* fDetectorMessenger = nullptr;

  public:
    // User parameters and Detector Messenger setters
    G4double lightguideSizeAbottom;
    void setLightGuideSizeAbottom(G4double value);
    G4double lightguideSizeAtop;
    void setLightGuideSizeAtop(G4double value);
    G4double lightguideSizeAheight;
    void setLightGuideSizeAheight(G4double value);
    G4double lightguideSizeBbottom;
    void setLightGuideSizeBbottom(G4double value);
    G4double lightguideSizeBtop;
    void setLightGuideSizeBtop(G4double value);
    G4double lightguideSizeBheight;
    void setLightGuideSizeBheight(G4double value);
    G4double lightguideLength;
    void setLightGuideLength(G4double value);
    G4bool hasWrap;
    void setHasWrap(G4bool value);
    G4bool hasGrease;
    void setHasGrease(G4bool value);
    G4bool hasCookie;
    void setHasCookie(G4bool value);
    G4String detectorType;
    void setDetectorType(G4String value);
    G4String lightguideShape;
    void setLightGuideShape(G4String value);

  public:
    // Getters
    AbsDetector* getDetector();
    G4double getLightGuideSizeAbottom();
    G4double getLightGuideSizeAtop();
    G4double getLightGuideSizeAheight();
    G4double getLightGuideSizeBbottom();
    G4double getLightGuideSizeBtop();
    G4double getLightGuideSizeBheight();
    G4double getLightGuideLength();
    G4String getLightGuideShape();
    G4bool getHasGrease();
    G4bool getHasCookie();
    G4bool getHasWrap();
    G4double getGreaseThickness();
    G4double getCookieThickness();
    G4double getAirThickness();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
