/*
 * DetectorProvider.cc
 *
 *  Created on: Sep 10, 2022
 *      Author: petrstepanov
 */

#include "DetectorProvider.hh"
#include "S13360_6025CS.hh"
#include "S13360_6025CS_2x2.hh"
#include "S13360_6025CS_6x6.hh"
#include "S13360_6025CS_20.hh"
#include "S12_GlueX.hh"
#include "S13_BIC.hh"

DetectorProvider::DetectorProvider() {
  // TODO Auto-generated constructor stub

}

DetectorProvider::~DetectorProvider() {
  // TODO Auto-generated destructor stub
}

AbsDetector* DetectorProvider::getDetector(G4String name) {
  AbsDetector *detector;
  if (name == "S13360-6025CS") {
    detector = new S13360_6025CS();
  } else if (name == "S13360-6025CS-2x2") {
    detector = new S13360_6025CS_2x2();
  } else if (name == "S13360-6025CS-20") {
    detector = new S13360_6025CS_20();
  } else if (name == "S13360-6025CS-6x6") {
    detector = new S13360_6025CS_6x6();
  } else if (name == "S12-GlueX") {
    detector = new S12_GlueX();
  } else if (name == "S13-BIC") {
    detector = new S13_BIC();
  }
  return detector;
}
