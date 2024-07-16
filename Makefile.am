netconfmodule_LTLIBRARIES = liblsi-gcode.la

liblsi_gcode_la_SOURCES = \
mod.c

# Depends on installed libyuma-dev
liblsi_gcode_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_gcode_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-gcode.yang

bin_SCRIPTS = gcode-run
