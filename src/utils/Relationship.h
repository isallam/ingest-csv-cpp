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
#incluce <vector>

class Relationship {
  class RelationshipRef {
  private:
      TargetKey key;
      std::string refAttrName;
      std::string revRefAttrName;
      ClassAccessor* revRefClassProxy;

  public:
      RelationshipRef(TargetKey& key, std::string& refAttrName, std::string& revRefAttrName) {
        this.key = key;
        this.refAttrName = refAttrName;
        this.revRefAttrName = revRefAttrName;
        revRefClassProxy = NULL;
      }

      TargetKey getKey() { return key; }
      std::string getRefAttrName() { return refAttrName; }
      std::string getRevRefAttrName() { return revRefAttrName; }

      ClassAccessor& getRevRefClassProxy() {
        if (revRefClassProxy == NULL)
          revRefClassProxy = SchemaManager.getInstance().getClassProxy(toClassName);
        return revRefClassProxy;
      }
  };

public:
  Relationship();
  Relationship(const Relationship& orig);
  virtual ~Relationship();

  Relationship(std::string toClassName) {
    this.toClassName = toClassName;
  }
  Relationship(std::string toClassName, bool isToMany) {
    this.toClassName = toClassName;
    this.isToOne = !isToMany;
  }
  std::string toClassName() { return this.toClassName; }
  List<RelationshipRef> getRelationshipRefList() {
    return relationshipRefList;
  }
  
  void add(TargetKey key, String refAttrName, String revRefAttrName);
  TargetList getTargetList();
  
private:
  std::string& toClassName;
  bool isToOne = true;
  TargetList* targetList;
  std::vector<RelationshipRef> relationshipRefList;
  
  std::vector<TargetKey>& getKeys();

};

#endif /* RELATIONSHIP_H */

