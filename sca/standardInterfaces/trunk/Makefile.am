%.pp.idl : %.idl
	@CC@ -E -x c++ $< > $@

%SK.cpp %.h %DynSK.cpp : %.idl
	@IDL@ @IDL_FLAGS@ -bcxx -I/usr/include -Wba -Wbd=DynSK.cpp -Wbh=.h -Wbs=SK.cpp -Wbkeep_inc_path $<
	cp $*.h include/standardinterfaces

SUBDIRS = include

BUILT_SOURCES =             \
    realCharSK.cpp          \
    realShortSK.cpp         \
    realLongSK.cpp          \
    realFloatSK.cpp         \
    realDoubleSK.cpp        \
    complexShortSK.cpp      \
    complexFloatSK.cpp      \
    complexDoubleSK.cpp     \
    complexCharSK.cpp       \
    complexLongSK.cpp       \
    FM_ControlSK.cpp        \
    soundControlSK.cpp      \
    stereoSoundSK.cpp       \
    realStringSK.cpp       \
    Radio_ControlSK.cpp

nodist_pkginclude_HEADERS = \
    realShort.h             \
    realLong.h              \
    realChar.h              \
    realFloat.h             \
    realDouble.h            \
    complexShort.h          \
    complexFloat.h          \
    complexDouble.h         \
    complexChar.h           \
    complexLong.h           \
    FM_Control.h            \
	soundControl.h          \
    stereoSound.h           \
    realString.h           \
    Radio_Control.h 

AM_CXXFLAGS = -Wall -g

INCLUDES = -I./include

pkginclude_HEADERS =        \
    realChar.idl            \
    realShort.idl           \
    realLong.idl            \
    realFloat.idl           \
    realDouble.idl          \
    complexShort.idl        \
    complexFloat.idl        \
    complexDouble.idl       \
    complexChar.idl         \
    complexLong.idl         \
    Radio_Control.idl       \
    FM_Control.idl          \
    soundControl.idl        \
    stereoSound.idl         \
    sampleRateConvertorControl.idl \
    realString.idl

pkgconfigdir = $(libdir)/pkgconfig

dist_pkgconfig_DATA = standardInterfaces.pc

lib_LTLIBRARIES = libstandardInterfaces.la

libstandardInterfaces_la_SOURCES =              \
    realShort_u.cpp     realShort_p.cpp         \
    realLong_u.cpp      realLong_p.cpp          \
    realFloat_u.cpp     realFloat_p.cpp         \
    realDouble_u.cpp    realDouble_p.cpp        \
    realChar_u.cpp      realChar_p.cpp          \
    realString_u.cpp    realString_p.cpp        \
    complexChar_u.cpp   complexChar_p.cpp       \
    complexShort_u.cpp  complexShort_p.cpp      \
    complexFloat_u.cpp  complexFloat_p.cpp      \
    complexDouble_u.cpp complexDouble_p.cpp     \
    complexLong_u.cpp   complexLong_p.cpp       \
    Radio_Control_u.cpp Radio_Control_p.cpp     \
    Resource_u.cpp 

nodist_libstandardInterfaces_la_SOURCES =       \
    realShortSK.cpp         \
    realLongSK.cpp          \
    realFloatSK.cpp         \
    realDoubleSK.cpp        \
    realCharSK.cpp          \
    realStringSK.cpp          \
    complexShortSK.cpp      \
    complexFloatSK.cpp      \
    complexDoubleSK.cpp     \
    complexCharSK.cpp       \
    complexLongSK.cpp       \
    Radio_ControlSK.cpp 

libstandardInterfaces_la_LDFLAGS = -version-info 0:6:0

CLEANFILES =                                    \
    realShort.h         realShortSK.cpp         \
    realLong.h          realLongSK.cpp          \
    realFloat.h         realFloatSK.cpp         \
    realDouble.h        realDoubleSK.cpp        \
    realChar.h          realCharSK.cpp          \
    realString.h        realStringSK.cpp        \
    complexShort.h      complexShortSK.cpp      \
    complexFloat.h      complexFloatSK.cpp      \
    complexDouble.h     complexDoubleSK.cpp     \
    complexChar.h       complexCharSK.cpp       \
    complexLong.h       complexLongSK.cpp       \
    Radio_Control.h     Radio_ControlSK.cpp     \
    standardIdl.pp.idl

python :
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline realChar.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline realShort.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline realLong.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline realFloat.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline realDouble.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline complexShort.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline complexFloat.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline complexDouble.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline complexChar.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline complexLong.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline Radio_Control.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline FM_Control.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline soundControl.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline stereoSound.idl
	omniidl -bpython -I/usr/local/include -Wbpackage=ossie.standardinterfaces -Wbinline sampleRateConvertorControl.idl 

# ---------- documentation ---------
.PHONY: docs documentation python

docs :
	./genDocs.py


