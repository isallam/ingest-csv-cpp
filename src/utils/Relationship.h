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
#include "SchemaManager.h"
#include "TargetKey.h"
#include "TargetList.h"

using namespace std;


namespace csv {

  class ClassAccessor;

  class RelationshipRef {
  private:
    csv::TargetKey _key;
    string _refAttrName;
    string _revRefAttrName;
    string _revRefClassName;
    ClassAccessor* _revRefClassProxy;

  public:

    RelationshipRef(TargetKey& key, std::string& refAttrName,
            std::string& revRefAttrName, string& revRefClassName) {
      _key = key;
      _refAttrName = refAttrName;
      _revRefAttrName = revRefAttrName;
      _revRefClassName = revRefClassName;
      _revRefClassProxy = nullptr;
    }

    csv::TargetKey& getKey() {
      return _key;
    }

    std::string& getRefAttrName() {
      return _refAttrName;
    }

    std::string getRevRefAttrName() {
      return _revRefAttrName;
    }

    csv::ClassAccessor* getRevRefClassProxy() {
      if (_revRefClassProxy == nullptr)
        _revRefClassProxy = csv::SchemaManager::getInstance()->getClassProxy(_revRefClassName);
      return _revRefClassProxy;
    }
  };

  class Relationship {
  public:

    Relationship() : _targetList(nullptr), _isToOne(true) {
    }
    Relationship(const Relationship& orig) = delete;
    virtual ~Relationship();

    Relationship(std::string toClassName, bool isToMany = true) : Relationship() {
      _toClassName = toClassName;
      _isToOne = !isToMany;
    }

    //    Relationship(std::string toClassName, bool isToMany) {
    //      _toClassName = toClassName;
    //      _isToOne = !isToMany;
    //    }

    std::string toClassName() {
      return _toClassName;
    }

    vector<RelationshipRef> getRelationshipRefList() {
      return _relationshipRefList;
    }

    void add(TargetKey key, string refAttrName, string revRefAttrName, string revRefClassName) {
      RelationshipRef relationshipRef(key, refAttrName, revRefAttrName, revRefClassName);
      _relationshipRefList.push_back(relationshipRef);
    }

    TargetList* getTargetList() {
      if (_targetList == NULL) {
        // initialize TargetList
        _targetList = new TargetList(
                csv::SchemaManager::getInstance()->getClassProxy(_toClassName),
                getKeys());
      }
      return _targetList;
    }

  private:
    bool _isToOne;
    TargetList* _targetList;
    std::string _toClassName;
    vector<RelationshipRef> _relationshipRefList;

    vector<TargetKey> getKeys() {
      std::vector<TargetKey> targetKeys;
      for (auto& relRef : _relationshipRefList) {
        targetKeys.push_back(relRef.getKey());
      }
      return targetKeys;
    }

  };
}
#endif /* RELATIONSHIP_H */

