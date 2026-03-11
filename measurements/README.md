We compare the first and last* 1000 samples of 1000BASE-X waveform generated with a traffic generator through 10 us long acquisition with 3 different scopes.

According to the traffic generator configuration used the signal should have a period of 2560 nano seconds: (3*64+2*20+88)*8

Since there are 5 samples per nanosecond the generated signal should repeat itself with a period of 12800 samples.


Start octave-cli:
```
pkg load signal
[signal_mxo5_plus,fs]=audioread("spark-sfp-acquisition-rs-mxo5/eth1-plus-diff.wav");
[signal_mxo5_minus,fs]=audioread("spark-sfp-acquisition-rs-mxo5/eth1-minus-diff.wav");
[signal_rto_1024_plus,fs]=audioread("spark-sfp-acquisition-rs-rto-1024/eth1-plus-diff.wav");
[signal_rto_1024_minus,fs]=audioread("spark-sfp-acquisition-rs-rto-1024/eth1-minus-diff.wav");
[signal_11801b_19_plus,fs]=audioread("spark-sfp-acquisition-tektronix-11801b-20260219/eth1-plus.wav");
[signal_11801b_19_minus,fs]=audioread("spark-sfp-acquisition-tektronix-11801b-20260219/eth1-minus.wav");
signal_mxo5=signal_mxo5_plus-signal_mxo5_minus;
signal_rto_1024=signal_rto_1024_plus-signal_rto_1024_minus;
signal_11801b_19=signal_11801b_19_plus-signal_11801b_19_minus;
signal_mxo5_50k=signal_mxo5(1:1:50000);
signal_rto_1024_50k=signal_rto_1024(1:2:2*50000);
signal_11801b_19_50k=signal_11801b_19(1:20:20*50000);
[a,b]=xcorr(signal_rto_1024_50k,signal_11801b_19_50k);
[v,i]=max(a)
% v = 8759.5
% i = 50727
[a,b]=xcorr(signal_mxo5_50k,signal_11801b_19_50k);
[v,i]=max(a)
% v = 9598.9
% i = 50838
plot(signal_11801b_19_50k(1:1000))
hold on
plot(signal_rto_1024_50k(727+1:1000+727))
hold on
plot(signal_mxo5_50k(838+1:1000+838))
print("-dpng", "200ns-1000samples-first.png");
hold off
plot(signal_11801b_19_50k(end-2000+1:end-1000))
hold on
plot(signal_rto_1024_50k(end-2000+727+1:end-1000+727))
hold on
plot(signal_mxo5_50k(end-2000+838+1:end-1000+838))
print("-dpng", "200ns-1000samples-last.png");
```
