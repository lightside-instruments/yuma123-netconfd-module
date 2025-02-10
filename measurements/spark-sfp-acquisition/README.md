*IDN?
Rohde&Schwarz,MXO44,1335.5050k04/201823,2.2.2.1

Separate plus minus pair acquisition (5GHz sampling rate (double) and 1.5GHz bandwidth):
./lsi-ivi-scope-acquisition-start-rs-mxo4 5000000000.000000000 1000000 ch1 positive 0.4000000 ch1 2.0 0.0
./lsi-ivi-scope-acquisition-complete-rs-mxo4 5000000000.000000000 1000000 ch1 positive 0.4000000 ch1 2.0 0.0
mv tmp/lsi-ivi-scope-ch1-signal.wav spark-sfp-acquisition/eth4-plus.wav


Differential acquisition (2.5GHz sampling rate and 1 GHz bandwidth):

./lsi-ivi-scope-acquisition-start-rs-mxo4 2500000000.000000000 1000000 ch1 positive 0.4000000 ch1 2.0 0.0 ch2 2.0 0.0
./lsi-ivi-scope-acquisition-complete-rs-mxo4 2500000000.000000000 1000000 ch1 positive 0.4000000 ch1 2.0 0.0 ch2 2.0 0.0
mv tmp/lsi-ivi-scope-ch1-signal.wav spark-sfp-acquisition/eth4-plus-diff.wav ; mv tmp/lsi-ivi-scope-ch2-signal.wav spark-sfp-acquisition/eth4-minus-diff.wav
