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

csv::SchemaManager::SchemaManager() {
}

csv::SchemaManager::SchemaManager(const SchemaManager& orig) {
}

csv::SchemaManager::~SchemaManager() {
}

csv::SchemaManager* csv::SchemaManager::getInstance() {
  if (!csv::SchemaManager::_instance) {
    csv::SchemaManager::_instance = new csv::SchemaManager();
  }
  return csv::SchemaManager::_instance;
}

csv::ClassAccessor* csv::SchemaManager::getClassProxy(std::string className) {
  csv::ClassAccessor* classAccessor = nullptr;

  if (_classProxyMap.find(className) == _classProxyMap.end()) {
    classAccessor = new csv::ClassAccessor(className);
    classAccessor->init();
    _classProxyMap[className] = classAccessor;
  }

  if (classAccessor == nullptr)
    classAccessor = _classProxyMap[className];
  
  return classAccessor;
}
