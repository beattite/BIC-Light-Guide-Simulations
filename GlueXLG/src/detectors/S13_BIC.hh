/*
 * S13_BIC.hh
 *
 *  Created on: Feb 22, 2021
 *      Author: petrstepanov
 */

#ifndef INCLUDE_S13_BIC_HH_
#define INCLUDE_S13_BIC_HH_

#include "AbsMPPC.hh"

class S13_BIC: public AbsMPPC {
  public:
    S13_BIC();
    virtual ~S13_BIC();

    G4String getName() override;
    G4double getXYSize() override;
    G4double getZSize() override;
    G4double getWindowThickness() override;
    G4double getCathodeThickness() override;
    G4Material* getWindowMaterial() override;
    G4Material* getCathodeMaterial() override;
};

#endif /* INCLUDE_S13_BIC_HH_ */
