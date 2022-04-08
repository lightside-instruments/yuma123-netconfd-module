netconfmodule_LTLIBRARIES = liblsi-ivi-switch.la

liblsi_ivi_switch_la_SOURCES = \
lsi-ivi-switch.c

# Depends on installed libyuma-dev
liblsi_ivi_switch_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_ivi_switch_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-ivi-switch.yang

bin_SCRIPTS = lsi-ivi-switch-set lsi-ivi-switch-gpib-set
