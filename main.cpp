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
//#include <locale>

#include <glob.h>  // once we move to c++17 we will use <filesystem> instead

#include <ooObjy.h>
#include <objy/Configuration.h>
#include <objy/Tools.h>
#include <objy/db/Connection.h>
#include <objy/db/Transaction.h>
#include <objy/db/TransactionScope.h>
#include "IngestCSV.h"
#include "src/utils/option.h"
#include "src/utils/ClassAccessor.h"

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
        .set_default("../data/testfd.boot")
        .help("bootfile file path (default ./data/fdtest.boot)");
    optionParser
        .add_option("-f", "--csvFile")
        .action("store")
        .type("string")
        .dest(CSVFilePath)
        .set_default("../source-data/addresses.csv")
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
        .set_default("../config/addressMapper.json")
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

  // set local (TBD: we need to make it configurable)
  // there is an issue with Placement and setting the global local... process will hang
  //std::locale::global(std::locale("en_US.utf8"));
  
  fdname =  _params.bootFile;
	try {
  ooObjy::setLoggingOptions(oocLogAll, true, false, "../logs");
  }
	catch (ooLogOpenFailed& ex) {
		cerr << "error setting up logging... " << ex.what() << endl;
    return -1;
	} 
  ooObjy::startup(24);

  objyconfig::ConfigurationManager* cfgMgr = objyconfig::ConfigurationManager::getInstance();
  cfgMgr->enableConfiguration(true, 0, 0, 0, 0);

  //connection = ooObjy::getConnection(bootfile);
  try { 
	  connection = objydb::Connection::connect(fdname.c_str());
  } catch (ooKernelException& ex) {
		cerr << "Failed to connect to FD (KernalException): " <<  ex.what() << endl;
		return -1;
  } catch (std::runtime_error& ex) {
		cerr << "Failed to connect to FD (RuntimeError): " <<  ex.what() << endl;
		return -1;
	}
 
	try {
	  objy::db::Transaction* tx = new objy::db::Transaction(objy::db::OpenMode::Update, "spark_write");
	  objy::data::Class clazz = objy::data::lookupClass("ooObj");
  	cout << "Connection to the DB checked" << endl;
  	tx->commit();
  	tx->release();
  } catch (ooKernelException& ex) {
		cerr << "Failed to connect (KernalException): " <<  ex.what() << endl;
		return -1;
	}
  
  try {
    csv::IngestCSV ingester;

    if (_params.csvPathPattern.empty()) {
      cout << "Processing file: " << _params.csvFile << endl;
      ingester.ingest(_params.csvFile, _params.mapperFile, _params.commitEvery);
    } else {
      cout << "Processing file pattern: " << _params.csvPathPattern << endl;
      
      /** The following is only supported on Linux, we'll change it to use c++17
          <filesystem> once moved to new compiler that support such functionality
       */
      vector<string> fileList;

      glob_t globbuf;
      globbuf.gl_offs = 0;
      
      glob(_params.csvPathPattern.c_str(), GLOB_DOOFFS, NULL, &globbuf);
      for (auto i = 0; i < globbuf.gl_pathc; i++) {
        //cout << i << ") found: " << globbuf.gl_pathv[i] << endl;
        fileList.push_back(string(globbuf.gl_pathv[i]));
      }
      globfree(&globbuf);

      int count = 1;
      int totalCount = fileList.size();
      for(string csvFile : fileList)
      {
        cout << "Processing File: " << csvFile << endl;
        ingester.ingest(csvFile, _params.mapperFile, _params.commitEvery);
        cout << "Processed " << count << " of " << totalCount << endl;
        count++;
      }
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

