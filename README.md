# Overview

YANG module based on the Switch IVI class

Allows configuration of channel and connections.


# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

# Supported devices
* HP 59306A - GPIB relay actuator - 6x 2 position (a,b) switches with 1 connection peer (c)
* LSI spark-relay-actuator-pi-3.x - same as HP 59306A but instead GPIB has GPIO control of the relays from a Raspberry Pi compatible device

In order to change the device replace the symbolic links for lsi-ivi-switch-set and lsi-ivi-switch-set before installing.

# Installation
```
autoreconf -i -f
./configure CFLAGS="-g -O0"  CXXFLAGS="-g -O0" --prefix=/usr
make
make install
```

# Testing installation
```
netconfd --module=lsi-ivi-switch  --no-startup --superuser=$USER
```

Other terminal:
```
yangcli --server=localhost --user=$USER

...

yangcli pi@localhost> create /channels/channel[name='a1']/connections -- connection='c1'
yangcli pi@localhost> create /channels/channel[name='c1']
yangcli pi@localhost> commit
```

Check the example/README for a example using the interface from python scripts.
