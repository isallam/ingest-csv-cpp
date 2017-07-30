/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Relationship.cpp
 * Author: ibrahim
 * 
 * Created on July 20, 2017, 3:37 PM
 */

#include <vector>

#include "Relationship.h"

Relationship::Relationship() {
}

Relationship::Relationship(const Relationship& orig) {
}

Relationship::~Relationship() {
}

void Relationship::add(TargetKey& key, std::string& refAttrName, std::string& revRefAttrName) {
  RelationshipRef relationshipRef = new RelationshipRef(key, refAttrName, revRefAttrName);
  relationshipRefList.push_back(relationshipRef);
}

TargetList* Relationship::getTargetList() {
  if (targetList == NULL) {
    // initialize TargetList
    targetList = new TargetList(
            SchemaManager.getInstance().getClassProxy(toClassName),
            getKeys());
  }
  return targetList;
}

std::vector<TargetKey>& Relationship::getKeys(){
  std::vector<TargetKey> targetKeys;
  for (auto& relRef : relationshipRefList)
  {
    targetKeys.push_back(relRef.getKey());
  }
  return targetKeys;
}


