Separate plus minus pair acquisition (1THz sampling rate and 20GHz bandwidth):
./lsi-ivi-scope-acquisition-start-tektronix-11801b 1000000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
./lsi-ivi-scope-acquisition-complete-tektronix-11801b 1000000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
mv tmp/lsi-ivi-scope-ch1-signal.wav spark-sfp-acquisition/eth0-plus.wav
mv tmp/lsi-ivi-scope-ch2-signal.wav spark-sfp-acquisition/eth0-minus.wav
