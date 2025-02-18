/*
 * DetectorMessenger.cpp
 *
 *  Created on: Sep 7, 2022
 *      Author: petrstepanov
 */

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const G4String DetectorMessenger::detectorTypeCmdDefaultValue = "S12-GlueX";
const G4String DetectorMessenger::lightguideShapeCmdDefaultValue = "";

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction *d) : fDetector(d) {
  fDetectorDir = new G4UIdirectory("/detector/");
  fDetectorDir->SetGuidance("Configuration of Detector setup.");

  fLightGuideSizeAbottomCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeAbottom", this);
  fLightGuideSizeAbottomCmd->SetGuidance("Define the bottom width of the input end of the lightguide.");
  // fLightGuideSizeAbottomCmd->SetParameterName("choice", true);
  fLightGuideSizeAbottomCmd->SetDefaultValue(lightguideSizeAbottomCmdDefaultValue);
  fLightGuideSizeAbottomCmd->AvailableForStates(G4State_PreInit);

  fLightGuideSizeAtopCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeAtop", this);
  fLightGuideSizeAtopCmd->SetGuidance("Define the top width of the input end of the lightguide.");
  // fLightGuideSizeAtopCmd->SetParameterName("choice", true);
  fLightGuideSizeAtopCmd->SetDefaultValue(lightguideSizeAtopCmdDefaultValue);
  fLightGuideSizeAtopCmd->AvailableForStates(G4State_PreInit);

  fLightGuideSizeAheightCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeAheight", this);
  fLightGuideSizeAheightCmd->SetGuidance("Define the height of the input end of the lightguide.");
  // fLightGuideSizeAheightCmd->SetParameterName("choice", true);
  fLightGuideSizeAheightCmd->SetDefaultValue(lightguideSizeAheightCmdDefaultValue);
  fLightGuideSizeAheightCmd->AvailableForStates(G4State_PreInit);

  fLightGuideSizeBbottomCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeBbottom", this);
  fLightGuideSizeBbottomCmd->SetGuidance("Define the bottom width of the output end of the lightguide.");
  // fLightGuideSizeBbottomCmd->SetParameterName("choice", true);
  fLightGuideSizeBbottomCmd->SetDefaultValue(lightguideSizeBbottomCmdDefaultValue);
  fLightGuideSizeBbottomCmd->AvailableForStates(G4State_PreInit);

  fLightGuideSizeBtopCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeBtop", this);
  fLightGuideSizeBtopCmd->SetGuidance("Define the top width of the output end of the lightguide.");
  // fLightGuideSizeBtopCmd->SetParameterName("choice", true);
  fLightGuideSizeBtopCmd->SetDefaultValue(lightguideSizeBtopCmdDefaultValue);
  fLightGuideSizeBtopCmd->AvailableForStates(G4State_PreInit);

  fLightGuideSizeBheightCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideSizeBheight", this);
  fLightGuideSizeBheightCmd->SetGuidance("Define the height of the output end of the lightguide.");
  // fLightGuideSizeBheightCmd->SetParameterName("choice", true);
  fLightGuideSizeBheightCmd->SetDefaultValue(lightguideSizeBheightCmdDefaultValue);
  fLightGuideSizeBheightCmd->AvailableForStates(G4State_PreInit);

  fLightGuideLengthCmd = new G4UIcmdWithADoubleAndUnit("/detector/lightguideLength", this);
  fLightGuideLengthCmd->SetGuidance("Define the length of the lightguide.");
  // fLightGuideLengthCmd->SetParameterName("choice", true);
  fLightGuideLengthCmd->SetDefaultValue(lightguideLengthCmdDefaultValue);
  fLightGuideLengthCmd->AvailableForStates(G4State_PreInit);

  fHasWrapCmd = new G4UIcmdWithABool("/detector/hasWrap", this);
  fHasWrapCmd->SetGuidance("Set presence of the light guide wrap.");
  // fHasWrapCmd->SetParameterName("choice", true);
  fHasWrapCmd->SetDefaultValue(false);
  fHasWrapCmd->AvailableForStates(G4State_PreInit);

  fHasGreaseCmd = new G4UIcmdWithABool("/detector/hasGrease", this);
  fHasGreaseCmd->SetGuidance("Set presence of grease between light guide and detector ('false' means an air gap will be used).");
  // fHasGreaseCmd->SetParameterName("choice", true);
  fHasGreaseCmd->SetDefaultValue(false);
  fHasGreaseCmd->AvailableForStates(G4State_PreInit);

  fHasCookieCmd = new G4UIcmdWithABool("/detector/hasCookie", this);
  fHasCookieCmd->SetGuidance("Set presence of optical cookie between light guide and detector ('false' means an air gap will be used, and hasGrease will take priority over hasCookie).");
  // fHasCookieCmd->SetParameterName("choice", true);
  fHasCookieCmd->SetDefaultValue(false);
  fHasCookieCmd->AvailableForStates(G4State_PreInit);

  fDetectorTypeCmd = new G4UIcmdWithAString("/detector/detectorType", this);
  fDetectorTypeCmd->SetGuidance("Define model of the sensitive detector.");
  // fDetectorTypeCmd->SetParameterName("choice", true);
  fDetectorTypeCmd->SetDefaultValue("");
  fDetectorTypeCmd->AvailableForStates(G4State_PreInit);

  fLightGuideShapeCmd = new G4UIcmdWithAString("/detector/lightguideShape", this);
  fLightGuideShapeCmd->SetGuidance("Define shape of the light guide (GlueX LightGuide number, g#, or BIC LightGuide number, b#).");
  // fLightGuideShapeCmd->SetParameterName("choice", true);
  fLightGuideShapeCmd->SetDefaultValue("");
  fLightGuideShapeCmd->AvailableForStates(G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger() {
  delete fDetectorDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand *command, G4String newValue) {
  // Select the type of reflection at ground
  if (command == fLightGuideSizeAbottomCmd) {
    fDetector->setLightGuideSizeAbottom(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideSizeAtopCmd) {
    fDetector->setLightGuideSizeAtop(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideSizeAheightCmd) {
    fDetector->setLightGuideSizeAheight(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideSizeBbottomCmd) {
    fDetector->setLightGuideSizeBbottom(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideSizeBtopCmd) {
    fDetector->setLightGuideSizeBtop(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideSizeBheightCmd) {
    fDetector->setLightGuideSizeBheight(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fLightGuideLengthCmd) {
    fDetector->setLightGuideLength(G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue));
  } else if (command == fHasWrapCmd) {
    fDetector->setHasWrap(G4UIcmdWithABool::GetNewBoolValue(newValue));
  } else if (command == fHasGreaseCmd) {
    fDetector->setHasGrease(G4UIcmdWithABool::GetNewBoolValue(newValue));
  } else if (command == fHasCookieCmd) {
    fDetector->setHasCookie(G4UIcmdWithABool::GetNewBoolValue(newValue));
  } else if (command == fDetectorTypeCmd) {
    fDetector->setDetectorType(newValue);
  } else if (command == fLightGuideShapeCmd) {
    fDetector->setLightGuideShape(newValue);
  }

}
