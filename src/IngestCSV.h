/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IngestCSV.h
 * Author: ibrahim
 *
 * Created on July 26, 2017, 5:34 PM
 */

#ifndef INGESTCSV_H
#define INGESTCSV_H
#include <string>
#include "IngestMapper.h"
#include <objy/db/Transaction.h>

namespace csv {

  class IngestCSV {
  public:
    IngestCSV();
    IngestCSV(const IngestCSV& orig);
    virtual ~IngestCSV();
    void ingest(std::string csvFile, std::string mapperFile, int commitEvery);
  private:
    void checkpoint(objy::db::Transaction* const tx);
    int processFile(std::string fileName, csv::IngestMapper& mapper);
  };
}
#endif /* INGESTCSV_H */

