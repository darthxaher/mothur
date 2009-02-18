#ifndef SOBS_H
#define SOBS_H

/*
 *  sobs.h
 *  Dotur
 *
 *  Created by Sarah Westcott on 1/7/09.
 *  Copyright 2009 Schloss Lab UMASS Amherst. All rights reserved.
 *
 */

/* This class implements the Sobs estimator on single group. 
It is a child of the calculator class. */


#include "calculator.h"

/***********************************************************************/

class Sobs : public Calculator {

public:
	Sobs() : Calculator("Sobs", 1) {};
	EstOutput getValues(SAbundVector* rank){
		data.resize(1,0);
		data[0] = (double)rank->getNumBins();
		return data;
	}
	EstOutput getValues(SharedRAbundVector* shared1, SharedRAbundVector* shared2) {return data;};
};

/***********************************************************************/

#endif