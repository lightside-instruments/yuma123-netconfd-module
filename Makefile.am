netconfmodule_LTLIBRARIES = liblsi-rover.la

liblsi_rover_la_SOURCES = \
lsi-rover.c

# Depends on installed libyuma-dev
liblsi_rover_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_rover_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-rover.yang

bin_SCRIPTS = lsi-rover-activate lsi-rover-dock lsi-rover-move
