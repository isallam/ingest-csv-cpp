/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TargetKey.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 3:46 PM
 */

#ifndef TARGETKEY_H
#define TARGETKEY_H

#include <string>
#include <vector>
#include <objy/data/Data.h>
#include "csv/CSVRecord.h"
#include "Property.h"

using namespace std;

namespace csv {

  class TargetKey {
  public:
    TargetKey() = default;
    virtual ~TargetKey() = default;
    virtual string toString() = 0;
    virtual string getCorrectValue(CSVRecord& record) = 0;
    virtual vector<Property> getProperties(CSVRecord& record) = 0;
    virtual string getAttrName() = 0;
    virtual string getRawFileAttrName() = 0;
  };

  /***********************************/
  /*      SingleKey                  */

  /***********************************/
  class SingleKey : public TargetKey {
  public:
    SingleKey() = delete;

    virtual ~SingleKey() {
    }

    SingleKey(const string& attrName, const string& rawFileAttrName,
            objy::data::LogicalType::type logicalType) :
    _attrName(attrName), _rawFileAttrName(rawFileAttrName), _logicalType(logicalType) {
    }

    SingleKey(const SingleKey& orig) :
    _attrName(orig._attrName), _rawFileAttrName(orig._rawFileAttrName), _logicalType(orig._logicalType) {
    }

    SingleKey(SingleKey&& orig) :
    _attrName(std::move(orig._attrName)),
    _rawFileAttrName(std::move(orig._rawFileAttrName)), _logicalType(std::move(orig._logicalType)) {
    }

    string toString() {
      string retString;

      retString = "attrName: " + _attrName +
              ", rawFileName: " + _rawFileAttrName +
              ", logicalType: " + objy::data::LogicalType::toString(_logicalType);

      return retString;
    }

    string getCorrectValue(CSVRecord& record) {
      //    if (logicalType == LogicalType.INTEGER) {
      //      long attrValue = 0;
      //      string attrValueStr = record.get(rawFileAttrName);
      //      if (!attrValueStr.equals(""))
      //        attrValue = Long.parseLong(attrValueStr);
      //      return attrValueStr;
      //    }
      //    if (logicalType == LogicalType.REAL) {
      //      double attrValue = 0;
      //      String attrValueStr = record.get(rawFileAttrName);
      //      if (!attrValueStr.equals(""))
      //        attrValue = Double.parseDouble(attrValueStr);
      //      return attrValue;
      //    }
      //    else 
      return record.get(_rawFileAttrName);
    }
    
    vector<Property> getProperties(CSVRecord& record) {
        vector<Property> nameValues;
          nameValues.push_back(Property(
                  getAttrName(),
                  getCorrectValue(record)));
          return nameValues;
    }

    string getAttrName() {
      return _attrName;
    }

    string getRawFileAttrName() {
      return _rawFileAttrName;
    }

    std::vector<SingleKey> keys() const {
      throw std::invalid_argument("SingleKey does implement keys()");
    }


  private:
    string _attrName;
    string _rawFileAttrName;
    objy::data::LogicalType::type _logicalType;

  };

  /***********************************/
  /*      CompositeKey               */

  /***********************************/
  class CompositeKey : public TargetKey {
  public:
    CompositeKey() = delete;
    CompositeKey(const CompositeKey& orig) = delete;

    virtual ~CompositeKey() {
    }

    CompositeKey(std::vector<SingleKey*> singleKeys) : _keys(singleKeys) {
    }

    CompositeKey(CompositeKey&& other) : _keys(std::move(other._keys)) {
    }

    std::string toString() {
      std::string strBuffer;
      for (SingleKey* key : _keys) {
        strBuffer += key->toString() + " \n ";
      }
      return strBuffer;
    }

    virtual string getCorrectValue(CSVRecord& record) {
      string value = "";
      for (SingleKey* aKey : _keys) {
        value += aKey->getCorrectValue(record);
      }
      return value;
    }
    
    vector<Property> getProperties(CSVRecord& record) {
        vector<Property> nameValues;
        for (unsigned int i = 0; i < _keys.size(); i++) {
          nameValues.push_back(Property(
                  _keys[i]->getAttrName(),
                  _keys[i]->getCorrectValue(record)));
        }
        return nameValues;
    }

    string getAttrName() {
      throw std::invalid_argument("CompositeKey does implement getAttrName()");
    }

    string getRawFileAttrName() {
      throw std::invalid_argument("CompositeKey does implement getRarFileAttrName()");
    }

    std::vector<SingleKey*> keys() const {
      return _keys;
    }


  private:
    std::vector<SingleKey*> _keys;

  };

}

#endif /* TARGETKEY_H */

