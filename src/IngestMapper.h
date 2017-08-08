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
#include "utils/ClassAccessor.h"
#include "utils/rapidjson/reader.h"
#include "utils/rapidjson/stringbuffer.h"
#include "utils/rapidjson/document.h"
#include "utils/Relationship.h"
#include "utils/TargetKey.h"
#include "utils/TargetList.h"

typedef std::map<std::string, std::string> AttributeMapperMap;
typedef std::map<std::string, std::string>::iterator AttributeMapperMapItr;

typedef std::vector<Relationship> RelationshipList;
typedef std::vector<Relationship>::iterator RelatinshipListItr;

namespace csv {
  namespace ingester {
    static const std::string ClassNameJSON = "ClassName";
    static const std::string ClassKeyJSON = "ClassKey";
    static const std::string StringsJSON = "Strings";
    static const std::string FloatsJSON = "Floats";
    static const std::string IntegersJSON = "Integers";
    static const std::string DatesJSON = "Dates";
    static const std::string RelationshipsJSON = "Relationships";
    static const std::string SchemaNameJSON = "SchemaName";
    static const std::string RawNameJSON = "RawName";
    static const std::string RelationshipNameJSON = "RelationshipName";
    static const std::string ToClassJSON = "ToClass";
    static const std::string ToClassRelationshipNameJSON = "ToClassRelationshipName";
    static const std::string KeyJSON = "Key";

    class IngestMapper {
    public:
      IngestMapper();
      IngestMapper(const IngestMapper& orig);
      virtual ~IngestMapper();

      IngestMapper(rapidjson::Document json);

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

      TargetList* getClassTargetList();

      TargetKey* getClassKey() {
        return classKey;
      }

    private:


    private:

      std::string className;
      TargetKey* classKey;
      TargetList* classTargetList;

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
      objy::data::Attribute getAttribute(std::string& keySchemaName, ClassAccessor& classAccessor);

    };

  }
}


#endif /* INGESTMAPPER_H */

