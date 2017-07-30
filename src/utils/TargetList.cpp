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

TargetList::TargetList() {
}

TargetList::TargetList(const TargetList& orig) {
}

TargetList::~TargetList() {
}

void TargetList::collectTargetInfo(CSVRecord record) {
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

void TargetList::addToTargetInfoMap(CSVRecord record,
        SingleKey... singleKeywords) {
  Property[] nameValues = new Property[singleKeywords.length];
  for (int i = 0; i < singleKeywords.length; i++) {
    nameValues[i] = new Property(
            singleKeywords[i].attrName,
            singleKeywords[i].getCorrectValue(record));
  }
  addToTargetInfoMap(nameValues);
}

void TargetList::addToTargetInfoMap(Property... nameValues) {

  TargetInfo idInfo = new TargetList.TargetInfo(nameValues);
  long hashValue = hashOfValues(nameValues);
  targetInfoMap.put(hashValue, idInfo);
}


public

Instance getTargetObject(CSVRecord record, TargetKey key) {
  Instance instance = null;
  try {
    if (key instanceof SingleKey) {
      instance = getTargetObjectForKyes(record, (SingleKey) key);
    } else { // it's a composite keyword.
      instance = getTargetObjectForKyes(record, ((CompositeKey) key).keys);
    }
  } catch (Exception ex) {
    LOG.error("Error for key(s): {}", key.toString());
    //ex.printStackTrace();
    throw ex;
  }

  return instance;
}

Instance TargetList::getTargetObjectForKyes(CSVRecord record,
        SingleKey... keys) {
  //    Object[] values = new Property[keys.length];
  List<Object> values = new ArrayList<>();
  for (SingleKey key : keys) {
    //values[i] = record.get(key.rawFileAttrName);
    values.add(key.getCorrectValue(record));
  }
  return getTargetObject(values.toArray());
}

long TargetList::hashOfValues(Property... nameValues) {
  String value = "";
  for (Property nameValue : nameValues) {
    value += nameValue.attrValue.toString();
  }
  return value.hashCode();
}

long TargetList::hash(Object... values) {
  String value = "";
  for (Object obj : values) {
    value += obj.toString();
  }
  return value.hashCode();
}

objy::data::Object& TargetList::getTargetObject(Object... values) {
  long hashValue = hash(values);
  Instance instance = null;

  TargetInfo targetInfo = targetInfoMap.get(hashValue);
  if (targetInfo != null)
    instance = targetInfo.targetObject.getInstance();

  if (instance == null)
    LOG.info("Ivalid instance for values: {}", values);

  return instance;
}

void TargetList::fetchTargets() {
  TargetFinder targetFinder = new TargetFinder();

  ObjectTargetKeyBuilder targetKeyBuilder;
  ObjectTargetKey targetKey;
  com.objy.data.Class objyClass = targetClass.getObjyClass();
  for (TargetInfo targetInfo : targetInfoMap.values()) {
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

int TargetList::createMissingTargets() {
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
