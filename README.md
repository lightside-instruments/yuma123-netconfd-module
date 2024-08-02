# Overview

YANG module based on the Scope IVI class

Allows acquisition of sample data for each channel which is recorded after commit


# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

# Supported devices
* ALSA arecord where the channel name is the same as the device name e.g. "default" or "hw:0,1"


# Installation
```
autoreconf -i -f
./configure CFLAGS="-g -O0"  CXXFLAGS="-g -O0" --prefix=/usr
make
make install
```

# Testing installation
```
netconfd --module=lsi-ivi-scope  --no-startup --superuser=$USER
```

Other terminal:
```
yangcli --server=localhost --user=$USER

...

yangcli pi@localhost> create /acquisition -- sample-rate=48000 samples=480000

yangcli pi@localhost> commit

```

In this implementation there is a deviation and the sample data is not double precision float array represented as base64 but base64 representation of the encoded wav file.
