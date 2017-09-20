/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Relationship.h
 * Author: ibrahim
 *
 * Created on July 20, 2017, 3:37 PM
 */

#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <memory>
#include <string>
#include <vector>
#include "SchemaManager.h"
#include "TargetKey.h"
#include "TargetList.h"

using namespace std;


namespace csv {

  class ClassAccessor;

  /***********************************/
  /*      RelationshipRef            */

  /***********************************/
  class RelationshipRef {
  private:
    csv::TargetKey*      _key;
    string               _refAttrName;
    string               _revRefAttrName;
    string               _revRefClassName;
    const ClassAccessor* _revRefClassProxy;

  public:

    RelationshipRef(TargetKey* key, const string& refAttrName,
            const string& revRefAttrName, const string& revRefClassName) :
    _refAttrName(refAttrName),
    _revRefAttrName(revRefAttrName),
    _revRefClassName(revRefClassName) {
      _key = key;
      _revRefClassProxy = csv::SchemaManager::getInstance()->getClassProxy(revRefClassName);
    }

    RelationshipRef(const RelationshipRef& other) :
    _refAttrName(other._refAttrName),
    _revRefAttrName(other._revRefAttrName),
    _revRefClassName(other._revRefClassName),
    _revRefClassProxy(other._revRefClassProxy),
    _key(other._key) 
    {}

    RelationshipRef(RelationshipRef&& other) :
    _key(std::move(other._key)),
    _refAttrName(std::move(other._refAttrName)),
    _revRefAttrName(std::move(other._revRefAttrName)),
    _revRefClassName(std::move(other._revRefClassName)),
    _revRefClassProxy(other._revRefClassProxy) {
      other._key = nullptr;
    }
    virtual ~RelationshipRef() {
        delete _key;
    };

    csv::TargetKey* getKey() const {
      return _key;
    }

    std::string getRefAttrName() const {
      return _refAttrName;
    }

    std::string getRevRefAttrName() const {
      return _revRefAttrName;
    }

    const csv::ClassAccessor* getRevRefClassProxy() const {
      return _revRefClassProxy;
    }
  };

  /***********************************/
  /*      Relationship               */

  /***********************************/
  class Relationship {
  public:

    Relationship() = delete;
    Relationship(const Relationship& orig) = delete;
    virtual ~Relationship() = default; // TBD.

    Relationship(const string& toClassName, bool isToMany = true) :
    _toClassName(toClassName), _isToOne(!isToMany), _targetList(nullptr) {
    }

    Relationship(Relationship&& other) :
    _toClassName(std::move(other._toClassName)),
    _isToOne(other._isToOne), _targetList(other._targetList) {
      other._targetList = nullptr;
    }

    std::string toClassName() const {
      return _toClassName;
    }

    const vector<csv::RelationshipRef*> getRelationshipRefList() const {
      return _relationshipRefList;
    }

    void add(TargetKey* key, const string& refAttrName,
            const string& revRefAttrName, const string& revRefClassName) {
      _relationshipRefList.push_back(new csv::RelationshipRef(key, refAttrName, revRefAttrName, revRefClassName));
    }

    csv::TargetList* getTargetList() {
      if (_targetList == NULL) {
        // initialize TargetList
        _targetList = new TargetList(
                csv::SchemaManager::getInstance()->getClassProxy(_toClassName),
                getKeys());
      }
      return _targetList;
    }

  private:
    std::string                   _toClassName;
    bool                          _isToOne;
    csv::TargetList*              _targetList;
    vector<csv::RelationshipRef*> _relationshipRefList;

    vector<TargetKey*> getKeys() {
      std::vector<TargetKey*> targetKeys;
      for (auto relRef : _relationshipRefList) {
        targetKeys.push_back(relRef->getKey());
      }
      return targetKeys;
    }

  };
}
#endif /* RELATIONSHIP_H */

