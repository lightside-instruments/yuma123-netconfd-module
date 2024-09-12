pkg load signal

freq=1000
samples=100
sample_freq=48000

audiowrite('signal-out.wav',sin([0:2*pi/samples:2*pi-2*pi/samples]),sample_freq);
