
AM_CXXFLAGS = -Wall

ossieName = soundCardPlayback
xmldir = $(prefix)/dev/xml/$(ossieName)
bindir = $(prefix)/dev/bin

bin_PROGRAMS = soundCardPlayback

soundCardPlayback_SOURCES =         \
    main.cpp                        \
    soundCardPlayback.h             \
    soundCardPlayback.cpp           

dist_xml_DATA =                     \
    soundCardPlayback.scd.xml       \
    soundCardPlayback.spd.xml       \
    soundCardPlayback.prf.xml


# ---------- documentation ---------

docs :
	doxygen Doxyfile

