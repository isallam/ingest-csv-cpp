/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CompositeKey.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 12:46 PM
 */
//-------------------------------------------------------------------------
// the CompositeKey will be handled as a one unit to represnt the object
// the combination of the keys will uniquely identify the object.
//-------------------------------------------------------------------------

#ifndef COMPOSITEKEY_H
#define COMPOSITEKEY_H

#include <string>
#include <vector>

#include "TargetKey.h"

class CompositeKey : public TargetKey {
public:
  CompositeKey();
  CompositeKey(const CompositeKey& orig);
  virtual ~CompositeKey();

  CompositeKey(std::vector<SingleKey>& singleKeys) {
    keys = singleKeys;
  }

  std::string toString() {
    std::string strBuffer;
    for (SingleKey key : keys)
    {
      strBuffer += key.toString() + std::endl;
    }
    return strBuffer;
  }
  
private:
  std::vector<SingleKey> keys;

};

#endif /* COMPOSITEKEY_H */

