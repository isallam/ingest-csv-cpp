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

SchemaManager::SchemaManager() {
}

SchemaManager::SchemaManager(const SchemaManager& orig) {
}

SchemaManager::~SchemaManager() {
}

SchemaManager* SchemaManager::getInstance() {
    if (schemaManagerInstance == std::nullptr_t) {
      schemaManagerInstance = new SchemaManager();
    }
    return schemaManagerInstance;
  }


ClassAccessor& SchemaManager::getClassProxy(std::string className) {
    ClassAccessor classAccessor = null;
    
    if (!classProxyMap.containsKey(className)) {
      classAccessor = new ClassAccessor(className);
      classAccessor.init();
      classProxyMap.put(className, classAccessor);
    }
    
    classAccessor = classProxyMap.get(className);
    return classAccessor;
  }
