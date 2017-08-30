/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IngestMapper.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 11:37 AM
 */

#ifndef INGESTMAPPER_H
#define INGESTMAPPER_H

#include <map>
#include <string>
#include "utils/rapidjson/reader.h"
#include "utils/rapidjson/stringbuffer.h"
#include "utils/rapidjson/document.h"
#include "utils/Relationship.h"
#include "utils/TargetKey.h"
#include "utils/TargetList.h"
#include "utils/ClassAccessor.h"


namespace csv {

  typedef std::map<std::string, objy::data::Attribute> AttributeMapperMap;
  typedef std::vector<csv::Relationship*> RelationshipList;

  
  class ClassAccessor;

  namespace ingester {
    static const char* ClassNameJSON               = "ClassName";
    static const char* ClassKeyJSON                = "ClassKey";
    static const char* AttributesJSON              = "Attributes";
//    static const char* FloatsJSON                  = "Floats";
//    static const char* IntegersJSON                = "Integers";
//    static const char* DatesJSON                   = "Dates";
    static const char* RelationshipsJSON           = "Relationships";
    static const char* SchemaNameJSON              = "SchemaName";
    static const char* RawNameJSON                 = "RawName";
    static const char* RelationshipNameJSON        = "RelationshipName";
    static const char* ToClassJSON                 = "ToClass";
    static const char* ToClassRelationshipNameJSON = "ToClassRelationshipName";
    static const char* KeyJSON                     = "Key";
  }

  class IngestMapper {
  public:
    IngestMapper() {
        _classProxy = nullptr;              
	_classKey = nullptr;
	_classTargetList = nullptr;	
    }
    IngestMapper(const IngestMapper& orig) = delete;
    virtual ~IngestMapper() {
			if (_classKey != nullptr)
      	delete _classKey;
			if (_classTargetList != nullptr)
      	delete _classTargetList;
    }

    bool initialize(rapidjson::Document::Object json);

    const std::string& getClassName() const {
      return _className;
    }

    const csv::ClassAccessor* getClassProxy() const {
        return _classProxy;
    }
    
    const AttributeMapperMap& getAttributeMap() const {
      return _attributeMap;
    }

//    const AttributeMapperMap& getFloatMap() const {
//      return _floatAttributeMap;
//    }
//
//    const AttributeMapperMap& getIntegersMap() const {
//      return _integerAttributeMap;
//    }
//
//    const AttributeMapperMap& getDatesMap() const {
//      return _dateAttributeMap;
//    }
//
    const RelationshipList& getRelationshipList() const {
      return _relationshipList;
    }

    bool hasRelationships() const {
      return (!getRelationshipList().empty());
    }

    bool hasClassKey() const {
      return _classKey != nullptr;
    }

    csv::TargetList* getClassTargetList();

    csv::TargetKey* getClassKey() {
      return _classKey;
    }

   private:

    std::string         _className;
    csv::ClassAccessor* _classProxy;
    csv::TargetKey*     _classKey;
    csv::TargetList*    _classTargetList;

    // map schema attribute names to raw data name
//    AttributeMapperMap _integerAttributeMap;
//    AttributeMapperMap _floatAttributeMap;
    AttributeMapperMap _attributeMap;
//    AttributeMapperMap _dateAttributeMap;

    RelationshipList   _relationshipList;

    /**
     * 
     * @param jsonArray
     * @param stringAttributeMap 
     */
//    void processArray(rapidjson::Document::Array& jsonArray,
//            AttributeMapperMap& stringAttributeMap);
    void processClassKey(rapidjson::Document::Array& jsonArray);
    void processRelationships(rapidjson::Document::Array& jsonArray);

  };

}


#endif /* INGESTMAPPER_H */

