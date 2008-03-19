lib_LTLIBRARIES = libossiecf.la
libossiecf_la_SOURCES = ApplicationFactory_impl.cpp File_impl.cpp Application_impl.cpp FileManager_impl.cpp Device_impl.cpp FileSystem_impl.cpp DeviceManager_impl.cpp Resource_impl.cpp DomainManager_impl.cpp applicationSupport.cpp debug.cpp helperFunctions.cpp ossieComponent.cpp

EXTRA_DIST = dirent.h

libossiecf_la_LDFLAGS = -version-info 0:4:0

AM_CXXFLAGS = -Wall -g

INCLUDES = -I../include -I../parser
