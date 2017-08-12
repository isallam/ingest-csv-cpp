/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SchemaManager.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 12:31 PM
 */

#ifndef SCHEMAMANAGER_H
#define SCHEMAMANAGER_H

#include <map>
#include <string>
#include <iostream>

using namespace std;

namespace csv {

  class ClassAccessor;

  typedef std::map<string, ClassAccessor*> ClassAccessorMap;

  class SchemaManager {
  public:
    SchemaManager(const csv::SchemaManager& orig) = delete;
    virtual ~SchemaManager() {}

    static csv::SchemaManager* getInstance() {
      cout << "in getInstance()" << endl;
      if (!_instance) {
        cout << "creating instance of schema manager" << endl;
        _instance = new csv::SchemaManager();
      }
      cout << "returning instance" << endl;
      return _instance;
      
    }

    csv::ClassAccessor* getClassProxy(const string& className);

  private:
    static SchemaManager* _instance;
    SchemaManager() {}

    ClassAccessorMap      _classProxyMap;

  };

}

#endif /* SCHEMAMANAGER_H */

