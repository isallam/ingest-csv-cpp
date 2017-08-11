/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchemaManager.cpp
 * Author: ibrahim
 * 
 * Created on July 20, 2017, 12:31 PM
 */

#include "SchemaManager.h"
#include "ClassAccessor.h"

//csv::SchemaManager::_instance = nullptr;

csv::SchemaManager* csv::SchemaManager::getInstance() {
  if (!csv::SchemaManager::_instance) {
    csv::SchemaManager::_instance = new csv::SchemaManager();
  }
  return csv::SchemaManager::_instance;
}

csv::ClassAccessor*& csv::SchemaManager::getClassProxy(const string& className) {

  auto itr = _classProxyMap.find(className);
  if (itr == _classProxyMap.end()) {
    csv::ClassAccessor* classAccessor = new csv::ClassAccessor(className);
    classAccessor->init();
    _classProxyMap[className] = classAccessor;
    return classAccessor;
  }

  return itr->second;
}
