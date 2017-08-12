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

#include <iostream>
#include "SchemaManager.h"
#include "ClassAccessor.h"

csv::SchemaManager* csv::SchemaManager::_instance = new csv::SchemaManager();


//csv::SchemaManager* csv::SchemaManager::getInstance() {
//  cout << "in getInstance()" << endl;
//  if (!_instance) {
//    cout << "creating instance of schema manager" << endl;
//    _instance = new csv::SchemaManager();
//  }
//  cout << "returning instance" << endl;
//  return _instance;
//}

csv::ClassAccessor*& csv::SchemaManager::getClassProxy(const string& className) {

  cout << "in getClassProxy" << endl;
  auto itr = _classProxyMap.find(className);
  if (itr == _classProxyMap.end()) {
    csv::ClassAccessor* classAccessor = new csv::ClassAccessor(className);
    classAccessor->init();
    _classProxyMap[className] = classAccessor;
    return classAccessor;
  }
  cout << "... returning class Accessor" << endl;
  return itr->second;
}
