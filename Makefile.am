netconfmodule_LTLIBRARIES = liblsi-ivi-dc-power.la

liblsi_ivi_dc_power_la_SOURCES = \
lsi-ivi-dc-power.c

# Depends on installed libyuma-dev
liblsi_ivi_dc_power_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_ivi_dc_power_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-ivi-dc-power.yang

bin_SCRIPTS = lsi-ivi-dc-power-set lsi-ivi-dc-power-gpib-e364xa-set
