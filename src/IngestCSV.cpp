/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IngestCSV.cpp
 * Author: ibrahim
 * 
 * Created on July 26, 2017, 5:34 PM
 */

#include <iostream>
#include <cassert>

#include <ooObjy.h>
#include <objy/Exception.h>

#include "IngestCSV.h"

#include "utils/rapidjson/document.h"
#include "utils/rapidjson/filereadstream.h"
#include "utils/rapidjson/stringbuffer.h"
#include "utils/csv/CSVParser.h"
#include "utils/csv/CSVFormat.h"
#include "utils/csv/CSVRecord.h"
#include "utils/Relationship.h"
#include "utils/ClassAccessor.h"

using namespace std;

csv::IngestCSV::IngestCSV() {
}

csv::IngestCSV::IngestCSV(const IngestCSV& orig) {
}

csv::IngestCSV::~IngestCSV() {
}

/**
 * 
 * @param csvFile
 * @param mapperFile
 * @param commitEvery
 */
void csv::IngestCSV::ingest(std::string csvFile, std::string mapperFile, int commitEvery) {
  rapidjson::Document jsonMapper;
  // read the mapper file as JSON and convert it to a Mapper object
  FILE* fp = fopen(mapperFile.c_str(), "rb");
  if (fp != NULL) {
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof (readBuffer));
    jsonMapper.ParseStream(is);
    fclose(fp);
  } else {
    std::cerr << "Error opening mapper file: " << mapperFile << std::endl;
		return;
  }

  try {
    objy::db::Transaction* tx = new objy::db::Transaction(objy::db::OpenMode::Update, "write");
    //      long timeStart = System.currentTimeMillis();
    // schemaManager need to be initialized within a transaction to cahce 
    // meta data information.
    std::string fileName = csvFile;

    bool val = fileName.empty();
    if (!fileName.empty()) {
      if (jsonMapper.IsArray()) {
        for (auto& element : jsonMapper.GetArray())
        {
          csv::IngestMapper mapper;
          
          mapper.initialize(element.GetObject());
          
          int objCount = processRawData(fileName, mapper);
          // for now we read and process the whole file before commiting, but we
          // might change that to iterate and commit as needed.
          checkpoint(tx);
          std::cout << "Processed '" << fileName
                  << "' Ingest... Total Objects: " << objCount << std::endl;
        }
      } else {
        assert(jsonMapper.IsObject());
        IngestMapper mapper;
        
        mapper.initialize(jsonMapper.GetObject());
 
        int objCount = processRawData(fileName, mapper);
        // for now we read and process the whole file before commiting, but we
        // might change that to iterate and commit as needed.
        checkpoint(tx);
        std::cout << "Processed '" << fileName
                << "' Ingest... Total Objects: " << objCount << std::endl;

      }
    }

    tx->commit();
    tx->release();
    //long timeDiff = System.currentTimeMillis() - timeStart;
    //LOG.info("Time: {} sec", (timeDiff/1000.0));
  } catch (ooKernelException& ex) {
    std::cout << ex.what() << std::endl;
  }
}

void csv::IngestCSV::checkpoint(objy::db::Transaction* const tx) {
  tx->commit();
  tx->start(objy::db::OpenMode::Update);
}

int csv::IngestCSV::processRawData(std::string fileName, IngestMapper& mapper) {
  //LOG.info("Starting Ingest for: {}: ", fileName);

  vector<csv::CSVRecord> records;

  int objCount = 0;
  try {
    // pass 1: process the keys and related types from the file and cache oids.
    // parse the file and pick the keys needed.
    csv::CSVFormat* csvFormat = csv::CSVFormat::create(csv::FormatType::RFC4180);
    csvFormat->withFirstRecordAsHeader();
    csv::CSVParser csvParser(fileName, csvFormat);
    cout << "getting records... " ;
    records = csvParser.getRecords();
    cout << " found: " << records.size() << endl;
    bool doProcessForRelationships = mapper.hasRelationships();
    bool doProcessClassKeys = mapper.hasClassKey();
    if (doProcessClassKeys || doProcessForRelationships) {
      std::cout << "Phase 1: process records and collect information for existing objects and relationships." << std::endl;
      for (CSVRecord& record : records) {
        if (doProcessForRelationships) {
          for (Relationship* rel : mapper.getRelationshipList()) {
            rel->getTargetList()->collectTargetInfo(record);
          }
        }
        if (doProcessClassKeys) {
          mapper.getClassTargetList()->collectTargetInfo(record);
        }
      }
      // fetch the ids that exist in the FD.
      if (doProcessForRelationships) {
        std::cout << "\t fetch and/or create related objects..." << std::endl;
        for (Relationship* rel : mapper.getRelationshipList()) {
          rel->getTargetList()->fetchTargets();
          int count = rel->getTargetList()->createMissingTargets();
          std::cout << "created " << count << " of "
                  << rel->toClassName() << " objects" << std::endl;
        }
      }
      // fetch existing objects if available.
      if (doProcessClassKeys) {
        std::cout << "\t fetch and/or create objects..." << std::endl;
        mapper.getClassTargetList()->fetchTargets();
        int count = mapper.getClassTargetList()->createMissingTargets();
        std::cout << "created " << count << " of "
                << mapper.getClassName() << " objects" << std::endl;
      }
    }
    //csvParser.rewind();

    // pass 2:
    std::cout << "Phase 2: update newly created objects and connect related objects." << std::endl;
    //records = csvParser.getRecords();
    for (csv::CSVRecord record : records) {
      mapper.getClassProxy()->createObject(record);
      objCount++;
    }
  } catch (csv::error::base& error) {
    cerr << "CSV::Error: " << error.what() << endl;	
  } catch (ooException& ex) {
    cerr << "ooException: " << ex.getStackTrace() << endl;
  } catch (objy::UserException& ex) {
    cerr << "objy::UserEception: " << ex.what() << endl;
  } catch (std::exception& stdex) {
    cerr << "csv::IngestCSV::processFile() - Exception: " << stdex.what() << endl;
  } catch (...) {
    cerr << "Error processing CSVFile (1)." << endl;
  }
  return objCount;
}
