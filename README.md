# Overview

YANG module based on the DC Power Supply IVI class

Allows configuration of Voltage and Current limits for each channel and
monitoring of the current values of the Current and Voltage for each channel.


# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

# Supported devices
* Agilient/Keysight E3647A (default)
* Ember - portable power supply by Blue Smoke Electronics

In order to change the device replace the symbolic links for lsi-ivi-dc-power-get and lsi-ivi-dc-power-get before installing.

# Installation
```
autoreconf -i -f
./configure CFLAGS="-g -O0"  CXXFLAGS="-g -O0" --prefix=/usr
make
make install
```

# Testing installation
```
export LSI_IVI_DC_POWER_VISA_RESOURCE_NAME="TCPIP::192.168.14.20::gpib,2::INSTR"
netconfd --module=lsi-ivi-dc-power  --no-startup --superuser=$USER
```

Other terminal:
```
yangcli --server=localhost --user=$USER

...

yangcli pi@localhost> create /outputs/output[name='out1'] -- voltage-level=0.8 current-limit=0.2

yangcli pi@localhost> commit

yangcli pi@localhost> xget /outputs-state

rpc-reply {
  data {
    outputs-state {
      output out1 {
        name out1
        measurement {
          voltage 0.803552500
          current 0.014911850
        }
      }
      output out2 {
        name out2
        measurement {
          voltage 0.001579323
          current -0.000341327
        }
      }
    }
  }
}
 
yangcli pi@localhost> delete /outputs
 
yangcli pi@localhost>
```

# Starting multiple netconfd instances
It is possible to start multiple instances of different device types. Here an example of starting a secondary instance on port 10830:
```
mkdir ember0
cd ember0
ln -s ../lsi-ivi-dc-power-gpib-ember-get lsi-ivi-dc-power-get
ln -s ../lsi-ivi-dc-power-gpib-ember-set lsi-ivi-dc-power-set
export PATH=`pwd`:"$PATH"
export LSI_IVI_DC_POWER_VISA_RESOURCE_NAME="TCPIP::192.168.4.103::serial,ACM0::INSTR"
netconfd --module=lsi-ivi-dc-power  --no-startup --ncxserver-sockname=/tmp/ncxserver-10830.sock --port=10830 --superuser=y123
'''

Keep in mind you need to have the corresponding ports enabled in /etc/ssh/sshd_config:

```
...
Port 830
Port 10830
Subsystem netconf "/usr/sbin/netconf-subsystem --ncxserver-sockname=10831@/tmp/ncxserver.sock --ncxserver-sockname=10830@/tmp/ncxserver-10830.sock"
```
