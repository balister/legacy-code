AM_CXXFLAGS = -Wall

devbindir = $(prefix)/dev/bin
devbin_PROGRAMS = USRP

USRP_SOURCES =                  \
    src/main.cpp                \
    src/USRP.cpp                \
    src/port_impl.cpp           \
    src/USRP.h                  \
    src/db_base.h               \
    src/db_base.cpp             \
    src/flex.h                  \
    src/flex.cpp                \
    src/basic.h                 \
    src/basic.cpp               \
    src/dbsrx.h                 \
    src/dbsrx.cpp               \
    src/tvrx.h                  \
    src/tvrx.cpp                \
    src/lf.h                    \
    src/lf.cpp

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


