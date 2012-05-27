//
//  dataset.h
//  rrf-fs-prototype
//
//  Created by Abu Zaher Faridee on 5/27/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef rrf_fs_prototype_dataset_h
#define rrf_fs_prototype_dataset_h

#include "trainingset.h"

class DataSet{
public:
  DataSet(vector< vector<string> > sharedFileContent, vector< vector<string> > designFileContent): 
    sharedFileContent(sharedFileContent),
    designFileContent(designFileContent){
  }
  
  void createTrainingSets(){
    for(unsigned i = 3; i < sharedFileContent[0].size(); i++){
      featureLabels.push_back(sharedFileContent[0][i]);
    }
    
    string numOUTStr = sharedFileContent[1][2];
    numOTUs = atoi(numOUTStr.c_str());
    
    numTrainingSets = sharedFileContent.size() - 1;
    
    
    for (unsigned i = 1; i < sharedFileContent.size(); i++) {
      
      vector<int> tempOtuCounts;
      for(unsigned j = 3; j < sharedFileContent[i].size(); j++){
        int count = atoi(sharedFileContent[i][j].c_str());
        tempOtuCounts.push_back(count);
      }
      string tempOutputClass(designFileContent[i-1][1].c_str());
      TrainingSet trainingSet(tempOtuCounts, tempOutputClass);
      tempOtuCounts.clear();
      trainingSets.push_back(trainingSet);
    }
    
    createUniqIdForTrainignSets();
  }
  
  void printTrainingSets(){
    for(unsigned i = 0; i < trainingSets.size(); i++){
      TrainingSet trainingSet = trainingSets[i];
      for (unsigned j = 0; j < trainingSet.getOtuCounts().size(); j++) {
        cout << trainingSet.getOtuCounts()[j] << " ";
      }
      cout << trainingSet.getOutputClass() << endl;
    }
  }
  
private:
  vector<string> featureLabels;  
  unsigned numOTUs;
  unsigned numTrainingSets;
  vector<TrainingSet> trainingSets;
  
  vector< vector<string> > sharedFileContent;
  vector< vector<string> > designFileContent;
  
  void createUniqIdForTrainignSets(){
    vector<string> uniqOutputStrings;
    for (unsigned i = 0; i < trainingSets.size(); i++) {
      string outputClass = trainingSets[i].getOutputClass();
      bool found = false;
      for (unsigned j = 0; j < uniqOutputStrings.size(); j++) {
        if (outputClass == uniqOutputStrings[j]){
          found = true;
        }
      }
      if (found == false){
        uniqOutputStrings.push_back(outputClass);
      }
    }

    for (unsigned i = 0; i < trainingSets.size(); i++) {
      string outputClass = trainingSets[i].getOutputClass();
      for (unsigned j = 0; j < uniqOutputStrings.size(); j++) {
        if (outputClass == uniqOutputStrings[j]){
          trainingSets[i].setOutputClassId(j);
        }
      }
    }
  }
};


#endif
