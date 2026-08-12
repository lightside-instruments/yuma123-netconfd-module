netconfmodule_LTLIBRARIES = libietf-hardware-state.la

libietf_hardware_state_la_SOURCES = \
ietf-hardware-state.c

# Depends on installed libyuma-dev
libietf_hardware_state_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
libietf_hardware_state_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-storage-drive-ext.yang

bin_SCRIPTS = hardware-state-get
