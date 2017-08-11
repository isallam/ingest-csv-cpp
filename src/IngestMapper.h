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


namespace csv {

  typedef std::map<std::string, std::string> AttributeMapperMap;
  typedef std::map<std::string, std::string>::iterator AttributeMapperMapItr;

  typedef std::vector<csv::Relationship*> RelationshipList;
  typedef std::vector<csv::Relationship*>::iterator RelatinshipListItr;

  
  class ClassAccessor;

  namespace ingester {
    static const char* ClassNameJSON               = "ClassName";
    static const char* ClassKeyJSON                = "ClassKey";
    static const char* StringsJSON                 = "Strings";
    static const char* FloatsJSON                  = "Floats";
    static const char* IntegersJSON                = "Integers";
    static const char* DatesJSON                   = "Dates";
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
    IngestMapper() = delete;
    IngestMapper(const IngestMapper& orig) = delete;
    virtual ~IngestMapper() {
      delete _classKey;
      delete _classTargetList;
    }

    IngestMapper(rapidjson::Document::Object json);

    const std::string& getClassName() const {
      return _className;
    }

    const AttributeMapperMap& getStringsMap() const {
      return _stringAttributeMap;
    }

    const AttributeMapperMap& getFloatMap() const {
      return _floatAttributeMap;
    }

    const AttributeMapperMap& getIntegersMap() const {
      return _integerAttributeMap;
    }

    const AttributeMapperMap& getDatesMap() const {
      return _dateAttributeMap;
    }

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

    std::string      _className;
    csv::TargetKey*  _classKey;
    csv::TargetList* _classTargetList;

    // map schema attribute names to raw data name
    AttributeMapperMap _integerAttributeMap;
    AttributeMapperMap _floatAttributeMap;
    AttributeMapperMap _stringAttributeMap;
    AttributeMapperMap _dateAttributeMap;

    RelationshipList   _relationshipList;

    /**
     * 
     * @param jsonArray
     * @param stringAttributeMap 
     */
    void processArray(rapidjson::Document::Array& jsonArray,
            AttributeMapperMap& stringAttributeMap);
    void processClassKey(rapidjson::Document::Array& jsonArray);
    void processRelationships(rapidjson::Document::Array& jsonArray);
    objy::data::Attribute getAttribute(const std::string& keySchemaName, 
                                      csv::ClassAccessor*& classAccessor);

  };

}


#endif /* INGESTMAPPER_H */

