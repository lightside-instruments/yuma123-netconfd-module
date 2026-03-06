Scope:  R&S MXO5

*IDN?
??

DUT: network-interconnect-tester-cores-0.60
---

yangcli user@192.168.4.139> create /interfaces/interface[name='eth1']/traffic-generator -- interburst-gap=88 interframe-gap=20 frame-data=6CA96F0000026CA96F00000108004500002ED4A500000A115816C0000201C0000202C0200007001A00000102030405060708090A0B0C0D0E0F1011126EECF305 frame-size=64 frames-per-burst=3

RPC OK Reply 7 for session 2:

yangcli user@192.168.4.139> merge /interfaces/interface[name='eth1'] -- type=ethernetCsmacd

RPC OK Reply 8 for session 2:

yangcli user@192.168.4.139> commit

---

Connections:
DUT 192.168.4.139 (SFP0-5)
Scope 192.168.4.10

management interface SFP4
signal from SFP1+ -> ch1, SFP1- -> ch2
trigger from SFP2+

Differential acquisition with external trigger (5GHz sampling rate and ? GHz bandwidth):

./lsi-ivi-scope-acquisition-start-rs-rto-1024 10000000000.000000000 1000000 ext0 positive 0.4000000 ch2 2.0 0.0 ch4 2.0 0.0
./lsi-ivi-scope-acquisition-complete-rs-rto-1024 10000000000.000000000 1000000 ext0 positive 0.4000000 ch2 2.0 0.0 ch4 2.0 0.0

