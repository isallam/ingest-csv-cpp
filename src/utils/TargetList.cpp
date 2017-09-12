/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TargetList.cpp
 * Author: ibrahim
 * 
 * Created on July 20, 2017, 3:49 PM
 */

#include "TargetList.h"
#include "ClassAccessor.h"

#include <objy/target_finder/TargetFinder.h>
 
void csv::TargetList::collectTargetInfo(CSVRecord& record) {
  for (auto key : _targetKeys) {
    //System.out.println(" >> in collectTargetInfo() - key:" + key);
//    try {
      addToTargetInfoMap(record, key);
//    } catch (Exception ex) {
//      LOG.error("Error for keywords: {}", key.toString());
//      //ex.printStackTrace();
//      throw ex;
//    }
  }
}

void csv::TargetList::addToTargetInfoMap(CSVRecord& record,
        TargetKey* key) {
  vector<Property> nameValues = key->getProperties(record);
  addToTargetInfoMap(nameValues);
}

void csv::TargetList::addToTargetInfoMap(const vector<Property>& nameValues) {

  TargetInfo* targetInfo = new TargetInfo(nameValues);
  long hashValue = hashOfValues(nameValues);
  _targetInfoMap[hashValue] = targetInfo;
}


objy::data::Object  csv::TargetList::getTargetObject(
    const csv::CSVRecord& record, csv::TargetKey* key) {
  objy::data::Object  instance;
  // TBD... handle error 
 // try {
      instance = getTargetObjectForKey(record, key);
//  } catch (Exception ex) {
//    LOG.error("Error for key(s): {}", key.toString());
//    //ex.printStackTrace();
//    throw ex;
//  }

  return instance;
}

objy::data::Object csv::TargetList::getTargetObjectForKey(csv::CSVRecord record,
        TargetKey* key) {
  return getTargetObject(hash(key->getCorrectValue(record)));
}

long csv::TargetList::hashOfValues(vector<Property> nameValues) {
  string value = "";
  for (Property nameValue : nameValues) {
    value += nameValue.getValue();
  }
  return std::hash<std::string>()(value);
}

long csv::TargetList::hash(string value) {
  return std::hash<std::string>()(value);
}

objy::data::Object csv::TargetList::getTargetObject(long hashValue) {
  objy::data::Object instance;

  auto itr = _targetInfoMap.find(hashValue);
  if (itr == _targetInfoMap.end())
    throw std::invalid_argument("can't find entry in targetInfoMap");
  
  TargetInfo* targetInfo = itr->second;
  throw std::logic_error("finish getTargetObject() impl");
  //instance = targetInfo->_targetObject.getInstance();
  if (instance.isNull())
    throw std::invalid_argument("invalid instance for target.");

  return instance;
}

void csv::TargetList::fetchTargets() {
  objy::target_finder::TargetFinder targetFinder;

  objy::target_finder::ObjectTargetKeyHandle targetKey;
  objy::data::Class objyClass = _targetClass->getObjyClass();
  //cout << "working with objyClass: " << objyClass.name() << endl; 
  
  for (auto pair : _targetInfoMap) {
    auto targetInfo = pair.second;
  	objy::target_finder::ObjectTargetKeyBuilder targetKeyBuilder(objyClass);
    for (auto keyValuePair : targetInfo->_nameValues) {
    //        cout << "Add to targetKeyBuilder: " << keyValuePair.attrName 
    //             << ", val: " << keyValuePair.attrValue) << endl;
      const objy::data::Attribute& attr = _targetClass->getAttribute(keyValuePair.getName());
      objy::data::Variable var;
      if (_targetClass->getValue(attr, var, keyValuePair.getValue()))
        targetKeyBuilder.add(keyValuePair.getName().c_str(), var); 
    }
    targetKey = targetKeyBuilder.build();
    targetInfo->_targetObject = targetFinder.getObjectTarget(targetKey);
  }
  targetFinder.resolveTargets();
}

int csv::TargetList::createMissingTargets() {
  int count = 0;
  throw std::logic_error("finish createMissingTargets() impl");
  // iterate over the objectTargets and create data as needed.
//  for (auto itr = _targetInfoMap.begin(); itr != _targetInfoMap.end(); itr++)
//  {
//    TargetInfo* targetInfo = itr->second;
//    if (targetInfo->_targetObject.get() == NULL) {
//      // create the ID object
//      objy::data::Object idInstance = _targetClass.createObject(targetInfo->_nameValues);
//      targetInfo->_targetObject.setInstance(idInstance);
//      count++;
//    }
//  }
  return count;
}
