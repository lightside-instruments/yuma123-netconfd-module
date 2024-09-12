pkg load signal

start_freq=1550
stop_freq=1700
samples=480000
sample_freq=48000

audiowrite('signal-out.wav',[chirp([0:1/sample_freq:10-1/sample_freq],start_freq,10,stop_freq,'linear')',zeros(1,samples)'],sample_freq);

