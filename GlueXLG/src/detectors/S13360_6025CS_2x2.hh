/*
 * S13360_6025CS_2x2.hh
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_S13360_6025CS_2x2_HH_
#define INCLUDE_S13360_6025CS_2x2_HH_

#include "AbsMPPC.hh"

class S13360_6025CS_2x2: public AbsMPPC {
  public:
    S13360_6025CS_2x2();
    virtual ~S13360_6025CS_2x2();

    G4String getName() override;
    G4double getXYSize() override;
    G4double getZSize() override;
    G4double getWindowThickness() override;
    G4double getCathodeThickness() override;
    G4Material* getWindowMaterial() override;
    G4Material* getCathodeMaterial() override;
};

#endif /* INCLUDE_S13360_6025CS_2x2_HH_ */
