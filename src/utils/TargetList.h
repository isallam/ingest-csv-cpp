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

class TargetList {
public:
  TargetList();
  TargetList(const TargetList& orig);
  virtual ~TargetList();

  TargetList(ClassAccessor* aTargetClass, std::vector<TargetKey>& theTargetKeys) {
    targetClass = aTargetClass;
    targetKeys = theTargetKeys;
  }
  TargetList(ClassAccessor* aTargetClass, TargetKey*& targetKey) {
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
    ObjectTarget targetObject;

    protected TargetInfo(Property... nameValues) {
      this.nameValues = nameValues;
    }
  };
  
  
  //---------------------
  // local attributes...
  //---------------------
  private HashMap<Long, TargetInfo> targetInfoMap = new HashMap<>();
  ClassAccessor* targetClass;
  std::vector<TargetKey> targetKeys;
  
  void addToTargetInfoMap(CSVRecord record, 
                        SingleKey... singleKeywords);
  void addToTargetInfoMap(Property... nameValues);
  objy::data::Object getTargetObjectForKyes(CSVRecord record, 
                        SingleKey... keys);
  objy::data::Object getTargetObject(Object... values);

  static long hashOfValues(Property...  nameValues);
  static long hash(Object...  values);
  
};

#endif /* TARGETLIST_H */

