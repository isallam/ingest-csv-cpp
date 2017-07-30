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

#include "IngestCSV.h"
#include "src/utils/rapidjson/document.h"
#include "src/utils/rapidjson/filereadstream.h"
#include "src/utils/rapidjson/stringbuffer.h"
#include "src/utils/fast-cpp-csv-parser/csv.h"
#include "utils/Relationship.h"

IngestCSV::IngestCSV() {
}

IngestCSV::IngestCSV(const IngestCSV& orig) {
}

IngestCSV::~IngestCSV() {
}

 void IngestCSV::ingest(std::string& csvFile, std::string& mapperFile, int commitEvery) {
    rapidjson::Document jsonMapper;
    // read the mapper file as JSON and convert it to a Mapper object
    FILE* fp = fopen(mapperFile, "rb");
    if (fp != NULL)
    {
      char readBuffer[65536];
      rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
      jsonMapper.ParseStream(is);
      fclose(fp);
    }
    else {
      std::cerr << "Error opening file: " << csvFile << std::endl;
    }
    
    try {
      objy::db::Transaction tx = new objy::db::Transaction(objy::db::OpenMode::Update, "spark_write");
//      long timeStart = System.currentTimeMillis();
      // schemaManager need to be initialized within a transaction to cahce 
      // meta data information.
      std::string& fileName = csvFile;

      if (fileName != NULL && !jsonMapper.Null()) {
        if (jsonMapper.IsArray()) {
          for (int i = 0; i < jsonMapper.Size(); i++)
          {
            IngestMapper mapper = new IngestMapper(jsonMapper[i]);
            int objCount = processFile(fileName, mapper);
            // for now we read and process the whole file before commiting, but we
            // might change that to iterate and commit as needed.
            checkpoint(tx);
            std::cout << "Done '" << fileName 
                    << "' Ingest... Total Objects: " <<  objCount << std::endl;
          }   
        }
        else 
        {
            IngestMapper mapper = new IngestMapper(jsonMapper);
            int objCount = processFile(fileName, mapper);
            // for now we read and process the whole file before commiting, but we
            // might change that to iterate and commit as needed.
            checkpoint(tx);
            std::cout << "Done '" << fileName 
                    << "' Ingest... Total Objects: " <<  objCount << std::endl;
          
        }
      }

      objy::db::Transaction.getCurrent().commit();
      //long timeDiff = System.currentTimeMillis() - timeStart;
      //LOG.info("Time: {} sec", (timeDiff/1000.0));
      
    } catch (ooKernelException& ex) {
      std::cout << ex.what() << std::endl;
    }
  }

  void IngestCSV::checkpoint(objy::db::Transaction& tx) {
    tx.commit();
    tx.start(objy::db::OpenMode::Update);
  }

int IngestCSV::processFile(std::string& fileName, IngestMapper& mapper) {
    //LOG.info("Starting Ingest for: {}: ", fileName);

    ClassAccessor classProxy = SchemaManager.getInstance().getClassProxy(mapper.getClassName());
    classProxy.setMapper(mapper);
    Iterable<CSVRecord> records = null;
    int objCount = 0;
    try {
      Reader in;
      // pass 1: process the keys and related types from the file and cache oids.
      in = new FileReader(fileName);
      // parse the file and pick the keys needed.
      records = CSVFormat.RFC4180.withFirstRecordAsHeader().parse(in);
      bool doProcessForRelationships = mapper.hasRelationships();
      bool doProcessClassKeys = mapper.hasClassKey();
      if (doProcessClassKeys || doProcessForRelationships) {
        for (CSVRecord record : records) {
          if (doProcessForRelationships) {
            for (Relationship rel : mapper.getRelationshipList()) {
              rel.getTargetList().collectTargetInfo(record);
            }
          }
          if (doProcessClassKeys) {
            mapper.getClassTargetList().collectTargetInfo(record);
          }
        }
        // fetch the ids that exist in the FD.
        if (doProcessForRelationships)
        {
          for (Relationship rel : mapper.getRelationshipList()) {
            rel.getTargetList().fetchTargets();
            int count = rel.getTargetList().createMissingTargets();
            std::cout << "created " << count << " of " 
                    << rel.toClassName() << " objects" << std::endl;
          }
        }
        // fetch existing objects if available.
        if (doProcessClassKeys) {
          mapper.getClassTargetList().fetchTargets();
          int count = mapper.getClassTargetList().createMissingTargets();
          std::cout << "created " << count << " of " 
                  << mapper.getClassName() << " objects" << std::endl;
        }
      }
      in.close();

      // pass 2:
      std::cout << "Phase 2: update newly created objects and connect related objects." << std::endl;
      in = new FileReader(fileName);
      records = CSVFormat.RFC4180.withFirstRecordAsHeader().parse(in);
      for (CSVRecord record : records) {
        classProxy.createObject(record);
        objCount++;
      }
    } catch (FileNotFoundException ex) {
      LOG.error(ex.getMessage());
    } catch (IOException ex) {
      LOG.error(ex.getMessage());
    }
    return objCount;
  }
