/*
 *  GammaPToXYZP.cc
 *  GlueXTools
 *
 *  Created by Matthew Shepherd on 5/25/10.
 *  Copyright 2010 Home. All rights reserved.
 *
 */

#include "GammaPToXYZP.h"
#include "TLorentzVector.h"

#include "AMPTOOLS_MCGEN/DalitzDecayFactory.h"
#include "IUAmpTools/Kinematics.h"

// FORTRAN routines
extern "C"{
void cobrems_(float* Emax, float* Epeak, float* emitmr, float* radt, float* Dist, float* collDiam, int* doPolFluxfloat);
float dntdx_(float* x);
float dnidx_(float* x);
};

// Wrapper function for total
double dNtdx(double x)
{
    float xx = x;
    return (double)dntdx_(&xx);
}

double dNidx(double x)
{
    float xx = x;
    return (double)dnidx_(&xx);
}

GammaPToXYZP::GammaPToXYZP( float lowMassXYZ, float highMassXYZ, 
        float massX, float massY, float massZ,
        ProductionMechanism::Type type,
        float beamMaxE = 12.0, float beamPeakE = 9.0, float beamLowE = 7.0, float beamHighE = 12.0) : 
    m_prodMech( ProductionMechanism::kProton, type, 7.5 ), // last arg is t dependence
    m_target( 0, 0, 0, 0.938 ),
    m_childMass( 0 ) 
{

    m_childMass.push_back( massX );
    m_childMass.push_back( massY );
    m_childMass.push_back( massZ );

    m_prodMech.setMassRange( lowMassXYZ, highMassXYZ );
    // Initialize coherent brem table
    float Emax =  beamMaxE;
    float Epeak = beamPeakE;
    float Elow = beamLowE;
    float Ehigh = beamHighE;

    int doPolFlux=0;  // want total flux (1 for polarized flux)
    float emitmr=10.e-9; // electron beam emittance
    float radt=20.e-6; // radiator thickness in m
    float collDiam=0.0034; // meters
    float Dist = 76.0; // meters
    cobrems_(&Emax, &Epeak, &emitmr, &radt, &Dist, &collDiam, &doPolFlux);

    // Create histogram
    cobrem_vs_E = new TH1D("cobrem_vs_E", "Coherent Bremstrahlung vs. E_{#gamma}", 1000, Elow, Ehigh);

    // Fill histogram
    for(int i=1; i<=cobrem_vs_E->GetNbinsX(); i++){
        double x = cobrem_vs_E->GetBinCenter(i)/Emax;
        double y = 0;
        if(Epeak<Elow) y = dNidx(x);
        else y = dNtdx(x);
        cobrem_vs_E->SetBinContent(i, y);
    } 
}

Kinematics* 
GammaPToXYZP::generate(){

    double beamE = cobrem_vs_E->GetRandom();
    m_beam.SetPxPyPzE(0,0,beamE,beamE);

    TLorentzVector resonance = m_prodMech.produceResonance( m_beam );
    double genWeight = m_prodMech.getLastGeneratedWeight();

    vector< TLorentzVector > allPart;
    allPart.push_back( m_beam );
    allPart.push_back( m_beam + m_target - resonance );

    DalitzDecayFactory decay( resonance.M(), m_childMass );

    vector<TLorentzVector> fsPart = decay.generateDecay();

    for( vector<TLorentzVector>::iterator aPart = fsPart.begin();
            aPart != fsPart.end(); ++aPart ){

        aPart->Boost( resonance.BoostVector() );
        allPart.push_back( *aPart );
    }

    return new Kinematics( allPart, genWeight );
}

void
GammaPToXYZP::addResonance( float mass, float width, float bf ){

    m_prodMech.addResonance( mass, width, bf );
}

