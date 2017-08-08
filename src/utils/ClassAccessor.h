/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClassAccessor.h
 * Author: ibrahim
 *
 * Created on July 19, 2017, 5:51 PM
 */

#ifndef CLASSACCESSOR_H
#define CLASSACCESSOR_H

#include <string>
#include <map>
#include <vector>

using namespace std;

#include <ooObjy.h>
#include <objy/data/Data.h>
#include <objy/data/List.h>
#include <objy/data/Class.h>

#include "Property.h"
#include "Relationship.h"
#include "csv/CSVRecord.h"


namespace csv {

  typedef map<std::string, objy::data::Attribute> AttributeMap;
  typedef map<std::string, objy::data::Attribute>::iterator AttributeMapItr;

  class IngestMapper;

  class ClassAccessor {
  public:
    ClassAccessor(std::string className);
    ClassAccessor(const ClassAccessor& orig);
    virtual ~ClassAccessor();

    void init();
    objy::data::Attribute getAttribute(string attrName);

    void setMapper(csv::IngestMapper* ingestMapper) {
      mapper = ingestMapper;
    }

    string getClassName();

    objy::data::Class getObjyClass() {
      return classRef;
    }

    objy::data::Object createInstance() {
      //objectCreatedCounter++;
      return objy::data::createPersistentObject(classRef);
    }

    objy::data::Object createObject(CSVRecord record);
    objy::data::Object createObject(vector<Property> properties);
    objy::data::Object setAttributes(objy::data::Object instance, CSVRecord record);
    void setAttributeValue(objy::data::Object instance,
            string attributeName, objy::data::Object value);
    void setReference(objy::data::Object instance,
            string attributeName, objy::data::Object value);
    void addReference(objy::data::Object instance,
            string attributeName, objy::data::Object value);

  private:
    void setAttributeValue(objy::data::Object& instance,
            objy::data::Attribute& attribute, objy::data::Variable& value);
    void setReference(objy::data::Object& instance,
            objy::data::Attribute& attribute, objy::data::Object& value);
    bool doesListContainReference(objy::data::List& list, objy::data::Object& value);
    void addReferenceIfDoesnotExist(objy::data::Map& map, objy::data::Reference& objRef);


  private:

    objy::data::Class classRef;
    string className;
    AttributeMap attributeMap;
    IngestMapper* mapper;

  };
}
#endif /* CLASSACCESSOR_H */

