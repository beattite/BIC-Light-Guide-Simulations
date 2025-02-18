/*
 * S12_GlueX.hh
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_S12_GlueX_HH_
#define INCLUDE_S12_GlueX_HH_

#include "AbsMPPC.hh"

class S12_GlueX: public AbsMPPC {
  public:
    S12_GlueX();
    virtual ~S12_GlueX();

    G4String getName() override;
    G4double getXYSize() override;
    G4double getZSize() override;
    G4double getWindowThickness() override;
    G4double getCathodeThickness() override;
    G4Material* getWindowMaterial() override;
    G4Material* getCathodeMaterial() override;
};

#endif /* INCLUDE_S12_GlueX_HH_ */
