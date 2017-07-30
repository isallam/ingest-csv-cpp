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


IngestMapper::IngestMapper() {
}

IngestMapper::IngestMapper(const IngestMapper& orig) {
}

IngestMapper::~IngestMapper() {
}

IngestMapper::IngestMapper(rapidjson::Document& json) {
  // construct needed information for processing data from the jsonObject
  className = json[ClassNameJSON].GetString();

  if (json.HasMember(ClassKeyJSON)) {
    rapidjson::Document::Array& jsonArray = json[ClassKeyJSON].GetArray();
    processClassKey(jsonArray);
  }

  if (json.HasMember(StringsJSON)) {
    rapidjson::Document::Array&  jsonArray = json[StringsJSON)].GetArray();
    processArray(jsonArray, stringAttributeMap);
  }

  if (json.HasMember(IntegersJSON)) {
    rapidjson::Document::Array&  jsonArray = json[IntegersJSON].GetArray();
    processArray(jsonArray, integerAttributeMap);
  }

  if (json.HasMember(FloatsJSON)) {
    rapidjson::Document::Array& jsonArray = json[FloatsJSON].GetArray();
    processArray(jsonArray, floatAttributeMap);
  }

  if (json.HasMember(DatesJSON)) {
    rapidjson::Document::Array&  jsonArray = json[DatesJSON].GetArray();
    processArray(jsonArray, dateAttributeMap);
  }

  if (json.HasMember(RelationshipsJSON)) {
    rapidjson::Document::Array&  jsonArray = json[RelationshipsJSON].GetArray();
    processRelationships(jsonArray);
  }

}

/**
 * 
 * @param jsonArray
 * @param stringAttributeMap 
 */
void IngestMapper::processArray(rapidjson::Document::Array&  jsonArray, 
        AttributeMapperMap& stringAttributeMap) {

  for (auto& element : jsonArray) {
    rapidjson::Document::Object& obj = element;
    std::string& schemaName = obj[SchemaNameJSON].GetString();
    std::string& rawName = obj[RawNameJSON].GetString();
    stringAttributeMap[schemaName] = rawName;
  }

}

void IngestMapper::processClassKey(rapidjson::Document::Array& jsonArray) {

  ClassAccessor classAccessor = SchemaManager.getInstance().getClassProxy(className);
  std::vector<rapidjson::Document::Object> keys;
  for (auto& keyElement : jsonArray) {
    keys.push_back(keyElement);
  }

  if (keys.size() > 1) // composite key
  {
    std::vector<SingleKey> singleKeys;
    for (auto& keyObj : keys) {
      std::string& keySchemaName = keyObj[SchemaNameJSON].GetString();
      std::string& keyRawName = keyObj[RawNameJSON].GetString();
      // get the type of the keySchemaName 
      objy::data::Attribute& attr = getAttribute(keySchemaName, classAccessor);

      SingleKey key = new SingleKey(keySchemaName, keyRawName,
              attr.attributeValueSpecification().logicalType());
      singleKeys.push_back(key);
    }
    classKey = new CompositeKey(singleKeys);
  } else {
    rapidjson::Document::Object keyObj = keys.at(0);
    std::string& keySchemaName = keyObj[SchemaNameJSON].GetString();
    std::string& keyRawName = keyObj[RawNameJSON].GetString();
    // get the type of the keySchemaName 
    objy::data::Attribute& attr = getAttribute(keySchemaName, classAccessor);
    classKey = new SingleKey(keySchemaName, keyRawName,
            attr.attributeValueSpecification().logicalType());
  }
}

void IngestMapper::processRelationships(rapidjson::Document::Array& jsonArray) {

  for (auto& element : jsonArray) {
    rapidjson::Document::Object& obj = element;
    std::string& relationshipName = obj[RelationshipNameJSON].GetString();
    std::string& toClass = obj[ToClassJSON].GetString();
    std::string toClassRelationshipName;
    if (obj.HasMember(ToClassRelationshipNameJSON)) {
      toClassRelationshipName = obj[ToClassRelationshipNameJSON].GetString();
    }

    // configure relationship.
    Relationship rel = new Relationship(toClass);

    ClassAccessor toClassAccessor = SchemaManager.getInstance().getClassProxy(toClass);

    rapidjson::Document::Array& keyArray = obj[KeyJSON].GetArray();
    std::vector<rapidjson::Document::Object> keys;
    for (auto& keyObj : keyArray) {
      keys.push_back(keyObj);
    }
    if (keys.size() > 1) // composite key
    {
      std::vector<SingleKey> singleKeys;
      for (auto& keyObj : keys) {
        std::string& keySchemaName = keyObj[SchemaNameJSON].GetString();
        std::string& keyRawName = keyObj[RawNameJSON].GetString();
        // get the type of the keySchemaName 
        objy::data::Attribute& attr = getAttribute(keySchemaName, toClassAccessor);
        SingleKey key = new SingleKey(keySchemaName, keyRawName,
                attr.attributeValueSpecification().logicalType());
        singleKeys.pop_back(key);
      }
      CompositeKey compositeKey = new CompositeKey(singleKeys);
      rel.add(compositeKey, relationshipName, toClassRelationshipName);
    } else {
      rapidjson::Document::Object& keyObj = keys.at(0);
      std::string&  keySchemaName = keyObj[SchemaNameJSON].GetString();
      std::string&  keyRawName = keyObj[RawNameJSON].GetString();
      // get the type of the keySchemaName 
      objy::data::Attribute& attr = getAttribute(keySchemaName, toClassAccessor);
      SingleKey key = new SingleKey(keySchemaName, keyRawName,
              attr.attributeValueSpecification().logicalType());
      rel.add(key, relationshipName, toClassRelationshipName);
    }
    relationshipList.push_back(rel);
  }

}

objy::data::Attribute& IngestMapper::getAttribute(std::string& keySchemaName, ClassAccessor& classAccessor) {
  objy::data::Attribute& attr = classAccessor.getAttribute(keySchemaName);
  if (attr.isNull()) {
    std::cerr << "Attribute: " keySchemaName 
              << " for Class: " << className << " is null" << std::endl;
  }
  return attr;
}

TargetList* IngestMapper::getClassTargetList() {
  if (classTargetList == NULL) {
    // initialize TargetList
    classTargetList = new TargetList(
            SchemaManager.getInstance().getClassProxy(className),
            classKey);
  }
  return classTargetList;
}

