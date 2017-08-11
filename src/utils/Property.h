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
    std::string _attrName;
    std::string _attrValue;

  public:

    Property(const std::string& name, const std::string& value) :
    _attrName(name), _attrValue(value) {
    }

    Property(const Property& other) :
    _attrName(other._attrName), _attrValue(other._attrValue) {
    }

    Property(Property&& other) :
    _attrName(std::move(other._attrName)), _attrValue(std::move(other._attrValue)) {
    }

    std::string getName() const {
      return _attrName;
    }

    std::string getValue() const {
      return _attrValue;
    }

  };
}
#endif /* PROPERTY_H */

