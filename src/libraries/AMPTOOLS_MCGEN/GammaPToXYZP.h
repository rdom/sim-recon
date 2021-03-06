#if !defined(GAMMAPTOXYZP)
#define GAMMAPTOXYZP

/*
 *  GammaPToXYZP.h
 *  GlueXTools
 *
 *  Created by Matthew Shepherd on 5/25/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "TLorentzVector.h"

#include "AMPTOOLS_MCGEN/ProductionMechanism.h"

#include "TH1D.h"

class Kinematics;
class AmpVecs;

class GammaPToXYZP {
  
public:
  
  GammaPToXYZP( float lowMassXYZ, float highMassXYZ, 
                float massX, float massY, float massZ,
                ProductionMechanism::Type type, 
                float beamMaxE, float beamPeakE, float beamLowE, float beamHighE );
  
  Kinematics* generate();
//  AmpVecs* generateMany( int nEvents );
  
  void addResonance( float mass, float width, float bf );
  
private:
  
  ProductionMechanism m_prodMech;
  
  TLorentzVector m_beam;
  TLorentzVector m_target;
  
  vector< double > m_childMass;

  TH1D *cobrem_vs_E;
  
};

#endif
