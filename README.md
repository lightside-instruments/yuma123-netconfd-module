# Overview

YANG module based on the DC Power Supply IVI class

Allows configuration of Voltage and Current limits for each channel and
monitoring of the current values of the Current and Voltage for each channel.


# Dependencies
Installed netconfd and yangcli
```
apt-get install netconfd yangcli
```

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
```
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

