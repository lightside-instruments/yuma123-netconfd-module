Separate plus minus pair acquisition (100G samples/s rate and 20GHz bandwidth):
./lsi-ivi-scope-acquisition-start-tektronix-11801b 100000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
./lsi-ivi-scope-acquisition-complete-tektronix-11801b 100000000000.000000000 1000000 tr0 positive 0.400000000 ch1 2.000000000 0.0
cp tmp/lsi-ivi-scope-ch1-signal.wav measurements/spark-sfp-acquisition-tektronix-11801b-20260217/eth1-plus.wav
cp tmp/lsi-ivi-scope-ch2-signal.wav measurements/spark-sfp-acquisition-tektronix-11801b-20260217/eth1-minus.wav

Seems the 5G S/sec mxo5 signal was 427 samples behind -> signal-diff.png:

octave:190> [s1,Fs] = audioread('tmp/lsi-ivi-scope-ch1-signal.wav');
octave:191> [s2,Fs] = audioread('measurements/spark-sfp-acquisition-rs-mxo5/eth1-plus-diff.wav');
octave:192> plot(s1(1:20:(1000-427)*20))
octave:193> hold on
octave:194> plot(s2(427:1:1000))
