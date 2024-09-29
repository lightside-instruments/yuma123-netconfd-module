pkg load signal
start_freq=1595
stop_freq=1610
#start_freq=1200
#start_freq=1597
#stop_freq=1700
#stop_freq=1602
samples=480000
sample_freq=48000

#audiowrite('signal-out.wav',[0.05*chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear')',zeros(1,samples)'],sample_freq);
chirp1_cos = chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear');
chirp1_sin = chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear',-90);
chirp1=chirp1_cos;
audiowrite('signal-out.wav',[0.05*chirp1',zeros(1,samples)'],sample_freq);
