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

  class IngestMapper;

  class ClassAccessor {
  public:
    ClassAccessor(const ClassAccessor& orig) = delete;
 
    virtual ~ClassAccessor() {
    }

    ClassAccessor(const std::string& name) :
    _className(name), _mapper(nullptr) {
    }

    ClassAccessor(ClassAccessor&& other) :
      _className(std::move(other._className)), 
      _mapper(other._mapper),
      _classRef(std::move(other._classRef)),
      _attributeMap(std::move(other._attributeMap))
    {
      other._mapper = nullptr;
    }

    void init();
    const objy::data::Attribute& getAttribute(const string& attrName) const;

    void setMapper(csv::IngestMapper* ingestMapper) {
      _mapper = ingestMapper;
    }

    string getClassName() const {
      return _className;
    }

    objy::data::Class getObjyClass() const {
      return _classRef;
    }

    objy::data::Object createInstance() const {
      //objectCreatedCounter++;
      return objy::data::createPersistentObject(_classRef);
    }

    objy::data::Object createObject(const csv::CSVRecord& record) const;
    objy::data::Object createObject(const vector<Property>& properties) const;
    void setAttributes(objy::data::Object& instance, 
            const csv::CSVRecord& record) const;
    void setAttribute(objy::data::Object& instance,
            const objy::data::Attribute& attribute, string value) const;
    
    void setAttributeValue(objy::data::Object& instance,
            const string& attributeName, const objy::data::Variable& value) const;
    void setReference(objy::data::Object& instance,
            const string& attributeName, const objy::data::Object& value) const;
    void addReference(objy::data::Object& instance,
            const string& attributeName, const objy::data::Object& value) const;

    bool getValue(const objy::data::Attribute& attr, objy::data::Variable& var, 
            std::string value) const;
    
  private:
    void setAttributeValue(objy::data::Object& instance,
            const objy::data::Attribute& attribute, const objy::data::Variable& value) const;
    void setReference(objy::data::Object& instance,
            const objy::data::Attribute& attribute, const objy::data::Object& value) const;
    bool doesListContainReference(const objy::data::List& list, const objy::data::Object& value) const;
    void addReferenceIfDoesnotExist(objy::data::Map& map, const objy::data::Reference& objRef) const;


  private:

    string            _className;
    IngestMapper*     _mapper;
    objy::data::Class _classRef;
    AttributeMap      _attributeMap;

  };
}
#endif /* CLASSACCESSOR_H */

