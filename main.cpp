/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   objysetup.cpp
 * Author: ibrahim
 *
 * The main reason of this file is to be able to create the schema and setup
 * the Placement before the ingest process runs. If you don't call this, you'll
 * have the correct schema but no placement
 * 
 * Created on January 11, 2017, 4:41 PM
 */

#include <cstdlib>
#include <string>
#include <iostream>

#include <ooObjy.h>
#include <objy/Configuration.h>
#include <objy/Tools.h>
#include <objy/db/Connection.h>
#include <objy/db/Transaction.h>
#include <objy/db/TransactionScope.h>
//#include <objy/data/Data.h>
#include <objy/data/List.h>
#include "IngestCSV.h"
#include "src/utils/option.h"

namespace objydb = objy::db;
namespace objydata = objy::data;
namespace objyconfig = objy::configuration;

using namespace std;

namespace config {
  static const char* CommitEvery    = "CommitEvery";
  static const char* BootFilePath   = "BootFilePath";
  static const char* CSVFilePath    = "CSVFilePath";
  static const char* CSVPathPattern = "CSVPathPattern";
  static const char* MapperFilePath = "MapperFilePath";


class _Params {
  optparse::OptionParser optionParser;
  
public:
  
  _Params(int argc, char* argv[]) {
    optionParser
        .usage("...")
        .version("")
        .description("Ingest CSV files into ThingSpan graph database.")
        .epilog("");
    optionParser
        .add_option("-c", "--commitEvery")
        .action("store")
        .type("int")
        .dest(CommitEvery)
        .set_default(20000)
        .help("commit frequency after # records (TBD)");
    optionParser
        .add_option("-b", "--bootFile")
        .action("store")
        .type("string")
        .dest(BootFilePath)
        .set_default("/home/projects/ingest-csv-cpp/data/testfd.boot")
        .help("bootfile file path (default ./data/fdtest.boot)");
    optionParser
        .add_option("-f", "--csvFile")
        .action("store")
        .type("string")
        .dest(CSVFilePath)
        .set_default("/home/projects/ingest-csv-cpp/source-data/addresses.csv")
        .help("full path to the csv file to ingest.");
    optionParser
        .add_option("-p", "--csvPathPattern")
        .action("store")
        .type("string")
        .dest(CSVPathPattern)
        .help("path pattern of csv files ");
    optionParser
        .add_option("-m", "--mapperFile")
        .action("store")
        .type("string")
        .dest(MapperFilePath)
        .set_default("/home/projects/ingest-csv-cpp/config/addressMapper.json")
        .help("full path to the JSON mapper file to aid the ingest process.");
    
    optparse::Values &values = optionParser.parse_args(argc, argv);
    
    bootFile = (const char*) values.get(BootFilePath);
    mapperFile = (const char*) values.get(MapperFilePath);
    commitEvery = values.get(CommitEvery).asInt32();
    
    csvFile = (const char*)values.get(CSVFilePath);
    csvPathPattern = (const char*) values.get(CSVPathPattern);
  }
  string bootFile;
  string csvFile;
  string csvPathPattern;
  string mapperFile;
  int commitEvery = 20000;
};
}

/*
 * 
 */
int main(int argc, char** argv) {

  string fdname;
  objydb::Connection* connection;
  objydb::Transaction* trx;
  config::_Params _params(argc, argv);

  // TBD... boot file is hard coded for now, will be params later
  //processParams(args);
  fdname =  _params.bootFile;
  ooObjy::setLoggingOptions(oocLogAll, true, false, "/home/projects/ingest-csv-cpp/logs");
  ooObjy::startup(24);

  objyconfig::ConfigurationManager* cfgMgr = objyconfig::ConfigurationManager::getInstance();
  cfgMgr->enableConfiguration(true, 0, 0, 0, 0);

  //connection = ooObjy::getConnection(bootfile);
  connection = objydb::Connection::connect(fdname.c_str());
  
  objy::db::Transaction* tx = new objy::db::Transaction(objy::db::OpenMode::Update, "spark_write");
  objy::data::Class clazz = objy::data::lookupClass("Person");
  cout << "found class: " << clazz.name() << " in the FD" << endl;
  tx->commit();
  tx->release();
  
  try {
    csv::IngestCSV ingester;

    if (_params.csvPathPattern.empty()) {
      cout << "Processing file: " << _params.csvFile << endl;
      ingester.ingest(_params.csvFile, _params.mapperFile, _params.commitEvery);
    } else {
      cout << "Processing files: " << _params.csvPathPattern << endl;
      cout << "... TBD... " << endl;
      //      Paths paths = new Paths();
      //      Paths results = paths.glob(null, _params.csvFiles);
      //      System.out.println("found: " + results.count());

      //      int count = 1;
      //      int totalCount = paths.getPaths().size();
      //      for(String csvFile : paths.getPaths())
      //      {
      //        LOG.info("Processing File: {}", csvFile);
      //        ingester.ingest(csvFile, _params.mapperFile, _params.commitEvery);
      //        LOG.info("Processed {} of {}", count, totalCount);
      //        count++;
      //      }
    }

  } catch (ooKernelException& e) {
    cout << e.what() << endl;
    return 1;
  } catch (ooBaseException& e) {
    cout << e.what() << endl;
    return 1;
  }

  return 0;
}

