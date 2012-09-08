//
//  abstractdecisiontree.hpp
//  rrf-fs-prototype
//
//  Created by Abu Zaher Faridee on 7/22/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef rrf_fs_prototype_abstractdecisiontree_hpp
#define rrf_fs_prototype_abstractdecisiontree_hpp

#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <limits>
#include "macros.h"
#include "treenode.hpp"

#define DEBUG_MODE

using namespace std;

class AbstractDecisionTree{
  
public:
  
  AbstractDecisionTree(vector<vector<int> >baseDataSet, 
                       vector<int> globalDiscardedFeatureIndices, 
                       OptimumFeatureSubsetSelector optimumFeatureSubsetSelector, 
                       string treeSplitCriterion)
  
  : baseDataSet(baseDataSet),
  numSamples((int)baseDataSet.size()),
  numFeatures((int)(baseDataSet[0].size() - 1)),
  numOutputClasses(0),
  rootNode(NULL),
  globalDiscardedFeatureIndices(globalDiscardedFeatureIndices),
  optimumFeatureSubsetSize(optimumFeatureSubsetSelector.getOptimumFeatureSubsetSize(numFeatures)),
  treeSplitCriterion(treeSplitCriterion) {
    
      // TODO: istead of calculating this for every DecisionTree
      // clacualte this once in the RandomForest class and pass the values
    for (int i = 0;  i < numSamples; i++) {
      int outcome = baseDataSet[i][numFeatures];
      vector<int>::iterator it = find(outputClasses.begin(), outputClasses.end(), outcome);
      if (it == outputClasses.end()){       // find() will return classes.end() if the element is not found
        outputClasses.push_back(outcome);
        numOutputClasses++;
      }
    }
    
#ifdef DEBUG_MODE
    PRINT_VAR(outputClasses);
    PRINT_VAR(numOutputClasses);
#endif
  }
  
  virtual ~AbstractDecisionTree(){
//    if (rootNode != NULL){ delete rootNode; }
  }
  
  void createBootStrappedSamples(){
    
#ifdef DEBUG_LEVEL_3
    DEBUGMSG_FUNC;
#endif

    vector<bool> isInTrainingSamples(numSamples, false);
    
    for (int i = 0; i < numSamples; i++) {
        // TODO: optimize the rand() function call + double check if it's working properly
      int randomIndex = rand() % numSamples;
      bootstrappedTrainingSamples.push_back(baseDataSet[randomIndex]);
      isInTrainingSamples[randomIndex] = true;
    }
            
    for (int i = 0; i < numSamples; i++) {
      if (isInTrainingSamples[i]){ bootstrappedTrainingSampleIndices.push_back(i); }
      else{
        bootstrappedTestSamples.push_back(baseDataSet[i]);
        bootstrappedTestSampleIndices.push_back(i);
      }
    }
    
#ifdef DEBUG_LEVEL_3
    PRINT_VAR(numSamples);
    PRINT_VAR(bootstrappedTrainingSampleIndices);
    PRINT_VAR(bootstrappedTestSampleIndices);
#endif    
  }
  
  void getMinEntropyOfFeature(vector<int> featureVector, vector<int> outputVector, 
                              double& minEntropy, int& featureSplitValue, double& intrinsicValue){
    
#ifdef DEBUG_LEVEL_3
    DEBUGMSG_FUNC;
#endif
    
    vector< vector<int> > featureOutputPair(featureVector.size(), vector<int>(2, 0));
    for (int i = 0; i < featureVector.size(); i++) { 
      featureOutputPair[i][0] = featureVector[i];
      featureOutputPair[i][1] = outputVector[i];
    }
      // TODO: using default behavior to sort(), need to specify the comparator for added safety and compiler portability
    sort(featureOutputPair.begin(), featureOutputPair.end());
    
#ifdef DEBUG_LEVEL_3
    PRINT_VAR(featureOutputPair);
#endif
    
    vector<int> splitPoints;
    vector<int> uniqueFeatureValues(1, featureOutputPair[0][0]);
        
    for (int i = 0; i < featureOutputPair.size(); i++) {
      int featureValue = featureOutputPair[i][0];
      vector<int>::iterator it = find(uniqueFeatureValues.begin(), uniqueFeatureValues.end(), featureValue);
      if (it == uniqueFeatureValues.end()){                 // NOT FOUND
        uniqueFeatureValues.push_back(featureValue);
        splitPoints.push_back(i);
      }
    }
    
#ifdef DEBUG_LEVEL_4
    PRINT_VAR(splitPoints);
#endif
    
    int bestSplitIndex = -1;
    if (splitPoints.size() == 0){
        // TODO: trying out C++'s infitinity, don't know if this will work properly
        // TODO: check the caller function of this function, there check the value if minEntropy and comapre to inf
        // so that no wrong calculation is done
      minEntropy = numeric_limits<double>::infinity();                          // OUTPUT
      intrinsicValue = numeric_limits<double>::infinity();                      // OUTPUT
      featureSplitValue = -1;                                                   // OUTPUT
    }else{
      getBestSplitAndMinEntropy(featureOutputPair, splitPoints, minEntropy, bestSplitIndex, intrinsicValue);  // OUTPUT
      featureSplitValue = featureOutputPair[splitPoints[bestSplitIndex]][0];    // OUTPUT
    }
    
  }
  
  double calcIntrinsicValue(int numLessThanValueAtSplitPoint, int numGreaterThanValueAtSplitPoint, int numSamples) {
    
    double upperSplitEntropy = 0.0, lowerSplitEntropy = 0.0;
    if (numLessThanValueAtSplitPoint > 0) {
      upperSplitEntropy = numLessThanValueAtSplitPoint * log2((double) numLessThanValueAtSplitPoint / (double) numSamples);
    }
    
    if (numGreaterThanValueAtSplitPoint > 0) {
      lowerSplitEntropy = numGreaterThanValueAtSplitPoint * log2((double) numGreaterThanValueAtSplitPoint / (double) numSamples);
    }
    
    double intrinsicValue = - ((double)(upperSplitEntropy + lowerSplitEntropy) / (double)numSamples);
    return intrinsicValue;
  }
  
  
  void getBestSplitAndMinEntropy(vector< vector<int> > featureOutputPairs, vector<int> splitPoints,
                                 double& minEntropy, int& minEntropyIndex, double& relatedIntrinsicValue){
    
#ifdef DEBUG_LEVEL_3
    DEBUGMSG_FUNC;
#endif
    
    int numSamples = (int)featureOutputPairs.size();
    vector<double> entropies;
    vector<double> intrinsicValues;
    
    for (int i = 0; i < splitPoints.size(); i++) {
      int index = splitPoints[i];
      int valueAtSplitPoint = featureOutputPairs[index][0];
      int numLessThanValueAtSplitPoint = 0;
      int numGreaterThanValueAtSplitPoint = 0;
      
      for (int j = 0; j < featureOutputPairs.size(); j++) {
        vector<int> record = featureOutputPairs[j];
        if (record[0] < valueAtSplitPoint){ numLessThanValueAtSplitPoint++; }
        else{ numGreaterThanValueAtSplitPoint++; }
      }
      
      double upperEntropyOfSplit = calcSplitEntropy(featureOutputPairs, index, numOutputClasses, true);
      double lowerEntropyOfSplit = calcSplitEntropy(featureOutputPairs, index, numOutputClasses, false);
            
      double totalEntropy = (numLessThanValueAtSplitPoint * upperEntropyOfSplit + numGreaterThanValueAtSplitPoint * lowerEntropyOfSplit) / (double)numSamples;
      double intrinsicValue = calcIntrinsicValue(numLessThanValueAtSplitPoint, numGreaterThanValueAtSplitPoint, numSamples);
      entropies.push_back(totalEntropy);
      intrinsicValues.push_back(intrinsicValue);
      
#ifdef DEBUG_LEVEL_3
      PRINT_VAR(index);
      cout << "----------------------------" << endl;
      PRINT_VAR(upperEntropyOfSplit);
      PRINT_VAR(lowerEntropyOfSplit);
      PRINT_VAR(numLessThanValueAtSplitPoint);
      PRINT_VAR(numGreaterThanValueAtSplitPoint);
      PRINT_VAR(totalEntropy);
#endif

    }
    
#ifdef DEBUG_LEVEL_3
    PRINT_VAR(entropies);
    PRINT_VAR(intrinsicValues);
#endif
    
      // set output values
    vector<double>::iterator it = min_element(entropies.begin(), entropies.end());
    minEntropy = *it;                                                         // OUTPUT
    minEntropyIndex = (int)(it - entropies.begin());                          // OUTPUT
    relatedIntrinsicValue = intrinsicValues[minEntropyIndex];                 // OUTPUT

  }
    
  double calcSplitEntropy(vector< vector<int> > featureOutputPairs, int splitIndex, int numOutputClasses, bool isUpperSplit = true) {
    vector<int> classCounts(numOutputClasses, 0);
    
    if (isUpperSplit) { 
      for (int i = 0; i < splitIndex; i++) { classCounts[featureOutputPairs[i][1]]++; }
    } else {
      for (int i = splitIndex; i < featureOutputPairs.size(); i++) { classCounts[featureOutputPairs[i][1]]++; }
    }
    
    int totalClassCounts = accumulate(classCounts.begin(), classCounts.end(), 0);
    
    double splitEntropy = 0.0;
    
    for (int i = 0; i < classCounts.size(); i++) {
      if (classCounts[i] == 0) { continue; }
      double probability = (double) classCounts[i] / (double) totalClassCounts;
      splitEntropy += -(probability * log2(probability));
    }
    
    return splitEntropy;
  }
  
  void getSplitPopulation(TreeNode* node, vector< vector<int> >& leftChildSamples, vector< vector<int> >& rightChildSamples){    
      // TODO: there is a possibility of optimization if we can recycle the samples in each nodes
      // we just need to pointers to the samples i.e. vector<int> and use it everywhere and not create the sample 
      // sample over and over again
      // we need to make this const so that it is not modified by all the function calling
      // currently purgeTreeNodesDataRecursively() is used for the same purpose, but this can be avoided altogher
      // if re-using the same data over the classes
    
    int splitFeatureGlobalIndex = node->getSplitFeatureIndex();
    
    for (int i = 0; i < node->getBootstrappedTrainingSamples().size(); i++) {
      vector<int> sample =  node->getBootstrappedTrainingSamples()[i];
      if (sample[splitFeatureGlobalIndex] < node->getSplitFeatureValue()){ leftChildSamples.push_back(sample); }
      else{ rightChildSamples.push_back(sample); }
    }
  }
  
  
    // TODO: checkIfAlreadyClassified() verify code
    // TODO: use bootstrappedOutputVector for easier calculation instead of using getBootstrappedTrainingSamples()
  bool checkIfAlreadyClassified(TreeNode* treeNode, int& outputClass) {
    
#ifdef DEBUG_LEVEL_3
    DEBUGMSG_FUNC;
    PRINT_VAR(treeNode->bootstrappedTrainingSamples.size());
#endif
    
    vector<int> tempOutputClasses;
    for (int i = 0; i < treeNode->getBootstrappedTrainingSamples().size(); i++) {
      int sampleOutputClass = treeNode->getBootstrappedTrainingSamples()[i][numFeatures];
      vector<int>::iterator it = find(tempOutputClasses.begin(), tempOutputClasses.end(), sampleOutputClass);
      if (it == tempOutputClasses.end()) {               // NOT FOUND
        tempOutputClasses.push_back(sampleOutputClass);
      }
    }
    
    if (tempOutputClasses.size() < 2) { outputClass = tempOutputClasses[0]; return true; }
    else { outputClass = -1; return false; }
  }
  
  void setMothurOut(MothurOut* m){ this->m = m; }
  
protected:
  
  vector< vector<int> > baseDataSet;
  int numSamples;
  int numFeatures;
  int numOutputClasses;
  vector<int> outputClasses;
  vector< vector<int> > bootstrappedTrainingSamples;
  vector<int> bootstrappedTrainingSampleIndices;
  vector< vector<int> > bootstrappedTestSamples;
  vector<int> bootstrappedTestSampleIndices;
  
  TreeNode* rootNode;
  vector<int> globalDiscardedFeatureIndices;
  int optimumFeatureSubsetSize;
  string treeSplitCriterion;
  
  MothurOut* m;
  
private:
  
};

#endif
