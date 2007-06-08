// $Id: DPi0_factory.cc 2496 2007-03-12 00:35:46Z kornicer $
//
//    File: DPi0_factory.cc
// Created: Tue Apr 17 11:57:50 EST 2007
// Creator: M. Kornicer (on Linux stan)
//

#include <math.h>

#include "DPi0.h"
#include "DPhoton.h"
#include "DPi0_factory.h"
#include "JANA/JEvent.h"


//----------------
// Constructor
//----------------
DPi0_factory::DPi0_factory()
{
	// Set defaults

}


//------------------
// evnt
// Pi0 facttory: loop over all pair combinations and make pi0
// 		 regatdless of the pair-mass at this point (MK)
//------------------
jerror_t DPi0_factory::evnt(JEventLoop *eventLoop, int eventnumber)
{
	vector<const DPhoton*> photons;
	eventLoop->Get(photons);
	
	// Loop over all photons and make pi0 candidates 
        for (unsigned int i = 0; i < photons.size() ; i++) {
          for (unsigned int j = i+1; j < photons.size() ; j++) {

                DLorentzVector P1 = photons[i]->getMom4();
                DLorentzVector P2 = photons[j]->getMom4();
                const unsigned int tag1 = photons[i]->getTag();
                const unsigned int tag2 = photons[j]->getTag();
		DPi0 *pi0 =  makePi0(P1, P2, tag1, tag2);

		_data.push_back(pi0);

           } 
        } 

	return NOERROR;
}

//------------------
// toString
//------------------
const string DPi0_factory::toString(void)
{
	// Ensure our Get method has been called so _data is up to date
	Get();
	if(_data.size()<=0)return string(); // don't print anything if we have no data!

	printheader("row:   E(GeV):   Px(cm):   Py(cm):   Pz(cm):   M(GeV):");
	
	for(unsigned int i=0; i<_data.size(); i++){
		DPi0 *pions = _data[i];

		printnewrow();
		printcol("%d",	i);
		printcol("%6.2f", pions->getMom4().T());
		printcol("%6.2f", pions->getMom4().X());
		printcol("%6.2f", pions->getMom4().Y());
		printcol("%6.2f", pions->getMom4().Z());
		printcol("%6.2f", pions->getMom4().M());
//		printcol("%5.2f", pi0s->getM());
//		printcol("%4.0f", fcalhit->t);
		printrow();
	}

	return _table;
}

// create pi0 candidate from two photons 
DPi0* DPi0_factory::makePi0(const TLorentzVector gamma1, const TLorentzVector gamma2, const unsigned int tag1, const unsigned int tag2) 
{

        DPi0* pi0 = new DPi0;
        
        pi0->setMom4(gamma1, gamma2);
        pi0->setTags(tag1, tag2);

        return pi0;
}

