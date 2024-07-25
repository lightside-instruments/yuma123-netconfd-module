netconfmodule_LTLIBRARIES = liblsi-thermometers.la

liblsi_thermometers_la_SOURCES = \
lsi-thermometers.c

# Depends on installed libyuma-dev
liblsi_thermometers_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_thermometers_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-thermometers.yang

bin_SCRIPTS = thermometers-get
