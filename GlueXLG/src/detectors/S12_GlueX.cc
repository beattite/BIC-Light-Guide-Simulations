/*
 * S12_GlueX.cc
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#include "S12_GlueX.hh"
#include "../Materials.hh"
#include <G4SystemOfUnits.hh>

// More info here:
// https://www.hamamatsu.com/resources/pdf/ssd/s13360_series_kapd1052e.pdf

S12_GlueX::S12_GlueX() : AbsMPPC() {
  efficiencyPoints = { 279.1, 2.1, 284.9, 6.6, 288.4, 9.4, 295.4, 12.2, 308.1, 14.8, 323.2, 16.4, 336.0, 17.7, 342.9,
                       18.1, 360.3, 18.2, 377.8, 20.0, 388.2, 21.6, 402.1, 23.1, 426.5, 24.6, 452.0, 25.3, 483.4, 25.0,
                       501.9, 24.4, 515.9, 23.4, 539.1, 21.6, 564.6, 19.6, 593.6, 17.2, 613.3, 16.0, 630.8, 14.5, 645.8,
                       13.2, 672.5, 11.4, 699.2, 9.8, 723.6, 8.5, 754.9, 7.1, 790.9, 5.7, 836.2, 4.0, 872.1, 3.1, 896.5,
                       2.6 };
}

S12_GlueX::~S12_GlueX() {
  // TODO Auto-generated destructor stub
}

G4String S12_GlueX::getName() {
  return "S12-GlueX";
}

G4double S12_GlueX::getXYSize() {
  return 12.7 * mm;
}

G4double S12_GlueX::getZSize() {
  return 0.7 * mm; // 20*mm, increased to see photon tracks terminated
}

G4double S12_GlueX::getWindowThickness() {
  return 0.45 * mm;
}

G4double S12_GlueX::getCathodeThickness() {
  return getZSize() - getWindowThickness();
}

G4Material* S12_GlueX::getWindowMaterial() {
  return Materials::getInstance()->getMaterial("epoxy-resin");
}

G4Material* S12_GlueX::getCathodeMaterial() {
  return Materials::getInstance()->getMaterial("silicon-crystal-S13");
}
