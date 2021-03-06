cmake_minimum_required (VERSION 2.8)
project (ingest-csv-cpp)

message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
ELSE()
  set(CMAKE_BUILD_TYPE Release)
  message("Release build.")
ENDIF()

#set(CMAKE_CXX_FLAGS "-std=c++11") 
#set(CMAKE_CXX_FLAGS " -O6 -g0  -m64 -Wall -flto -msse3 -Wextra -Wformat -pedantic -std=c++11 -ffast-math -march=native -fno-check-new -funroll-loops -Wno-deprecated -falign-loops=16 -Wformat-security -fstrict-aliasing -Wstrict-aliasing=2 -Wno-strict-overflow -Wno-unused-variable -Wno-variadic-macros -fomit-frame-pointer -Wno-unused-parameter -finline-limit=1000000 -Wno-unused-private-field -Wno-unused-local-typedefs")

set(CMAKE_CXX_FLAGS " -O6 -fno-inline -m64 -Wall -msse3 -Wextra -Wformat -pedantic -std=c++11 -ffast-math -march=native -D__extern_always_inline=\"\" -funroll-loops -Wno-deprecated -Wformat-security -fstrict-aliasing -Wstrict-aliasing=2 -Wno-strict-overflow -Wno-unused-variable -Wno-variadic-macros -Wno-unused-parameter -fno-inline -Wno-unused-private-field -Wno-unused-local-typedefs -Wno-unknown-pragmas")

ADD_DEFINITIONS(-DNDEBUG)
ADD_DEFINITIONS(-DWANT_DENSE)
ADD_DEFINITIONS(-D__STDC_FORMAT_MACROS=1)
#ADD_DEFINITIONS(-D_GLIBCXX_USE_CXX11_ABI=0)

SET(OBJY_VERSION 12.5)

include_directories(. ./src /opt/Objectivity/${OBJY_VERSION}/include)
 
SET(SOURCES main.cpp)

add_subdirectory (src)
add_subdirectory (src/utils)

set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_CXX_FLAGS})

#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /opt/Objectivity/12.4/lib)
link_directories(/opt/Objectivity/${OBJY_VERSION}/lib)

SET(SYSTEM_LIBS dl)
SET(OBJYLIBS oo.${OBJY_VERSION} ooSessionManager.${OBJY_VERSION} ooConfiguration.${OBJY_VERSION} ooObjectModel.${OBJY_VERSION} ooData.${OBJY_VERSION} ooUtil.${OBJY_VERSION} ooError.${OBJY_VERSION} ooTool.${OBJY_VERSION} ooTargetFinder.${OBJY_VERSION} ooPersistentDataStore.${OBJY_VERSION})

add_executable(ingest_csv ${SOURCES} main.cpp)

target_link_libraries(ingest_csv LINK_PUBLIC ${SYSTEM_LIBS} ${OBJYLIBS})
