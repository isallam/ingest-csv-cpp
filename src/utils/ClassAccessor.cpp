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
#include "IngestMapper.h"

csv::ClassAccessor::ClassAccessor(const ClassAccessor& orig) {
}

csv::ClassAccessor::~ClassAccessor() {
}

csv::ClassAccessor::ClassAccessor(std::string name) {
  _className = name;
}

void csv::ClassAccessor::init() {
  _classRef = objy::data::lookupClass(_className.c_str());
  for (int i = 0; i < _classRef.numberOfAttributes(); i++)
  {
    objy::data::Attribute attr = _classRef.attribute(i);
    //Attribute attr = classRef.lookupAttribute(attrName);
    _attributeMap[attr.name()] = attr;
  }
}

objy::data::Attribute csv::ClassAccessor::getAttribute(const std::string attrName) const {
  AttributeMap::const_iterator itr = _attributeMap.find(attrName);
  if (itr == _attributeMap.end())
  {
    cerr << "Failed to get attribute: " << attrName 
            << " from ClassAccessor for class: " << _className << endl;
    throw std::invalid_argument("Failed to get attribute: " + attrName );
  }
  
  return itr->second;
}

objy::data::Object csv::ClassAccessor::createObject(CSVRecord record) {

  objy::data::Object instance;

  // check if we already have the instance
  if (_mapper->hasClassKey()) {
    instance = _mapper->getClassTargetList()->getTargetObject(record, *_mapper->getClassKey());
  }

  if (instance.isNull()) {
    instance = createInstance();
  }

  // iterate all relationships and resolve references
  for (auto& rel : _mapper->getRelationshipList()) {
    TargetList* relTarget = rel->getTargetList();
    for (csv::RelationshipRef relRef : rel->getRelationshipRefList()) {
      const objy::data::Object& refInstance = relTarget->getTargetObject(record, relRef.getKey());
      if (!refInstance.isNull())
      {
        setReference(instance, relRef.getRefAttrName(), refInstance);
      }
      if (!relRef.getRevRefAttrName().empty()) // we have a reverse attribute to set
      {
        relRef.getRevRefClassProxy()->addReference(refInstance, 
                relRef.getRevRefAttrName(), const_cast<objy::data::Object>(instance));
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
objy::data::Object csv::ClassAccessor::createObject(vector<Property> properties) {
  objy::data::Object&& instance = createInstance();
  for (Property property : properties) {
    setAttributeValue(instance, property.getName(), property.getValue());
  }
  return instance;
}

objy::data::Object& csv::ClassAccessor::setAttributes(
    objy::data::Object& instance, csv::CSVRecord& record) {


 AttributeMapperMap::iterator itr;
  string attrValueStr;

  // iterate and create any Integer attribute
  AttributeMapperMap& intMap = _mapper->getIntegersMap();
  itr = intMap.begin();

  while (itr != intMap.end())
  {
    try {
      long attrValue = 0;
      attrValueStr = record.get(itr->second);
      if (!attrValueStr.empty())
        attrValue = std::stol(attrValueStr);
      setAttributeValue(instance, itr->first, attrValue);
      itr++;
    } catch (std::invalid_argument& invalidEx) {
      cerr << "Invalid value for integer conversion: " << attrValueStr << endl;
      throw invalidEx;
   } catch (std::out_of_range& outEx) {
      cerr << "Invalid value for integer conversion: " << attrValueStr << endl;
      throw outEx;
    }
  }

  // iterate and create any Real atttribute
  AttributeMapperMap& floatMap = _mapper->getFloatMap();
  itr = floatMap.begin();
  
  while (itr != floatMap.end()) {
    try {
      double attrValue = 0;
      attrValueStr = record.get(itr->second);
      if (!attrValueStr.empty())
        attrValue = std::stod(attrValueStr);
      setAttributeValue(instance, itr->first, attrValue);
      itr++;
    } catch (std::invalid_argument& invalidEx) {
      cerr << "Invalid value for float conversion: " << attrValueStr << endl;
      throw invalidEx;
    } catch (std::out_of_range& outEx) {
      cerr << "Invalid value for float conversion: " << attrValueStr << endl;
      throw outEx;
    }

  }

  // iterate and create any string attribute
  AttributeMapperMap& strMap = _mapper->getStringsMap();
  itr = strMap.begin();
  
  while (itr != strMap.end()) {
    setAttributeValue(instance, itr->first, record.get(itr->second));
    itr++;
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
  AttributeMap::iterator itr = _attributeMap.find(attributeName);
  if (itr != _attributeMap.end())
  {
    setAttributeValue(instance, itr->second, value);
  } else {
    std::cerr << "Error for : " << instance.getClass(true).name() 
            << " -attr: " << attributeName << " is not in the attribute map" 
            << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance, 
        objy::data::Attribute& attribute, const objy::data::Object& value) {
  objy::data::Variable varValue;
  instance.attributeValue(attribute, varValue);

  objy::data::LogicalType::type attrLogicalType = 
          attribute.attributeValueSpecification()->logicalType();
  
  if ( attrLogicalType == objy::data::LogicalType::List) {
    objy::data::List& list = varValue.get<objy::data::List>();
    if (!doesListContainReference(list, value))
      list.append(objy::data::Variable(objy::data::createReference(value)));
  } else if (attrLogicalType == objy::data::LogicalType::Map) {
    const objy::data::Map& map = varValue.get<objy::data::Map>();
    addReferenceIfDoesnotExist(map, objy::data::createReference(value));
  } else if (attrLogicalType == objy::data::LogicalType::Reference) {
    varValue.set(objy::data::createReference(value));
  } else {
    std::cerr << "Illegal attribute type " << objy::data::LogicalType::toString(attrLogicalType) 
            << " for Instance value." << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance, 
        std::string& attributeName, const objy::data::Object& value) {
  objy::data::Attribute& attribute = _attributeMap.find(attributeName)->second;
  if (instance.isNull() || value.isNull() || attribute.isNull())
  {
    std::cerr << "For attr: " << attributeName 
            << " - instance/attribute/value: " << objy::data::oidFor(instance).sprint() 
            << " / " << attribute.name() << " / " << objy::data::oidFor(value).sprint()
            << std::endl;
  }
  setReference(instance, attribute, value);
}

void csv::ClassAccessor::addReference(objy::data::Object& instance, 
        std::string& attributeName, objy::data::Object& value) {
  objy::data::Attribute& attribute = _attributeMap.find(attributeName)->second;
  setReference(instance, attribute, value);
}

bool csv::ClassAccessor::doesListContainReference(objy::data::List& list, 
        const objy::data::Object& value) {
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

void csv::ClassAccessor::addReferenceIfDoesnotExist(const objy::data::Map& map, 
        objy::data::Reference& objRef) {
  // set the key and value in the map from the call object.... 
  ooId key = objy::data::oidFor(objRef);
  //System.out.println("call.getObjectId(): " + call.getObjectId().toString());
  if (!map.containsKey(key.sprint())) {
    objy::data::Variable value(objRef);
    map[key] = value;
  }
}