/*
 * DetectorMessenger.h
 *
 *  Created on: Sep 7, 2022
 *      Author: petrstepanov
 */

#ifndef SRC_DETECTORMESSENGER_H_
#define SRC_DETECTORMESSENGER_H_

#include <G4UImessenger.hh>

#include <globals.hh>
#include <G4SystemOfUnits.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithABool.hh>

class DetectorConstruction;
class G4UIdirectory;


class DetectorMessenger: public G4UImessenger
{
	friend class DetectorConstruction;

public:

  void SetNewValue(G4UIcommand*, G4String);

private:

  DetectorMessenger(DetectorConstruction* );
  ~DetectorMessenger();

  DetectorConstruction*        fDetector;           // pointer to detector
  G4UIdirectory*               fDetectorDir;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeAbottomCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeAtopCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeAheightCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeBbottomCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeBtopCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideSizeBheightCmd;
  G4UIcmdWithADoubleAndUnit*   fLightGuideLengthCmd;
  G4UIcmdWithABool*            fHasWrapCmd;
  G4UIcmdWithABool*            fHasGreaseCmd;
  G4UIcmdWithABool*            fHasCookieCmd;
  G4UIcmdWithAString*          fDetectorTypeCmd;
  G4UIcmdWithAString*          fLightGuideShapeCmd;

public:
  // Command default values
  static constexpr G4double lightguideSizeAbottomCmdDefaultValue = 24.33*mm;
  static constexpr G4double lightguideSizeAtopCmdDefaultValue = 25.02*mm;
  static constexpr G4double lightguideSizeAheightCmdDefaultValue = 20.57*mm;
  static constexpr G4double lightguideSizeBbottomCmdDefaultValue = 13.19*mm;
  static constexpr G4double lightguideSizeBtopCmdDefaultValue = 13.61*mm;
  static constexpr G4double lightguideSizeBheightCmdDefaultValue = 13.19*mm;
  static constexpr G4double lightguideLengthCmdDefaultValue = 80.00*mm;
  static const G4bool hasWrapCmdDefaultValue = false;
  static const G4bool hasGreaseCmdDefaultValue = false;
  static const G4bool hasCookieCmdDefaultValue = false;
  static const G4String detectorTypeCmdDefaultValue;
  static const G4String lightguideShapeCmdDefaultValue;
};
#endif /* SRC_DETECTORMESSENGER_H_ */
