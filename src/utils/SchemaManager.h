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

namespace csv {

  class ClassAccessor;

  typedef std::map<std::string, ClassAccessor*> ClassAccessorMap;
  typedef std::map<std::string, ClassAccessor*>::iterator ClassAccessorMapItr;

  class SchemaManager {
  public:
    SchemaManager();
    SchemaManager(const csv::SchemaManager& orig);
    virtual ~SchemaManager();

    static csv::SchemaManager* getInstance();

    csv::ClassAccessor* getClassProxy(std::string className);

  private:

    ClassAccessorMap      _classProxyMap;
    static SchemaManager* _instance;

  };

}

#endif /* SCHEMAMANAGER_H */

