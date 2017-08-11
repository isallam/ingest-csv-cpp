/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IngestMapper.cpp
 * Author: ibrahim
 * 
 * Created on July 20, 2017, 11:37 AM
 */

#include <vector>
#include "IngestMapper.h"
#include "utils/ClassAccessor.h"

using namespace csv::ingester;

csv::IngestMapper::IngestMapper(rapidjson::Document::Object json) {
  // construct needed information for processing data from the jsonObject
  _className = json[ClassNameJSON].GetString();

  if (json.HasMember(ClassKeyJSON)) {
    rapidjson::Document::Array jsonArray = json[ClassKeyJSON].GetArray();
    processClassKey(jsonArray);
  }

  if (json.HasMember(StringsJSON)) {
    rapidjson::Document::Array  jsonArray = json[StringsJSON].GetArray();
    processArray(jsonArray, _stringAttributeMap);
  }

  if (json.HasMember(IntegersJSON)) {
    rapidjson::Document::Array  jsonArray = json[IntegersJSON].GetArray();
    processArray(jsonArray, _integerAttributeMap);
  }

  if (json.HasMember(FloatsJSON)) {
    rapidjson::Document::Array jsonArray = json[FloatsJSON].GetArray();
    processArray(jsonArray, _floatAttributeMap);
  }

  if (json.HasMember(DatesJSON)) {
    rapidjson::Document::Array  jsonArray = json[DatesJSON].GetArray();
    processArray(jsonArray, _dateAttributeMap);
  }

  if (json.HasMember(RelationshipsJSON)) {
    rapidjson::Document::Array  jsonArray = json[RelationshipsJSON].GetArray();
    processRelationships(jsonArray);
  }

}

/**
 * 
 * @param jsonArray
 * @param stringAttributeMap 
 */
void csv::IngestMapper::processArray(rapidjson::Document::Array&  jsonArray, 
        AttributeMapperMap& stringAttributeMap) {

  for (auto& element : jsonArray) {
    rapidjson::Document::Object obj = element.GetObject();
    std::string schemaName = obj[SchemaNameJSON].GetString();
    std::string rawName = obj[RawNameJSON].GetString();
    stringAttributeMap[schemaName] = rawName;
  }

}

void csv::IngestMapper::processClassKey(rapidjson::Document::Array& jsonArray) {

  csv::ClassAccessor*& classAccessor = csv::SchemaManager::getInstance()->getClassProxy(_className);
  std::vector<rapidjson::Document::Object> keys;
  for (auto& element : jsonArray) {
    rapidjson::Document::Object obj = element.GetObject();
    keys.push_back(obj);
  }

  if (keys.size() > 1) // composite key
  {
    std::vector<csv::SingleKey*> singleKeys;
    for (auto& keyObj : keys) {
      std::string keySchemaName = keyObj[SchemaNameJSON].GetString();
      std::string keyRawName = keyObj[RawNameJSON].GetString();
      // get the type of the keySchemaName 
      objy::data::Attribute attr = getAttribute(keySchemaName, classAccessor);

      SingleKey* key = new SingleKey(keySchemaName, keyRawName,
              attr.attributeValueSpecification()->logicalType());
      singleKeys.push_back(key);
    }
    _classKey = new csv::CompositeKey(singleKeys);
  } else {
    rapidjson::Document::Object keyObj = keys.at(0);
    std::string keySchemaName = keyObj[SchemaNameJSON].GetString();
    std::string keyRawName = keyObj[RawNameJSON].GetString();
    // get the type of the keySchemaName 
    objy::data::Attribute attr = getAttribute(keySchemaName, classAccessor);
    _classKey = new SingleKey(keySchemaName, keyRawName,
            attr.attributeValueSpecification()->logicalType());
  }
}

void csv::IngestMapper::processRelationships(rapidjson::Document::Array& jsonArray) {

  for (auto& element : jsonArray) {
    rapidjson::Document::Object obj = element.GetObject();
    std::string relationshipName = obj[RelationshipNameJSON].GetString();
    std::string toClass = obj[ToClassJSON].GetString();
    std::string toClassRelationshipName;
    if (obj.HasMember(ToClassRelationshipNameJSON)) {
      toClassRelationshipName = obj[ToClassRelationshipNameJSON].GetString();
    }

    // configure relationship.
    Relationship* rel = new Relationship(toClass);

    ClassAccessor*& toClassAccessor = csv::SchemaManager::getInstance()->getClassProxy(toClass);

    rapidjson::Document::Array keyArray = obj[KeyJSON].GetArray();
    std::vector<rapidjson::Document::Object> keys;
    for (auto& element : keyArray) {
      keys.push_back(element.GetObject());
    }
    if (keys.size() > 1) // composite key
    {
      std::vector<SingleKey*> singleKeys;
      for (auto& keyObj : keys) {
        std::string keySchemaName = keyObj[SchemaNameJSON].GetString();
        std::string keyRawName = keyObj[RawNameJSON].GetString();
        // get the type of the keySchemaName 
        objy::data::Attribute attr = getAttribute(keySchemaName, toClassAccessor);
        csv::SingleKey* key = new csv::SingleKey(keySchemaName, keyRawName,
                attr.attributeValueSpecification()->logicalType());
        singleKeys.push_back(key);
      }
      csv::CompositeKey* compositeKey = new csv::CompositeKey(singleKeys);
      rel->add(compositeKey, relationshipName, toClassRelationshipName, _className);
    } else {
      rapidjson::Document::Object& keyObj = keys.at(0);
      std::string  keySchemaName = keyObj[SchemaNameJSON].GetString();
      std::string  keyRawName = keyObj[RawNameJSON].GetString();
      // get the type of the keySchemaName 
      objy::data::Attribute attr = getAttribute(keySchemaName, toClassAccessor);
      csv::SingleKey* key = new csv::SingleKey(keySchemaName, keyRawName,
              attr.attributeValueSpecification()->logicalType());
      rel->add(key, relationshipName, toClassRelationshipName, _className);
    }
    _relationshipList.push_back(rel);
  }

}

objy::data::Attribute csv::IngestMapper::getAttribute(const string& keySchemaName, 
        ClassAccessor*& classAccessor) {
  objy::data::Attribute attr = classAccessor->getAttribute(keySchemaName);
  if (attr.isNull()) {
    std::cerr << "Attribute: " << keySchemaName 
              << " for Class: " << _className << " is null" << std::endl;
  }
  return attr;
}

csv::TargetList* csv::IngestMapper::getClassTargetList() {
  if (_classTargetList == nullptr) {
    // initialize TargetList
    _classTargetList = new TargetList(
            csv::SchemaManager::getInstance()->getClassProxy(_className),
            _classKey);
  }
  return _classTargetList;
}

