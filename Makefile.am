netconfmodule_LTLIBRARIES = liblsi-gpios.la

liblsi_gpios_la_SOURCES = \
gpios.c

# Depends on installed libyuma-dev
liblsi_gpios_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_gpios_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-gpios.yang lsi-gpios-raspberry-pi-names.yang

bin_SCRIPTS = gpios-init gpios-set gpios-get

