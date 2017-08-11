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
#include "TargetKey.h"


using namespace std;

namespace csv {

  class ClassAccessor;
  
  /******
   * Helper class to hold the target object reference for further processing.
   */
  struct TargetInfo {
    std::vector<Property> _nameValues;
    objy::target_finder::ObjectTargetHandle _targetObject;

    TargetInfo(const vector<Property>& nameValues) : _nameValues(nameValues){
    }
  };

  class TargetList {
  public:
    TargetList() = delete;
    TargetList(const TargetList& orig) = delete;
    TargetList(TargetList&& other) = delete;

    virtual ~TargetList() {
    }

    TargetList(const csv::ClassAccessor* targetClass, const vector<csv::TargetKey*>& targetKeys) :
    _targetClass(targetClass), _targetKeys(targetKeys) {
    }

    TargetList(const csv::ClassAccessor* targetClass, TargetKey* const targetKey) :
    _targetClass(targetClass) {
      _targetKeys.push_back(targetKey);
    }

    void collectTargetInfo(CSVRecord record);
    objy::data::Object getTargetObject(const csv::CSVRecord& record, TargetKey* key);
    void fetchTargets();
    int createMissingTargets();

  private:

    //---------------------
    // local attributes...
    //---------------------
    map<int64, TargetInfo*> _targetInfoMap;
    const csv::ClassAccessor* _targetClass;
    vector<TargetKey*> _targetKeys;

    void addToTargetInfoMap(CSVRecord record, TargetKey* key) {
      throw std::invalid_argument("calling the default addToTargetInfoMap()");
    }
    
    void addToTargetInfoMap(CSVRecord record,
            SingleKey* key);
    void addToTargetInfoMap(CSVRecord record,
            CompositeKey* key);

    void addToTargetInfoMap(const vector<Property>& nameValues);

    objy::data::Object getTargetObjectForKey(csv::CSVRecord record,
            csv::TargetKey* key);
  
    objy::data::Object getTargetObject(long hashValue);

    static long hashOfValues(Property nameValue);
    static long hashOfValues(vector<Property> nameValues);

    static long hash(string values);

  };
}
#endif /* TARGETLIST_H */

