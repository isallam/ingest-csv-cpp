/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   SingleKey.cpp
 * Author: ibrahim
 * 
 * Created on July 20, 2017, 3:28 PM
 */

#include "SingleKey.h"

SingleKey::SingleKey() {
}

SingleKey::SingleKey(const SingleKey& orig) {
}

SingleKey::~SingleKey() {
}

  Object SingleKey::getCorrectValue(CSVRecord record) {
    if (logicalType == LogicalType.INTEGER) {
      long attrValue = 0;
      String attrValueStr = record.get(rawFileAttrName);
      if (!attrValueStr.equals(""))
        attrValue = Long.parseLong(attrValueStr);
      return attrValue;
    }
    if (logicalType == LogicalType.REAL) {
      double attrValue = 0;
      String attrValueStr = record.get(rawFileAttrName);
      if (!attrValueStr.equals(""))
        attrValue = Double.parseDouble(attrValueStr);
      return attrValue;
    }
    else 
      return record.get(rawFileAttrName);
  }
