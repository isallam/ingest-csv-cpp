/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Property.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 3:34 PM
 */

#ifndef PROPERTY_H
#define PROPERTY_H
#include <string>

namespace csv {

  class Property {
  private:
    std::string attrName;
    std::string attrValue;

  public:

    Property(std::string name, std::string value) {
      attrName = name;
      attrValue = value;
    }

    std::string getName() {
      return attrName;
    }

    std::string getValue() {
      return attrValue;
    }

  };
}
#endif /* PROPERTY_H */

