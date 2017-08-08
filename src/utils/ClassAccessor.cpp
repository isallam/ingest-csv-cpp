/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ClassAccessor.cpp
 * Author: ibrahim
 * 
 * Created on July 19, 2017, 5:51 PM
 */

#include "ClassAccessor.h"

csv::ClassAccessor::ClassAccessor(const ClassAccessor& orig) {
}

csv::ClassAccessor::~ClassAccessor() {
}

csv::ClassAccessor::ClassAccessor(std::string name) {
  className = name;
}

void csv::ClassAccessor::init() {
  classRef = objy::data::lookupClass(this.className);
  for (int i = 0; i < classRef.numberOfAttributes(); i++)
  {
    objy::data::Attribute attr = classRef.attribute(i);
    //Attribute attr = classRef.lookupAttribute(attrName);
    attributeMap[attr.name()] = attr;
  }
}

objy::data::Attribute csv::ClassAccessor::getAttribute(std::string attrName) const {
  AttributeMapItr itr = attributeMap.find(attrName);
  return *itr;
}

objy::data::Object csv::ClassAccessor::createObject(CSVRecord record) {

  objy::data::Object instance;

  // check if we already have the instance
  if (mapper.hasClassKey()) {
    instance = mapper.getClassTargetList().getTargetObject(record, mapper.getClassKey());
  }

  if (instance.isNull()) {
    instance = createInstance();
  }

  // iterate all relationships and resolve references
  for (Relationship rel : mapper.getRelationshipList()) {
    TargetList relTarget = rel.getTargetList();
    for (csv::Relationship::RelationshipRef relRef : rel.getRelationshipRefList()) {
      objy::data::Object& refInstance = relTarget.getTargetObject(record, relRef.getKey());
      if (!refInstance.isNull())
      {
        setReference(instance, relRef.getRefAttrName(), refInstance);
      }
      if (!relRef.getRevRefAttrName().empty()) // we have a reverse attribute to set
      {
        relRef.getRevRefClassProxy().addReference(refInstance, relRef.getRevRefAttrName(), instance);
      }
    }
  }

  setAttributes(instance, record);

  return instance;
}

/**
 *  Create an instance based on the list of properties passed.
 *  This will do a partial creation of some objects.
 * 
 * @param properties
 * @return new created Instance
 */
objy::data::Object csv::ClassAccessor::createObject(Property... properties) {
  objy::data::Object& instance = createInstance();
  for (Property property : properties) {
    setAttributeValue(instance, property.getName(), property.getValue());
  }
  return instance;
}

objy::data::Object& csv::ClassAccessor::setAttributes(objy::data::Object& instance, CSVRecord record) {


  // iterate and create any Integer attribute
  for (Map.Entry<String, String> entry : mapper.getIntegersMap().entrySet()) {
    //      System.out.println("Entry()" + entry.toString());
    try {
      long attrValue = 0;
      String attrValueStr = record.get(entry.getValue());
      if (!attrValueStr.equals(""))
        attrValue = Long.parseLong(attrValueStr);
      setAttributeValue(instance, entry.getKey(), attrValue);
    } catch (NumberFormatException nfEx) {
      //        System.out.println("... entry: " + entry.getValue() + " for raw: " + entry.getKey());
      nfEx.printStackTrace();
      throw nfEx;
    }
  }

  // iterate and create any Real atttribute
  for (Map.Entry<String, String> entry : mapper.getFloatMap().entrySet()) {
    //      System.out.println("Entry()" + entry.toString());
    try {
      double attrValue = Double.parseDouble(record.get(entry.getValue()));
      setAttributeValue(instance, entry.getKey(), attrValue);
    } catch (NumberFormatException nfEx) {
      nfEx.printStackTrace();
      throw nfEx;
    }
  }

  // iterate and create any string attribute
  for (Map.Entry<String, String> entry : mapper.getStringsMap().entrySet()) {
    //      System.out.println("Entry()" + entry.toString());
    setAttributeValue(instance, entry.getKey(), record.get(entry.getValue()));
  }

  return instance;
}

void csv::ClassAccessor::setAttributeValue(objy::data::Object& instance, 
        objy::data::Attribute& attribute, objy::data::Variable& value) {
  objy::data::Variable varValue;
  instance.attributeValue(attribute, varValue);
  varValue.set(value);
}

void csv::ClassAccessor::setAttributeValue(objy::data::Object& instance, 
        std::string& attributeName, objy::data::Variable& value) {
  AttributeMapItr itr = attributeMap.find(attributeName);
  if (itr != attributeMap.end())
  {
    setAttributeValue(instance, itr->second, value);
  } else {
    std::cerr << "Error for : " << instance.getClass(true).name() 
            << " -attr: " << attributeName << " is not in the attribute map" 
            << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance, 
        objy::data::Attribute& attribute, objy::data::Object& value) {
  objy::data::Variable varValue;
  instance.attributeValue(attribute, varValue);

  objy::data::LogicalType attrLogicalType = attribute.attributeValueSpecification().logicalType();
  if ( attrLogicalType == objy::data::LogicalType::List) {
    objy::data::List& list = varValue.get<objy::data::List>();
    if (!doesListContainReference(list, value))
      list.append(objy::data::Variable(objy::data::createReference(value)));
  } else if (attrLogicalType == objy::data::LogicalType::Map) {
    objy::data::Map& map = varValue.get<objy::data::Map>();
    addReferenceIfDoesnotExist(map, objy::data::createReference(value));
  } else if (attrLogicalType == objy::data::LogicalType::Reference) {
    varValue.set(objy::data::createReference(value));
  } else {
    std::cerr << "Illegal attribute type " << objy::data::LogicalType::toString(attrLogicalType) 
            << " for Instance value." << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance, 
        std::string& attributeName, objy::data::Object& value) {
  objy::data::Attribute& attribute = attributeMap.find(attributeName)->second;
  if (instance.isNull() || value.isNull() || attribute.isNull())
  {
    std::cerr << "For attr: " << attributeName 
            << " - instance/attribute/value: " << objy::data::oidFor(instance).sprint() 
            << " / " + attribute.name() + " / " + objy::data::oidFor(value).sprint()
            << std::endl;
  }
  setReference(instance, attribute, value);
}

void csv::ClassAccessor::addReference(objy::data::Object& instance, 
        std::string& attributeName, objy::data::Object& value) {
  objy::data::Attribute& attribute = attributeMap.find(attributeName)->second;
  setReference(instance, attribute, value);
}

bool csv::ClassAccessor::doesListContainReference(objy::data::List& list, 
        objy::data::Object& value) {
  objy::data::Variable var;
  objy::uint_64 valueOid = value.identifier().get<objy::uint_64>();
  for (int i = 0; i < list.size(); i++) {
    list.elementAt(i, var);
    objy::uint_64 refOid = var.get<objy::data::Reference>().identifier().get<objy::uint_64>();
    if (refOid == valueOid) {
      return true;
    }
  }

  return false;
}

void csv::ClassAccessor::addReferenceIfDoesnotExist(objy::data::Map& map, 
        objy::data::Reference& objRef) {
  // set the key and value in the map from the call object.... 
  ooId key = objy::data::oidFor(objRef);
  //System.out.println("call.getObjectId(): " + call.getObjectId().toString());
  if (!map.containsKey(key.sprint())) {
    objy::data::Variable value(objRef);
    map[key] = value;
  }
}