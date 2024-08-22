pkg load signal

start_freq=1550
stop_freq=1700
samples=480000
sample_freq=48000

[chirp1_stereo,Fs] = audioread('chirp.wav');
chirp1=chirp1_stereo(:,1);
[signal1_stereo,Fs] = audioread('signal.wav');
signal1=signal1_stereo(:,2);
my_xcorr = xcorr(signal1-mean(signal1),chirp1-mean(chirp1));
%my_xcorr_positive_lags = my_xcorr((end+1)/2:end);
[correlation_max,correlation_max_index]=max(abs(my_xcorr))
[signal_max,signal_max_index]=max(abs(signal1-mean(signal1)))
[signal_max_hilbert,signal_max_index_hilbert]=max(abs(hilbert(signal1-mean(signal1))))
plot(abs(hilbert(signal1-mean(signal1))))
format long
resonance_peak_frequency = start_freq+(correlation_max_index-length(signal1)+signal_max_index)*(stop_freq-start_freq)/samples
resonance_peak_frequency_hilbert = start_freq+(correlation_max_index-length(signal1)+signal_max_index_hilbert)*(stop_freq-start_freq)/samples

