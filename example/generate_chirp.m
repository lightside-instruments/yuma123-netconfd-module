pkg load signal

start_freq=1595
#start_freq=1597
stop_freq=1610
#stop_freq=1602
samples=480000
sample_freq=48000

#audiowrite('signal-out.wav',[0.05*chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear')',zeros(1,samples)'],sample_freq);
chirp1 = chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear');
audiowrite('signal-out.wav',[0.05*chirp1',zeros(1,samples)'],sample_freq);
