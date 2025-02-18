/*
 * Utils.cc
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#include "Utils.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "detectors/AbsDetector.hh"

#include <G4ThreeVector.hh>
#include <G4Event.hh>
#include <G4Exception.hh>
#include <G4ExceptionSeverity.hh>
#include <G4GeneralParticleSource.hh>
#include <G4HCofThisEvent.hh>
#include <G4ios.hh>
#include <G4ParticleDefinition.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4ScoringManager.hh>
#include <G4SystemOfUnits.hh>
#include <G4SingleParticleSource.hh>
#include <G4SPSEneDistribution.hh>
#include <G4SPSPosDistribution.hh>
#include <G4StatDouble.hh>
#include <G4THitsMap.hh>
#include <G4Timer.hh>
#include <G4VHitsCollection.hh>
#include <G4VModularPhysicsList.hh>
#include <G4VScoringMesh.hh>
#include <G4AnalysisManager.hh>
#include <G4VProcess.hh>
#include <stddef.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
// #include "g4analysis.hh"

using std::chrono::_V2::system_clock;

Utils::Utils() /* : firstTOFNtupleId(0)*/{
}

const G4int Utils::maxTofEvents = 100;

// G4int* Utils::TOFNtupleIds = new G4int[maxTofEvents];

Utils::~Utils() {
  // TODO Auto-generated destructor stub
}

void Utils::startTimer() {
  tStart = std::chrono::high_resolution_clock::now();
}

using namespace std::chrono_literals;

void Utils::printTimer() {
  // Print time (wall clock)
  // https://en.cppreference.com/w/cpp/chrono/c/clock
  auto tEnd = std::chrono::high_resolution_clock::now();
  std::chrono::duration<float> interval = tEnd - tStart;

  // https://stackoverflow.com/questions/14391327/how-to-get-duration-as-int-millis-and-float-seconds-from-chrono
  std::cout << "Duration is: " << std::chrono::duration_cast<std::chrono::hours>(interval).count() << " hours or "
            << std::chrono::duration_cast<std::chrono::minutes>(interval).count() << " minutes or "
            << std::chrono::duration_cast<std::chrono::seconds>(interval).count() << " seconds." << std::endl;
}

Utils *Utils::instance = NULL;

Utils* Utils::getInstance() {
  if (!instance) {
    instance = new Utils();
  }
  return instance;
}

G4double Utils::getEnergy() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return 0;

  const G4ParticleGun *particleGun = primaryGeneratorAction->GetParticleGun();
  return particleGun->GetParticleEnergy();
}

G4double Utils::getLightGuideLength() {
  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (dc == NULL)
    return 0;

  return dc->getLightGuideLength();
}

G4String Utils::getLightGuideShape() {
  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (dc == NULL)
    return 0;

  return dc->getLightGuideShape();
}

G4bool Utils::getHasGrease() {
  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (dc == NULL)
    return 0;

  return dc->getHasGrease();
}

G4bool Utils::getHasCookie() {
  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (dc == NULL)
    return 0;

  return dc->getHasCookie();
}

G4bool Utils::getHasWrap() {
  DetectorConstruction *dc = (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (dc == NULL)
    return 0;

  return dc->getHasWrap();
}

/*
 G4double Utils::getGPSZPos() const {
 PrimaryGeneratorAction *primaryGeneratorAction =
 (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
 if (primaryGeneratorAction == NULL)
 return 0;

 // PS: we're setting energy via "/gps/ene/mono". Therefore energy is saved
 // in G4SingleParticleSource (check in G4GeneralParticcleSourcceMessenger) which is a member ofG4GeneralParticleSource
 const G4ParticleGun* particleGun = primaryGeneratorAction->GetParticleGun();
 G4ThreeVector particlePosition = particleGun->GetParticlePosition();
 return particlePosition.getZ();
 }
 */
G4String Utils::getParticleName() {
  PrimaryGeneratorAction *primaryGeneratorAction =
      (PrimaryGeneratorAction*) G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
  if (primaryGeneratorAction == NULL)
    return "";

  G4ParticleGun *particleGun = primaryGeneratorAction->GetParticleGun();
  return particleGun->GetParticleDefinition()->GetParticleName();
}

G4int Utils::getNumberOfEvents() {
  G4RunManager *runManager = G4RunManager::GetRunManager();
  G4int nEvents = runManager->GetNumberOfEventsToBeProcessed();
  return nEvents;
}

DetectorConstruction* Utils::getDetectorConstruction() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  return detectorConstruction;
}

AbsDetector* Utils::getDetector() {
  DetectorConstruction *detectorConstruction =
      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
  if (detectorConstruction == NULL)
    return 0;
  return detectorConstruction->getDetector();
}

EnergyValueUnit Utils::formatEnergy(G4double value) {
  if (value >= 1E6) {
    EnergyValueUnit evu;
    evu.unit = "TeV";
    evu.value = value / 1E6;
    return evu;
  }
  if (value >= 1E3) {
    EnergyValueUnit evu;
    evu.unit = "GeV";
    evu.value = value / 1E3;
    return evu;
  }
  if (value >= 1) {
    EnergyValueUnit evu;
    evu.unit = "MeV";
    evu.value = value;
    return evu;
  }
  EnergyValueUnit evu;
  evu.unit = "keV";
  evu.value = value * 1000;
  return evu;
}

G4bool Utils::usingOpticalPhysics() {
  G4VUserPhysicsList *userPhysicsList = (G4VUserPhysicsList*) G4RunManager::GetRunManager()->GetUserPhysicsList();
  if (userPhysicsList == NULL)
    return false;

  G4VModularPhysicsList *modularPhysicsList = dynamic_cast<G4VModularPhysicsList*>(userPhysicsList);
  if (modularPhysicsList == NULL)
    return false;

  return modularPhysicsList->GetPhysics("Optical") != NULL;
}

//G4ThreeVector* Utils::getAbsolutePosition(G4VPhysicalVolume* volume){
//  G4ThreeVector* vector = new G4ThreeVector();
//  for (G4VPhysicalVolume* currentVolume = volume; currentVolume!=NULL; currentVolume = currentVolume->GetMotherLogical()->Get;GetMotherLogical()Logical()->Get)
//}

//PhotoCathodeType Utils::getPhotoCathodeType(){
//  DetectorConstruction *detectorConstruction =
//      (DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();
//  if (detectorConstruction == NULL)
//    return PhotoCathodeType::Bialkali;
//  return detectorConstruction->getPMT()->getPhotocathodeType();
//}

G4double Utils::getWavelength(const G4Track *track) {
  G4double energy = track->GetDynamicParticle()->GetKineticEnergy() / eV; // in eV
  G4double hc = 1239.84193;
  G4double waveLength = hc / (energy); // in nm
  return waveLength;
}

G4String Utils::getOutputFileName(G4String suffix) {
  // Construct filename
  std::stringstream buffer;
//  buffer << Utils::getEnergy()*1E6. << "eV-"; // Doesn't work, since I'm setting the particle gun energy after the output file is created...
//  buffer << "450nm-";
  buffer << "SingleClad-450nm-";
  buffer << Utils::getNumberOfEvents() << "events-";
//  buffer << Utils::getParticleName() << "-";
  buffer << Utils::getLightGuideLength() << "-mm-lightguide-";
  if (Utils::getLightGuideShape() != "") buffer << Utils::getLightGuideShape() << "-";
  if (Utils::getHasWrap() == 1) buffer << "wrap-";
  Utils::getHasGrease() || Utils::getHasCookie() ? (Utils::getHasGrease() ? buffer << "grease-" : buffer << "cookie-") : buffer << "air-";
//  if (Utils::getGPSZPos() > 0)
//    buffer << "+";
//  if (Utils::getGPSZPos() > 0 && Utils::getGPSZPos() < 100)
//    buffer << "0";
//  buffer << Utils::getGPSZPos() << "mm-";
  buffer << Utils::getDetector()->getName();
  if (suffix.length() > 0) {
    buffer << "-" << suffix;
  }
  buffer << ".root";

  G4String fileNameString = buffer.str();
  return fileNameString;
}
