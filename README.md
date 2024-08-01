# Overview

YANG module based on the Function Generator IVI class

Allows configuration of arbitrary waveform data for each channel which is generated after commit


# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

# Supported devices
* ALSA aplay where the channel name is the same as the device name e.g. "default" or "hw:0,1"


# Installation
```
autoreconf -i -f
./configure CFLAGS="-g -O0"  CXXFLAGS="-g -O0" --prefix=/usr
make
make install
```

# Testing installation
```
netconfd --module=lsi-ivi-function-generator  --no-startup --superuser=$USER
```

Other terminal:
```
yangcli --server=localhost --user=$USER

...

yangcli pi@localhost> create /channels/channel[name='default'] --data='...'

yangcli pi@localhost> commit

```

In this implementation there is a deviation and the sample data is not double precision float array represented as base64 but wav file playable with the following command:

> aplay -D "default" signal.wav

The data is the base64 representation of the encoded wav file.
