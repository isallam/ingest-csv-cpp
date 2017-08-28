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
#include <algorithm>
#include <string>

#include "ClassAccessor.h"
#include "IngestMapper.h"

using namespace std;

void csv::ClassAccessor::init() {
  //cout << "locating class: " << _className.c_str() << endl;
  objy::data::Class clazz = objy::data::lookupClass(_className.c_str());
  _classRef = clazz;
  objy::data::Sequence seq = _classRef.attributes();
  objy::data::Attribute geo = _classRef.lookupAttribute("geo_location");

  for (unsigned i = 0; i < _classRef.numberOfAttributes(); i++) {
    objy::data::Attribute attr = _classRef.attribute(i);
    _attributeMap[attr.name()] = attr;
  }
}

const objy::data::Attribute& csv::ClassAccessor::getAttribute(const string& attrName) const {
  AttributeMap::const_iterator itr = _attributeMap.find(attrName);
  if (itr == _attributeMap.end()) {
    cerr << "Failed to get attribute: " << attrName
            << " from ClassAccessor for class: " << _className << endl;
    throw std::invalid_argument("Failed to get attribute: " + attrName);
  }

  return itr->second;
}

objy::data::Object csv::ClassAccessor::createObject(const CSVRecord& record) const {

  objy::data::Object instance;

  // check if we already have the instance
  if (_mapper->hasClassKey()) {
    instance = _mapper->getClassTargetList()->getTargetObject(record, _mapper->getClassKey());
  }

  if (instance.isNull()) {
    instance = createInstance();
  }

  // iterate all relationships and resolve references
  for (auto& rel : _mapper->getRelationshipList()) {
    TargetList* relTarget = rel->getTargetList();
    auto refList = rel->getRelationshipRefList();
    for (auto itr = refList.begin(); itr != refList.end(); itr++) {
      objy::data::Object refInstance = relTarget->getTargetObject(record, itr->getKey());
      if (!refInstance.isNull()) {
        setReference(instance, itr->getRefAttrName(), refInstance);
      }
      if (!itr->getRevRefAttrName().empty()) // we have a reverse attribute to set
      {
        itr->getRevRefClassProxy()->addReference(refInstance,
                itr->getRevRefAttrName(), instance);
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
objy::data::Object csv::ClassAccessor::createObject(const vector<Property>& properties) const {
  objy::data::Object&& instance = createInstance();
  for (Property property : properties) {
    objy::data::Variable var;
    const objy::data::Attribute& attr = this->getAttribute(property.getName());
    if (this->getValue(attr, var, property.getValue()))
      setAttributeValue(instance, attr, var);
  }
  return instance;
}

void csv::ClassAccessor::setAttributes(
        objy::data::Object& instance, const csv::CSVRecord& record) const {

  string attrValueStr;
  AttributeMapperMap::const_iterator itr;

  // iterate and create any Integer attribute
  const AttributeMapperMap& intMap = _mapper->getIntegersMap();
  itr = intMap.begin();

  while (itr != intMap.end()) {
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
  const AttributeMapperMap& floatMap = _mapper->getFloatMap();
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
  const AttributeMapperMap& strMap = _mapper->getStringsMap();
  itr = strMap.begin();

  while (itr != strMap.end()) {
    setAttributeValue(instance, itr->first, record.get(itr->second).c_str());
    itr++;
  }

}

void csv::ClassAccessor::setAttributeValue(objy::data::Object& instance,
        const objy::data::Attribute& attribute, const objy::data::Variable& value) const {
  objy::data::Variable varValue;
  instance.attributeValue(attribute, varValue);
  //varValue.set(value);
  varValue = value;
}

void csv::ClassAccessor::setAttributeValue(objy::data::Object& instance,
        const std::string& attributeName, const objy::data::Variable& value) const {
  AttributeMap::const_iterator itr = _attributeMap.find(attributeName);
  if (itr != _attributeMap.end()) {
    setAttributeValue(instance, itr->second, value);
  } else {
    std::cerr << "Error for : " << instance.getClass(true).name()
            << " -attr: " << attributeName << " is not in the attribute map"
            << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance,
        const objy::data::Attribute& attribute, const objy::data::Object& value) const {
  objy::data::Variable varValue;
  instance.attributeValue(attribute, varValue);

  objy::data::LogicalType::type attrLogicalType =
          attribute.attributeValueSpecification()->logicalType();

  if (attrLogicalType == objy::data::LogicalType::List) {
    objy::data::List list = varValue.get<objy::data::List>();
    if (!doesListContainReference(list, value))
      list.append(objy::data::createReference(value));
  } else if (attrLogicalType == objy::data::LogicalType::Map) {
    objy::data::Map map = varValue.get<objy::data::Map>();
    addReferenceIfDoesnotExist(map, objy::data::createReference(value));
  } else if (attrLogicalType == objy::data::LogicalType::Reference) {
    varValue.set(objy::data::createReference(value));
  } else {
    std::cerr << "Illegal attribute type " << objy::data::LogicalType::toString(attrLogicalType)
            << " for Instance value." << std::endl;
  }
}

void csv::ClassAccessor::setReference(objy::data::Object& instance,
        const string& attributeName, const objy::data::Object& value) const {
  const objy::data::Attribute& attribute = _attributeMap.find(attributeName)->second;
  if (instance.isNull() || value.isNull() || attribute.isNull()) {
    std::cerr << "For attr: " << attributeName
            << " - instance/attribute/value: " << objy::data::oidFor(instance).sprint()
            << " / " << attribute.name() << " / " << objy::data::oidFor(value).sprint()
            << std::endl;
  }
  setReference(instance, attribute, value);
}

void csv::ClassAccessor::addReference(objy::data::Object& instance,
        const string& attributeName, const objy::data::Object& value) const {
  const objy::data::Attribute& attribute = _attributeMap.find(attributeName)->second;
  setReference(instance, attribute, value);
}

bool csv::ClassAccessor::doesListContainReference(const objy::data::List& list,
        const objy::data::Object& value) const {
  objy::data::Variable var;
  objy::uint_64 valueOid = value.identifier().get<objy::uint_64>();
  for (unsigned i = 0; i < list.size(); i++) {
    list.elementAt(i, var);
    objy::uint_64 refOid = var.get<objy::data::Reference>().identifier().get<objy::uint_64>();
    if (refOid == valueOid) {
      return true;
    }
  }

  return false;
}

void csv::ClassAccessor::addReferenceIfDoesnotExist(objy::data::Map& map,
        const objy::data::Reference& objRef) const {
  // set the key and value in the map from the call object.... 
  ooId oid = objy::data::oidFor(objRef);
  char* key = oid.sprint();
  //System.out.println("call.getObjectId(): " + call.getObjectId().toString());
  if (!map.containsKey(key)) {
    //objy::data::Variable value(objRef);
    map[key] = objRef;
  }
}

/**
 * Construct the correct Variable from the value (passed as string) based on the 
 * attribute type.
 * 
 * This is mainly used to convert data from CSV files to attribute values.
 * We assume that the types will be primitive and data/time 
 * (Date time is not yet implemented)
 * @param attr
 * @param value
 * @return 
 */
bool csv::ClassAccessor::getValue(const objy::data::Attribute& attr,
        objy::data::Variable& var, std::string value) const {

  bool retValue = true;

  objy::data::LogicalType::type logicalType = attr.attributeValueSpecification()->logicalType();
//	cout << "logicalType: " << objy::data::LogicalType::toString(logicalType) << endl;
  
  switch (logicalType) {
    case objy::data::LogicalType::Integer:
    {
//      cout << "integer tyoe for value: " << value << endl;
      objy::uint_16 encoding = attr.specification()->encoding();
      objy::uint_16 storage = attr.specification()->storage();
      try {
        if (encoding == objy::data::IntegerEncoding::Signed) {
          objy::int_64 intNumber = std::stol(value);
          var.set<objy::int_64>(intNumber);
        } else {
          objy::uint_64 intNumber = std::stoul(value);
          var.set<objy::uint_64>(intNumber);
        }
      } catch (std::invalid_argument& ex) {
        cerr << "invalid conversion to Integer: " << ex.what() << endl;
        retValue = false;
      }
     }
      break;
   case objy::data::LogicalType::Real:
    {
      try {
        objy::float_64 realNumber = std::stod(value);
        var.set<objy::float_64>(realNumber);
      } catch (std::invalid_argument& ex) {
        cerr << "invalid conversion to Real: " << ex.what() << endl;
        retValue = false;
      }
    }
      break;
    case objy::data::LogicalType::Boolean:
    {
      string bValue;
      for (auto& c : value)
        bValue.push_back(std::toupper(c));

      if ((bValue.compare("TRUE") == 0) || (bValue.compare("1") == 0))
        var.set<bool>(true);
      else if ((bValue.compare("FALSE") == 0) || (bValue.compare("0") == 0))
        var.set<bool>(false);
      else {
        cerr << "invalid conversion to bool for value: " << value << endl;
        retValue = false;
      }
      break;
    }
    case objy::data::LogicalType::Character:
    {
      char charVal = value.front();
      var.set<char>(charVal);
      break;
    }
    case objy::data::LogicalType::String:
    {
      objy::uint_16 encoding = attr.attributeValueSpecification()->encoding();

      objy::data::Utf8String utf8 = objy::data::createUtf8String();
      utf8.set(value.c_str());

      switch (encoding) {
        case objy::data::StringEncoding::Byte:
        {
          var.set<objy::data::ByteString>(objy::data::convertToByteString(utf8));
          break;
        }
        case objy::data::StringEncoding::Utf8:
        {
          var.set<objy::data::Utf8String>(utf8);
          break;
        }
        case objy::data::StringEncoding::Utf16:
        {
          var.set<objy::data::Utf16String>(objy::data::convertToUtf16String(utf8));
          break;
        }
        case objy::data::StringEncoding::Utf32:
        {
          var.set<objy::data::Utf32String>(objy::data::convertToUtf32String(utf8));
          break;
        }
        case objy::data::StringEncoding::Unspecified:
        {
          string errString("Unspecified encoding for attribute: ");
          errString.append(attr.name()).append(" in class: ").append(this->_className);
          throw std::domain_error(errString);
        }
      }
      break;
    }
    default:
    {
      cerr << "For attr: " << attr.name()
              << " invalid logical type conversion for record with value: " << value
              << endl;
      retValue = false;
      break;
    }
  }
  return retValue;
}
