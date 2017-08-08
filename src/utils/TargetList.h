/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TargetList.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 3:49 PM
 */

#ifndef TARGETLIST_H
#define TARGETLIST_H

#include <vector>
#include <objy/target_finder/TargetFinder.h>
#include <objy/target_finder/ObjectTarget.h>
#include "Property.h"
#include "ClassAccessor.h"
#include "TargetKey.h"


using namespace std;

namespace csv {

  class TargetList {
  public:
    TargetList();
    TargetList(const TargetList& orig);
    virtual ~TargetList();

    TargetList(csv::ClassAccessor* aTargetClass, std::vector<csv::TargetKey> theTargetKeys) {
      targetClass = aTargetClass;
      targetKeys = theTargetKeys;
    }

    TargetList(csv::ClassAccessor* aTargetClass, TargetKey*& targetKey) {
      targetClass = aTargetClass;
      targetKeys.push_back(*targetKey);
    }

    void collectTargetInfo(CSVRecord record);
    objy::data::Object getTargetObject(CSVRecord record, TargetKey& key);
    void fetchTargets();
    int createMissingTargets();

  private:

    /******
     * Helper class to hold the target object reference for further processing.
     */
    class TargetInfo {
      std::vector<Property> nameValues;
      objy::target_finder::ObjectTargetHandle targetObject;

      TargetInfo(vector<Property> nameValues) {
        nameValues = nameValues;
      }
    };


    //---------------------
    // local attributes...
    //---------------------
    std::map<int64, TargetInfo> targetInfoMap;
    csv::ClassAccessor* targetClass;
    std::vector<TargetKey> targetKeys;

    void addToTargetInfoMap(CSVRecord record,
            vector<SingleKey> singleKeywords);
    void addToTargetInfoMap(vector<Property> nameValues);
    objy::data::Object getTargetObjectForKyes(CSVRecord record,
            vector<SingleKey> keys);
    objy::data::Object getTargetObject(vector<string> values);

    static long hashOfValues(Property nameValue);
    static long hashOfValues(vector<Property> nameValues);
    static long hash(vector<string> values);

  };
}
#endif /* TARGETLIST_H */

