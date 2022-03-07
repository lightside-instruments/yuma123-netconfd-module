netconfmodule_LTLIBRARIES = liblsi-camera.la

liblsi_camera_la_SOURCES = \
lsi-camera.c

# Depends on installed libyuma-dev
liblsi_camera_la_CPPFLAGS = -I${includedir}/yuma/agt -I${includedir}/yuma/ncx -I${includedir}/yuma/platform $(XML_CPPFLAGS)
liblsi_camera_la_LDFLAGS = -module -lyumaagt -lyumancx

yang_DATA = lsi-camera.yang

bin_SCRIPTS = lsi-camera-cameras-get lsi-camera-cameras-get-v4l2 lsi-camera-cameras-get-pi
