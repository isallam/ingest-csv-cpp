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


objy::data::Reference  csv::TargetList::getTargetObject(
    const csv::CSVRecord& record, csv::TargetKey* key) {
  objy::data::Reference  reference;
  // TBD... handle error 
 // try {
      reference = getTargetObjectForKey(record, key);
//  } catch (std::exception ex) {
//    LOG.error("Error for key(s): {}", key.toString());
//    //ex.printStackTrace();
//    throw ex;
//  }

  return reference;
}

objy::data::Reference csv::TargetList::getTargetObjectForKey(csv::CSVRecord record,
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

objy::data::Reference csv::TargetList::getTargetObject(long hashValue) {
  objy::data::Reference reference;

  auto itr = _targetInfoMap.find(hashValue);
  if (itr == _targetInfoMap.end())
    throw std::invalid_argument("can't find entry in targetInfoMap");
  
  TargetInfo* targetInfo = itr->second;
  //throw std::logic_error("finish getTargetObject() impl");
  reference = targetInfo->_targetObject->reference();
  if (reference.isNull())
    throw std::invalid_argument("invalid instance for target.");
  
  return reference;
}

void csv::TargetList::fetchTargets() {
  objy::target_finder::TargetFinder targetFinder;

  objy::target_finder::ObjectTargetKeyHandle targetKey;
  objy::data::Class objyClass = _targetClass->getObjyClass();
  //cout << "working with objyClass: " << objyClass.name() << endl; 
  
  try {
    for (auto pair : _targetInfoMap) {
      auto targetInfo = pair.second;
      objy::target_finder::ObjectTargetKeyBuilder targetKeyBuilder(objyClass);
      for (auto keyValuePair : targetInfo->_nameValues) {
      //        cout << "Add to targetKeyBuilder: " << keyValuePair.attrName 
      //             << ", val: " << keyValuePair.attrValue) << endl;
        const objy::data::Attribute& attr = _targetClass->getAttribute(keyValuePair.getName());
        objy::data::Variable var;
        if (_targetClass->getValue(attr, var, keyValuePair.getValue()))
        {
            targetKeyBuilder.add(keyValuePair.getName().c_str(), var); 
        }
      }
      
      targetKey = targetKeyBuilder.build();
      targetInfo->_targetObject = targetFinder.getObjectTarget(targetKey);
    }
    targetFinder.resolveTargets();
  } catch (std::exception& ex) {
    cerr << "csv::TargetList::fetchTargets() Exception: " << ex.what() << endl;
    throw ex;
  }
}

int csv::TargetList::createMissingTargets() {
  int count = 0;
  //throw std::logic_error("finish createMissingTargets() impl");
  // iterate over the objectTargets and create data as needed.
  for (auto pair: _targetInfoMap)
  {
    auto targetInfo = pair.second;
    if (!targetInfo->_targetObject->reference()) {
      // create the object
      //cout << "creating object : " << count << endl;
      objy::data::Object idInstance = _targetClass->createObject(targetInfo->_nameValues);
      targetInfo->_targetObject->setReference(idInstance);
      count++;
    }
  }
  return count;
}
