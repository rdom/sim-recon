// $Id$
//
//    File: DEventRFBunch_factory_Thrown.cc
// Created: Thu Dec  3 17:27:55 EST 2009
// Creator: pmatt (on Linux ifarml6 2.6.18-128.el5 x86_64)
//

#include "DEventRFBunch_factory_Thrown.h"
#include <deque>

using namespace jana;

//------------------
// init
//------------------
jerror_t DEventRFBunch_factory_Thrown::init(void)
{
	return NOERROR;
}

//------------------
// brun
//------------------
jerror_t DEventRFBunch_factory_Thrown::brun(jana::JEventLoop *locEventLoop, int32_t runnumber)
{
	return NOERROR;
}

//------------------
// evnt
//------------------
jerror_t DEventRFBunch_factory_Thrown::evnt(jana::JEventLoop *locEventLoop, uint64_t eventnumber)
{
	vector<const DMCThrown*> locMCThrowns;
	locEventLoop->Get(locMCThrowns);
	if(locMCThrowns.empty())
		return NOERROR; //not a MC event!

	vector<const DRFTime*> locRFTimes;
	locEventLoop->Get(locRFTimes, "TRUTH");
	if(locRFTimes.empty())
	{
		DEventRFBunch *locEventRFBunch = new DEventRFBunch;
		locEventRFBunch->dTime = numeric_limits<double>::quiet_NaN();
		locEventRFBunch->dTimeVariance = numeric_limits<double>::quiet_NaN();
		locEventRFBunch->dNumParticleVotes = 0;
		locEventRFBunch->dTimeSource = SYS_NULL;
		_data.push_back(locEventRFBunch);
		return NOERROR;
	}

	DEventRFBunch *locEventRFBunch = new DEventRFBunch;
	locEventRFBunch->dTime = locRFTimes[0]->dTime;
	locEventRFBunch->dTimeVariance = 0.0;
	locEventRFBunch->dNumParticleVotes = 0;
	locEventRFBunch->dTimeSource = SYS_RF;
	_data.push_back(locEventRFBunch);

	return NOERROR;
}

//------------------
// erun
//------------------
jerror_t DEventRFBunch_factory_Thrown::erun(void)
{
	return NOERROR;
}

//------------------
// fini
//------------------
jerror_t DEventRFBunch_factory_Thrown::fini(void)
{
	return NOERROR;
}

