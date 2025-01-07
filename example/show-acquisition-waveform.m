pkg load signal

generate_chirp

[chirp1_stereo,Fs] = audioread('signal-out.wav');
chirp1=chirp1_stereo(:,1);
[signal1,Fs] = audioread('signal.wav');

plot(chirp1*generator_amplitude/2)
hold on
plot(signal1*scope_channel_range/2)
title('Raw signal')
