AM_CXXFLAGS = -Wall

devbindir = $(prefix)/dev/bin
devbin_PROGRAMS = USRP

USRP_SOURCES =                  \
    main.cpp                \
    USRP.cpp                \
    port_impl.cpp           \
    USRP.h                  \
    db_base.h               \
    db_base.cpp             \
    flex.h                  \
    flex.cpp                \
    basic.h                 \
    basic.cpp               \
    dbsrx.h                 \
    dbsrx.cpp               \
    tvrx.h                  \
    tvrx.cpp                \
    lf.h                    \
    lf.cpp

ossieName = USRP
xmldir = $(prefix)/dev/xml/$(ossieName)

dist_xml_DATA =                 \
    xml/USRP.scd.xml            \
    xml/USRP.spd.xml


# ---------- documentation ---------

# make sure "make docs" doesn't conflict with
# separate docs/ directory
.PHONY: docs documentation

docs :
	./genDocs.py


