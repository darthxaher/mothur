#ifndef CALCULATOR_H
#define CALCULATOR_H

using namespace std;

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string>
#include <map>
#include <algorithm>
#include "utilities.hpp"
#include "sabundvector.hpp"
#include "sharedsabundvector.h"
#include "rabundvector.hpp"
#include "uvest.h"

/* The calculator class is the parent class for all the different estimators implemented in mothur except the tree calculators.
It has 2 pure functions EstOutput getValues(SAbundVector*), which works on a single group, and 
EstOutput getValues(SharedRAbundVector* shared1, SharedRAbundVector* shared2), which compares 2 groups. */ 


using namespace std;
typedef vector<double> EstOutput;

/***********************************************************************/

class Calculator {

public:
	Calculator(){};
	Calculator(string n, int c) : name(n), cols(c) {};
	virtual EstOutput getValues(SAbundVector*) = 0;	
	virtual EstOutput getValues(SharedRAbundVector* shared1, SharedRAbundVector* shared2) = 0;
	virtual void print(ostream& f)	{ f.setf(ios::fixed, ios::floatfield); f.setf(ios::showpoint);
									  f << data[0]; for(int i=1;i<data.size();i++){	f << '\t' << data[i];	}}
	virtual string getName()		{	return name;	}
	virtual int getCols()		{	return cols;	}
protected:
	EstOutput data;
	string name;
	int cols;

};

/**************************************************************************************************/
/*This Class holds all of the methods that manipulate vectors.
These methods are used in the other classes.
This class must be included if any of the other classes are to be used.*/

class VecCalc
{
	// The methods seen in the order here is how they are ordered throughout the class.
	public:
		void printElements(vector<double>); //This prints the values of the vector on one line with a space between each value.
		void printElements(vector<string>); //This prints the values of the vector on one line with a space between each value.
		int findString(vector<string>, string);//This returns the index of the given string in the given <string> vector, if the string does not exist in the vector it returns -1.
		double mean(vector<double>); //This returns the mean value of the vector.
		double stError(vector<double>); //This returns the standard error of the vector.
		double sumElements(vector<double>); //This returns the sum of all the values in the vector.
		double sumElements(vector<double>, int); //This returns the sum of all the values in the vector excluding those whose index is before the given index.  
		double findMax(vector<double>); //This returns the maximum value in the vector.
		double numNZ(vector<double>); //This returns the number of non-zero values in the vector.
		double numPos(vector<double>); //This returns the number of positive values in the vector.
		double findMaxDiff(vector<double>, vector<double>); //This returns the absolute value of the maximum difference between the two vectors.
		double findDStat(vector<double>, vector<double>, double); //This returns the D-Statistic of the two vectors with the given total number of species.
		vector<int> findQuartiles(vector<double>); //This returns a vector with the first element being the index of the lower quartile of the vector and the second element being the index of the upper quartile of the vector.
		vector<double> add(vector<double>, double); //This adds the given number to every element in the given vector and returns the new vector.
		vector<double> multiply(vector<double>, double); //This multiplies every element in the given vector by the given number and returns the new vector.
		vector<double> power(vector<double>, double); //This raises every element in the given vector to the given number and returns the new vector.
		vector<double> addVecs(vector<double>,vector<double>); //The given vectors must be the same size. This adds the ith element of the first given vector to the ith element of the second given vector and returns the new vector.
		vector<double> multVecs(vector<double>,vector<double>); //The given vectors must be the same size. This multiplies the ith element of the first given vector to the ith element of the second given vector and returns the new vector.
		vector<double> remDup(vector<double>); //This returns a vector that contains 1 of each unique element in the given vector. The order of the elements is not changed.
		vector<double> genCVec(vector<double>); //This returns a cumilative vector of the given vector. The ith element of the returned vector is the sum of all the elements in the given vector up to i.
		vector<double> genRelVec(vector<double>); //This finds the sum of all the elements in the given vector and then divides the ith element in the given vector by that sum and then puts the result into a new vector, which is returned after all of the elements in the given vector have been used.
		vector<double> genDiffVec(vector<double>, vector<double>);//This subtracts the ith element of the second given vector from the ith element of the first given vector and returns the new vector.
		vector<double> genCSVec(vector<double>);//This calculates the number of species that have the same number of individuals as the ith element of the given vector and then returns a cumulative vector.
		vector<double> genTotVec(vector<vector<double> >); //This adds up the ith element of all the columns and puts that value into a new vector. It those this for all the rows and then returns the new vector.
		vector<double> quicksort(vector<double>); //This sorts the given vector from highest to lowest and returns the sorted vector.
		vector<vector<double> > gen2DVec(vector<double>, int, int); //(vector, #rows/columns, 0 if the second parameter was rows, 1 if the second parameter was columns) Transforms a single vector that was formatted like a table into a 2D vector.
		vector<string> getSData(char[]);//This takes a file name as a parameter and reads all of the data in the file into a <string> vector.
};

/**************************************************************************************************/
/*This Class contains methods that return the B Diverstiy of two sets
of data. The four methods are the Whittaker's measure, the Marczewski-Stainhaus distance,
the Sorensen quantitative index, and the Morisita-Horn index.
The main method takes a number of columns of data and performs all 4 methods on each
combination of columns. It prints a table for every method that shows the B Diverstiy for 
each combination. It also calculates the overall diversity for Whittaker's measure and 
the Marczewski-Steinhaus distance.*/


class BDiversity
{
	public:
		void doBD(vector<double>, double);//Main method
		double getWhitt(vector<double>,vector<double>);//Whittacker's measure
		double getMS(vector<double>, vector<double>);//Marczewski-Stainhaus distance
		double getSor(vector<double>, vector<double>);//Sorensen quantitative index
		double getMor(vector<double>, vector<double>);//Morisita-Horn index
		void printD(vector<vector<double> >, int);//This prints a table that represents the given 2D vector, the second paramter specifies which method is to be used (1 for Whitt, 2 for MS, 3 for Sor, and 4 for Mor)
};

/**************************************************************************************************/

/*This Class is similar to the GeometricSeries.h class. It calculates
the broken stick distribution of the table and prints the D-Statistic 
and the confidence limits for the Kolmogorov-Smirnov 1-Sample test
with a 95% confidence level.*/

class BrokenStick
{
	public:
		void doBStick(vector<double>);
};

/**************************************************************************************************/
/*This Class calculates the geometric series distribution for the data.
It prints the D-Statistic and the critical values for the Kolmogorov-Smirnov
1-sample test at the 95% confidence interval.*/

class GeometricSeries
{
	public:
		void doGeomTest(vector<double>);
};

/**************************************************************************************************/
//This Class calculates the jackknifed estimate of the data and
//prints it and the confidence limits at a chosen confidence level.

class Jackknifing
{
	public:
		void doJK(vector<double>, double);
};
/**************************************************************************************************/
/*This Class stores calculates the Kolmogorov-Smirnov 2-Sample test between two samples.
It prints the D-Statistic and the critical value for the test at 
the 90% and 95% confidence interval.*/

class KS2SampleTest
{
	public:
		void doKSTest(vector<double>, vector<double>);
};
/**************************************************************************************************/
/*This Class calculates the Log Series Distribution for the data.
It then generates a D-Statistic and prints the D-Statistic and
the critical values for the Kolmogorov-Smirnov 1 sample test.*/

class LogSD
{
	public:
		void doLogSD(vector<double>, vector<double>);
};

/**************************************************************************************************/
//This Class calculates and prints the Q-Statistic for the data.
class QStatistic
{
	public:
		void doQStat(vector<double>);
};
/**************************************************************************************************/
class SSBPDiversityIndices
{
	public:
		void doSSBP(vector<double>);
		double getShan(vector<double> vec);//The Shannon Index
		double getSimp(vector<double> vec);//The Simpson Index
		double getBP(vector<double> vec);//The Berger-Parker Index
};
/**************************************************************************************************/
//This Class stores the table of the confidence limits of the Student-T distribution.
class TDTable
{
	public:
		double getConfLimit(int,int);
};

/**************************************************************************************************/
/*This Class calculates the truncated lognormal for the data.
It then prints the D-Statistic and the critical values for the
Kolmogorov-Smirnov 1-Sample test.*

class TrunLN
{
	public:
		void doTrunLN(vector<double>, vector<double>);
};
/**************************************************************************************************/

#endif
