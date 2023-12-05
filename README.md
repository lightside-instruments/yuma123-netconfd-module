# Overview

YANG module for driving around a rover

Currently allows activating the rover from sleep and rotating it with the precision of integer degrees from 0 to 359

# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

# Supported devices
* iRoomba 600

In order to change the device replace the symbolic links for lsi-rover-activate, lsi-rover-rotate, lsi-rover-dock

# Installation
```
autoreconf -i -f
./configure CFLAGS="-g -O0"  CXXFLAGS="-g -O0" --prefix=/usr
make
make install
```

# Testing installation
```
netconfd --module=lsi-rover  --no-startup --superuser=$USER
```

Other terminal:
```
yangcli --server=localhost --user=$USER

...

yangcli pi@localhost> create /rover -- angle=0
yangcli pi@localhost> commit
yangcli pi@localhost> replace /rover -- angle=180
yangcli pi@localhost> commit
yangcli pi@localhost> delete /rover
yangcli pi@localhost> commit
```

