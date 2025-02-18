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
/// \file SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "detectors/AbsDetector.hh"
#include "Utils.hh"

#include <G4Step.hh>
#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4LogicalVolume.hh>
#include <G4OpticalPhoton.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step *step) {
  // Set scoring, shield, and LG volumes if not set previously
  if (!fScoringVolume) {
    const DetectorConstruction *detConstruction =
        static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
  }
  if (!fShieldVolume) {
    const DetectorConstruction *detConstruction =
        static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fShieldVolume = detConstruction->GetShieldVolume();
  }
  if (!fLGVolume) {
    const DetectorConstruction *detConstruction =
        static_cast<const DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fLGVolume = detConstruction->GetLGVolume();
  }

  // Get volume of the current step
  G4LogicalVolume *volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

  // Check if the particle is leaving the light guide or being killed in the light guide
  if (volume == fLGVolume) {
    if (step->GetTrack()->GetTrackID() == 1 && (step->GetPostStepPoint()->GetStepStatus() == fGeomBoundary || step->GetTrack()->GetTrackStatus() == fStopAndKill)) {
      // Fill Ntuple: the generated photon is leaving or dying inside the light guide
      if (!fAnalysisManager) fAnalysisManager = G4AnalysisManager::Instance();
      fAnalysisManager->FillNtupleDColumn(9, step->GetPostStepPoint()->GetPosition().x());
      fAnalysisManager->FillNtupleDColumn(10, step->GetPostStepPoint()->GetPosition().y());
      fAnalysisManager->FillNtupleDColumn(11, step->GetPostStepPoint()->GetPosition().z());
      fAnalysisManager->FillNtupleDColumn(12, step->GetPostStepPoint()->GetMomentumDirection().theta());
      fAnalysisManager->FillNtupleDColumn(13, step->GetPostStepPoint()->GetMomentumDirection().phi());
    }
  }

//std::cout << std::endl << "TRACK DEFINITION:	" << step->GetTrack()->GetDefinition() << std::endl << "TRACK ID:		" << step->GetTrack()->GetTrackID() << std::endl << "PARENT ID:	" << step->GetTrack()->GetParentID() << std::endl << "fStopAndKill:		" << fStopAndKill << std::endl << "VOLUME:		" << volume << std::endl << "fScoringVolume:		" << fScoringVolume << std::endl << "POSITION:		" << step->GetPostStepPoint()->GetPosition().x() << "	" << step->GetPostStepPoint()->GetPosition().y() << "	" << step->GetPostStepPoint()->GetPosition().z() << std::endl;

  // Check if we are in the MPPC shield and, if so, kill the track
  if (volume == fShieldVolume) step->GetTrack()->SetTrackStatus(fStopAndKill);
  // Check if we are in scoring volume
  if (volume == fScoringVolume) {
//std::cout << std::endl << "SCORING VOLUME TRACK DEFINITION:	" << step->GetTrack()->GetDefinition() << std::endl << "TRACK ID:		" << step->GetTrack()->GetTrackID() << std::endl << "fStopAndKill:		" << fStopAndKill << std::endl << "VOLUME:		" << volume << std::endl << "fScoringVolume:		" << fScoringVolume << std::endl << "POSITION:		" << step->GetPostStepPoint()->GetPosition().x() << "	" << step->GetPostStepPoint()->GetPosition().y() << "	" << step->GetPostStepPoint()->GetPosition().z() << std::endl;
    // Terminate optical photon track
    step->GetTrack()->SetTrackStatus(fStopAndKill);
//if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition() || step->GetTrack()->GetTrackID() != 1) std::cout << std::endl << "WARNING" << std::endl;
    // Ensure particle type is the primary optical photon
    if (step->GetTrack()->GetDefinition() == G4OpticalPhoton::Definition() && step->GetTrack()->GetTrackID() == 1) {
      // Fill Ntuple: the generated photon has reached the sensitive detector
      if (!fAnalysisManager) fAnalysisManager = G4AnalysisManager::Instance();
      fAnalysisManager->FillNtupleIColumn(6, 1);
      fAnalysisManager->FillNtupleDColumn(14, step->GetPreStepPoint()->GetPosition().x());
      fAnalysisManager->FillNtupleDColumn(15, step->GetPreStepPoint()->GetPosition().y());
      fAnalysisManager->FillNtupleDColumn(16, step->GetPreStepPoint()->GetPosition().z());
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
