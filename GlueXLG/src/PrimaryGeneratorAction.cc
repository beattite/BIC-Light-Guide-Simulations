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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include <G4LogicalVolumeStore.hh>
#include <G4LogicalVolume.hh>
#include <G4Box.hh>
#include <G4RunManager.hh>
#include <G4ParticleGun.hh>
#include <G4ParticleTable.hh>
#include <G4ParticleDefinition.hh>
#include <G4SystemOfUnits.hh>
#include <Randomize.hh>
#include "G4PhysicalConstants.hh"
#include "DetectorConstruction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction() {
  fParticleGun = new G4ParticleGun();
  G4int n_particle = 1;
   fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="opticalphoton");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(2.7552156*eV); // 450 nm
  //fParticleGun->SetParticleEnergy(2.0*eV); // 620 nm
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  //this function is called at the begining of each event

  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  G4double assemblyLength = dc->getLightGuideLength() + (dc->getHasGrease() || dc->getHasCookie() ? (dc->getHasGrease() ? dc->getGreaseThickness() : dc->getCookieThickness()) : dc->getAirThickness()) + dc->getDetector()->getZSize();
  G4double z = -assemblyLength / 2;
  
  G4double width, height;
  dc->getLightGuideSizeAbottom() >= dc->getLightGuideSizeAtop() ? width = dc->getLightGuideSizeAbottom() : width = dc->getLightGuideSizeAtop();
  height = dc->getLightGuideSizeAheight();
  G4double d = -1.0;
  G4double x = 0.0;
  G4double y = 0.0;
  while (abs(x) > d) {
    x = (G4UniformRand() - 0.5)*width;
    y = (G4UniformRand() - 0.5)*height;
    //x = G4UniformRand()*width*0.5; // Half-source in x
    //y = G4UniformRand()*height*0.5; // Half-source in y
    d = dc->getLightGuideSizeAbottom()/2.0 + (dc->getLightGuideSizeAtop() - dc->getLightGuideSizeAbottom())/(2.0*height)*(height/2.0 + y);
  }
  fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z)); // Might have to use 'SetInitialValues' in newer versions of Geant4?
  G4double phi = G4UniformRand()*twopi;
  //G4double theta = acos(0.893371388327838 + G4UniformRand()*0.106628611672162); // Uniformly generate over cos(theta) up to a maximum of 26.7 degrees (double-clad Kuraray)
  G4double theta = acos(0.937281989491892 + G4UniformRand()*0.062718010508108); // Uniformly generate over cos(theta) up to a maximum of 20.4 degrees (single-clad Kuraray)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta)));
//std::cout << std::endl << "INITIAL LOCATION:	" << x << "	" << y << "	" << z << std::endl;
//std::cout << "MOMENTUM DIRECTION:	" << sin(theta)*cos(phi) << "	" << sin(theta)*sin(phi) << "	" << cos(theta) << std::endl;
//  fParticleGun->SetParticlePolarization(G4ThreeVector(-1,0,0)); // TESTING POLARIZATION
  fParticleGun->GeneratePrimaryVertex(anEvent);
  
  // Output thrown parameters to Ntuple
  if (!fAnalysisManager) fAnalysisManager = G4AnalysisManager::Instance();
  fAnalysisManager->FillNtupleDColumn(0, x);
  fAnalysisManager->FillNtupleDColumn(1, y);
  fAnalysisManager->FillNtupleDColumn(2, z);
  fAnalysisManager->FillNtupleDColumn(3, theta);
  fAnalysisManager->FillNtupleDColumn(4, phi);
  fAnalysisManager->FillNtupleDColumn(5, fParticleGun->GetParticleEnergy());
//std::cout << std::endl << "PARTICLEGUN POLARIZATION:	" << fParticleGun->GetParticlePolarization().x() << "	" << fParticleGun->GetParticlePolarization().y() << "	" << fParticleGun->GetParticlePolarization().z() << std::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
