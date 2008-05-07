%SK.cpp %.h : %.idl
	@IDL@ @IDL_FLAGS@ -bcxx -Wbh=.h -Wbs=SK.cpp -Wbkeep_inc_path $<

%.idl :
	cp @SI_PATH@/standardinterfaces/$@ .


AM_CXXFLAGS = -Wall

bin_PROGRAMS = soundCardPlayback
soundCardPlayback_SOURCES =         \
    main.cpp                        \
    soundCardPlayback.h             \
    soundCardPlayback.cpp           \
    port_impl.cpp

BUILT_SOURCES =                     \
    soundControlSK.cpp              \
    complexShortSK.cpp              \
    soundControl.idl                \
    complexShort.idl

CLEANFILES =                        \
    soundControlSK.cpp              \
    soundControl.h                  \
    complexShortSK.cpp              \
    complexShort.h                  \
    complexShort.idl                \
    soundControl.idl

nodist_soundCardPlayback_SOURCES =  \
    soundControlSK.cpp              \
    complexShortSK.cpp

ossieName = soundCardPlayback
xmldir = $(prefix)/dev/xml/$(ossieName)
bindir = $(prefix)/dev/bin

dist_xml_DATA =                     \
    soundCardPlayback.scd.xml       \
    soundCardPlayback.spd.xml       \
    soundCardPlayback.prf.xml


# ---------- documentation ---------

docs :
	doxygen Doxyfile

