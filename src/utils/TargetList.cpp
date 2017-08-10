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
#include <objy/target_finder/TargetFinder.h>
 

csv::TargetList::TargetList() {
}

csv::TargetList::TargetList(const TargetList& orig) {
}

csv::TargetList::~TargetList() {
}

void csv::TargetList::collectTargetInfo(CSVRecord record) {
  for (TargetKey key : targetKeys) {
    //System.out.println(" >> in collectTargetInfo() - key:" + key);
    try {
      if (key instanceof SingleKey) {
        addToTargetInfoMap(record, (SingleKey) key);
      } else { // it's a compositeKey.
        addToTargetInfoMap(record, ((CompositeKey) key).keys);
      }
    } catch (Exception ex) {
      LOG.error("Error for keywords: {}", key.toString());
      //ex.printStackTrace();
      throw ex;
    }
  }
}

void csv::TargetList::addToTargetInfoMap(CSVRecord record,
        vector<SingleKey> singleKeywords) {
  vector<Property> nameValues;
  for (int i = 0; i < singleKeywords.size(); i++) {
    nameValues.push_back(Property(
            singleKeywords[i].attrName,
            singleKeywords[i].getCorrectValue(record)));
  }
  addToTargetInfoMap(nameValues);
}

void csv::TargetList::addToTargetInfoMap(vector<Property> nameValues) {

  TargetInfo idInfo = new TargetList.TargetInfo(nameValues);
  long hashValue = hashOfValues(nameValues);
  targetInfoMap[hashValue] = idInfo;
}


objy::data::Object  csv::TargetList::getTargetObject(
    csv::CSVRecord record, csv::TargetKey& key) {
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
        SingleKey key) {
  return getTargetObject(hash(key.getCorrectValue(record)));
}

objy::data::Object csv::TargetList::getTargetObjectForKey(csv::CSVRecord record,
        CompositeKey key) {
  vector<string> values;
  for (SingleKey aKey : key.keys()) {
    //values[i] = record.get(key.rawFileAttrName);
    values.push_back(aKey.getCorrectValue(record));
  }
  return getTargetObject(hash(values));
}

long csv::TargetList::hashOfValues(vector<Property> nameValues) {
  string value = "";
  for (Property nameValue : nameValues) {
    value += nameValue.getValue();
  }
  return std::hash<std::string>()(value);
}

long csv::TargetList::hash(vector<string> values) {
  String value = "";
  for (auto& str : values) {
    value += str;
  }
  return std::hash<std::string>()(value);
}

objy::data::Object& csv::TargetList::getTargetObject(long hashValue) {
  objy::data::Object instance;

  TargetInfo targetInfo = targetInfoMap[hashValue];
  if (targetInfo != null)
    instance = targetInfo.targetObject.getInstance();

  if (instance == null)
    LOG.info("Ivalid instance for values: {}", values);

  return instance;
}

void csv::TargetList::fetchTargets() {
  objy::target_finder::TargetFinder targetFinder;

  objy::target_finder::ObjectTargetKeyHandle targetKey;
  objy::data::Class& objyClass = targetClass.getObjyClass();
  objy::target_finder::ObjectTargetKeyBuilder targetKeyBuilder(objyClass);
  
  for (TargetInfo targetInfo : targetInfoMap) {
     targetKeyBuilder = new ObjectTargetKeyBuilder(objyClass);
    for (Property keyValuePair : targetInfo.nameValues) {
      //        System.out.println("Add to targetKeyBuilder: " + keyValuePair.attrName +
      //                ", val: " + keyValuePair.attrValue);
      targetKeyBuilder.add(keyValuePair.attrName, new Variable(keyValuePair.attrValue));
    }
    targetKey = targetKeyBuilder.build();
    targetInfo.targetObject = targetFinder.getObjectTarget(targetKey);
  }
  targetFinder.resolveTargets();
}

int csv::TargetList::createMissingTargets() {
  int count = 0;
  // iterate over the objectTargets and create data as needed.
  for (TargetInfo targetInfo : targetInfoMap.values()) {
    if (targetInfo.targetObject.getInstance() == null) {
      // create the ID object
      objy::data::Object idInstance = targetClass.createObject(targetInfo.nameValues);
      targetInfo.targetObject.setInstance(idInstance);
      count++;
    }
  }
  return count;
}
