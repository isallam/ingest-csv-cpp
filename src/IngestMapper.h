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
    IngestMapper();
    IngestMapper(const IngestMapper& orig);
    virtual ~IngestMapper();

    IngestMapper(rapidjson::Document& json);

    std::string getClassName() {
      return className;
    }

    AttributeMapperMap& getStringsMap() {
      return stringAttributeMap;
    }

    AttributeMapperMap& getFloatMap() {
      return floatAttributeMap;
    }

    AttributeMapperMap& getIntegersMap() {
      return integerAttributeMap;
    }

    AttributeMapperMap& getDatesMap() {
      return dateAttributeMap;
    }

    RelationshipList& getRelationshipList() {
      return relationshipList;
    }

    bool hasRelationships() {
      return (!getRelationshipList().empty());
    }

    bool hasClassKey() {
      return classKey != NULL;
    }

    csv::TargetList* getClassTargetList();

    csv::TargetKey* getClassKey() {
      return classKey;
    }

  private:


  private:

    std::string className;
    csv::TargetKey* classKey;
    csv::TargetList* classTargetList;

    // map schema attribute names to raw data name
    AttributeMapperMap integerAttributeMap;
    AttributeMapperMap floatAttributeMap;
    AttributeMapperMap stringAttributeMap;
    AttributeMapperMap dateAttributeMap;

    RelationshipList relationshipList;

    /**
     * 
     * @param jsonArray
     * @param stringAttributeMap 
     */
    void processArray(rapidjson::Document::Array& jsonArray,
            AttributeMapperMap& stringAttributeMap);
    void processClassKey(rapidjson::Document::Array& jsonArray);
    void processRelationships(rapidjson::Document::Array& jsonArray);
    objy::data::Attribute getAttribute(std::string& keySchemaName, ClassAccessor* classAccessor);

  };

}


#endif /* INGESTMAPPER_H */

