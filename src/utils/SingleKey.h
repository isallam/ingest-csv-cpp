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

class SingleKey : public TargetKey {
public:
  SingleKey();
  SingleKey(const SingleKey& orig);
  virtual ~SingleKey();
  SingleKey(std::string attrName, std::string rawFileAttrName,
          objy::data::LogicalType logicalType) {
    attrName = attrName;
    rawFileAttrName = rawFileAttrName;
    logicalType = logicalType;
  }
  std::string toString() {
    std::string retString;
    
    retString = "attrName: " + attrName + ", rawFileName: " + rawFileAttrName +
            ", logicalType: " + logicalType.toString();
    return retString;
  }

  Object getCorrectValue(CSVRecord record);


private:
    std::string attrName;
    std::string rawFileAttrName;
    objy::data::LogicalType logicalType;
    
};

#endif /* SINGLEKEY_H */

