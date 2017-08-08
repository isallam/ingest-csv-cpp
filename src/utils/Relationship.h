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

#include <string>
#include <vector>
#include "TargetKey.h"
#include "ClassAccessor.h"

using namespace std;

class Relationship {

  class RelationshipRef {
  private:
    TargetKey _key;
    std::string _refAttrName;
    std::string _revRefAttrName;
    ClassAccessor* _revRefClassProxy;

  public:

    RelationshipRef(TargetKey& key, std::string& refAttrName, std::string& revRefAttrName) {
      _key = key;
      _refAttrName = refAttrName;
      _revRefAttrName = revRefAttrName;
      _revRefClassProxy = NULL;
    }

    TargetKey getKey() {
      return _key;
    }

    std::string getRefAttrName() {
      return _refAttrName;
    }

    std::string getRevRefAttrName() {
      return _revRefAttrName;
    }

    ClassAccessor& getRevRefClassProxy() {
      if (_revRefClassProxy == NULL)
        _revRefClassProxy = SchemaManager.getInstance().getClassProxy(toClassName);
      return _revRefClassProxy;
    }
  };

public:
  Relationship();
  Relationship(const Relationship& orig);
  virtual ~Relationship();

  Relationship(std::string toClassName) {
    _toClassName = toClassName;
  }

  Relationship(std::string toClassName, bool isToMany) {
    _toClassName = toClassName;
    _isToOne = !isToMany;
  }

  std::string toClassName() {
    return _toClassName;
  }

  vector<RelationshipRef> getRelationshipRefList() {
    return _relationshipRefList;
  }

  void add(TargetKey key, string refAttrName, string revRefAttrName) {
    RelationshipRef relationshipRef = new RelationshipRef(key, refAttrName, revRefAttrName);
    _relationshipRefList.push_back(relationshipRef);
  }

  TargetList getTargetList() {
    if (_targetList == NULL) {
      // initialize TargetList
      _targetList = new TargetList(
              SchemaManager.getInstance().getClassProxy(toClassName),
              getKeys());
    }
    return _targetList;
  }

private:
  std::string& _toClassName;
  bool _isToOne = true;
  TargetList* _targetList;
  vector<RelationshipRef> _relationshipRefList;

  vector<TargetKey>& getKeys() {
    std::vector<TargetKey> targetKeys;
    for (auto& relRef : _relationshipRefList) {
      targetKeys.push_back(relRef.getKey());
    }
    return targetKeys;
  }

};

#endif /* RELATIONSHIP_H */

