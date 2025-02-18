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
/// \file RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "Utils.hh"

#include <G4RunManager.hh>
#include <G4Run.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4LogicalVolume.hh>
#include <G4UnitsTable.hh>
#include <G4SystemOfUnits.hh>
#include <G4AnalysisManager.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction() {
  // Create analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetVerboseLevel(1);

  // Book ntuple
  analysisManager->CreateNtuple("lightguides", "Values per Geant4 event");
  analysisManager->CreateNtupleDColumn("thrownX");
  analysisManager->CreateNtupleDColumn("thrownY");
  analysisManager->CreateNtupleDColumn("thrownZ");
  analysisManager->CreateNtupleDColumn("thrownTheta");
  analysisManager->CreateNtupleDColumn("thrownPhi");
  analysisManager->CreateNtupleDColumn("thrownE");
  analysisManager->CreateNtupleIColumn("detected");
  analysisManager->CreateNtupleIColumn("eventNumber");
  analysisManager->CreateNtupleDColumn("guideLength");
  analysisManager->CreateNtupleDColumn("exitX");
  analysisManager->CreateNtupleDColumn("exitY");
  analysisManager->CreateNtupleDColumn("exitZ");
  analysisManager->CreateNtupleDColumn("exitTheta");
  analysisManager->CreateNtupleDColumn("exitPhi");
  analysisManager->CreateNtupleDColumn("detectionX");
  analysisManager->CreateNtupleDColumn("detectionY");
  analysisManager->CreateNtupleDColumn("detectionZ");
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*) {
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
//  analysisManager->SetNtupleDirectoryName("output");
  G4String fileName = "output/";
  fileName += Utils::getOutputFileName();
  analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run *run) {
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0)
    return;

  // Run conditions
  const PrimaryGeneratorAction *generatorAction =
      static_cast<const PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction) {
    const G4ParticleGun *particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy, "Energy");
  }

  // Print
  //
  if (IsMaster()) {
    G4cout << G4endl << "------------------- End of Global Run ----------------------";
  } else {
    G4cout << G4endl << "------------------- End of Local Run -----------------------";
  }
  
  G4cout << G4endl << " The run consists of " << nofEvents << " " << runCondition << G4endl
  << "------------------------------------------------------------" << G4endl << G4endl;
  
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
