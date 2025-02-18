/*
 * Utils.hh
 *
 *  Created on: Nov 29, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_UTILS_HH_
#define SRC_UTILS_HH_

#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4Types.hh>
#include <G4Step.hh>
#include <G4Track.hh>
#include <string>
#include <vector>

class AbsDetector;
class DetectorConstruction;
class G4Event;
class G4Timer;
class G4VScoringMesh;

struct EnergyValueUnit {
  G4double value;
  G4String unit;
};

class Utils {
private:
  Utils();
  static Utils* instance;

  std::chrono::high_resolution_clock::time_point tStart;

//  G4int firstTOFNtupleId;
//  G4int* TOFNtupleIds;

  //       event_number  ntuple_number
  std::map<int,          int> eventNumberToNTupleNumber;

public:
  virtual ~Utils();

  static const G4int maxTofEvents;

  void startTimer();
  void printTimer();

  // Utils();
  static Utils* getInstance();

  static G4double getEnergy();
  static G4String getParticleName();
  static G4double getLightGuideLength();
  static G4String getLightGuideShape();
  static G4bool getHasGrease();
  static G4bool getHasCookie();
  static G4bool getHasWrap();
  static G4int getNumberOfEvents();
  static AbsDetector* getDetector();

  static DetectorConstruction* getDetectorConstruction();

  static G4bool usingOpticalPhysics();

  static EnergyValueUnit formatEnergy(G4double);
  // static PhotoCathodeType getPhotoCathodeType();

  static G4double getWavelength(const G4Track* track);

  static G4String getOutputFileName(G4String suffix = "");
};

#endif /* SRC_UTILS_HH_ */
