/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SingleKey.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 3:28 PM
 */
//-------------------------------------------------------------------------
// most keys are single, but the ID object use a composite key that is
// the hash of both IDReference + IDType
//----------------------------------------------------------------------s---

#ifndef SINGLEKEY_H
#define SINGLEKEY_H

#include <string>
#include <objy/data/Data.h>
#include "TargetKey.h"

using namespace std;

class SingleKey : public TargetKey {
public:
  SingleKey();
  SingleKey(const SingleKey& orig);
  virtual ~SingleKey();

  SingleKey(string attrName, string rawFileAttrName,
          objy::data::LogicalType logicalType) {
    attrName = attrName;
    rawFileAttrName = rawFileAttrName;
    logicalType = logicalType;
  }

  string toString() {
    string retString;

    retString = "attrName: " + attrName + ", rawFileName: " + rawFileAttrName +
            ", logicalType: " + objy::data::LogicalType::toString(logicalType);

    return retString;
  }

  string getCorrectValue(CSVRecord record) {
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
    return record.get(rawFileAttrName);
  }


private:
  string attrName;
  string rawFileAttrName;
  objy::data::LogicalType::type logicalType;

};

#endif /* SINGLEKEY_H */

