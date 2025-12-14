Configure traffic generator:
 merge /interfaces/interface[name='eth1']/traffic-generator -- frames-per-burst=3 interframe-gap=20 frame-size=64 frame-data=0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 interburst-gap=88
 commit
Separate plus minus pair acquisition (100GHz sampling rate and 20GHz bandwidth):
./lsi-ivi-scope-acquisition-start-tektronix-11801b 100000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
./lsi-ivi-scope-acquisition-complete-tektronix-11801b 100000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
mv tmp/lsi-ivi-scope-ch1-signal.wav measurement/spark-sfp-acquisition-tektronix-11801b-4/eth0-plus.wav
mv tmp/lsi-ivi-scope-ch2-signal.wav measurement/spark-sfp-acquisition-tektronix-11801b-4/eth0-minus.wav

