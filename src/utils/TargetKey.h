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

namespace csv {

  class TargetKey {
  };

  class SingleKey : public TargetKey {
  public:
    SingleKey();
    SingleKey(const SingleKey& orig);
    virtual ~SingleKey();

    SingleKey(string attrName, string rawFileAttrName,
            objy::data::LogicalType::type logicalType) {
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

  class CompositeKey : public TargetKey {
  public:
    CompositeKey();
    CompositeKey(const CompositeKey& orig);
    virtual ~CompositeKey();

    CompositeKey(std::vector<SingleKey> singleKeys) {
      keys = singleKeys;
    }

    std::string toString() {
      std::string strBuffer;
      for (SingleKey key : keys) {
        strBuffer += key.toString() + " \n ";
      }
      return strBuffer;
    }

  private:
    std::vector<SingleKey> keys;

  };

}

#endif /* TARGETKEY_H */

