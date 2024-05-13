netconfmodule_LTLIBRARIES = liblsi-lights.la

liblsi_lights_la_SOURCES = \
mod.c

# Depends on installed libyuma-dev
liblsi_lights_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_lights_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-lights.yang

bin_SCRIPTS = lights-init lights-set
