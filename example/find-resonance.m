pkg load signal

start_freq=1500
stop_freq=1700
samples=480000
sample_freq=48000

[chirp1,Fs] = audioread('chirp.wav');
[signal1,Fs] = audioread('signal.wav');
my_xcorr = xcorr(signal1(:,1)-mean(signal1(:,1)),chirp1(:,1)-mean(chirp1(:,1)));
my_xcorr_positive_lags = my_xcorr((end+1)/2:end);
[correlation_max,correlation_max_index]=max(abs(my_xcorr_positive_lags))
[signal_max,signal_max_index]=max(abs(signal1(:,1)-mean(signal1(:,1))))
[signal_max_hilbert,signal_max_index_hilbert]=max(abs(hilbert(signal1(:,1)-mean(signal1(:,1)))))
plot(abs(hilbert(signal1(:,1)-mean(signal1(:,1)))))
format long
resonance_peak_frequency = start_freq+(correlation_max_index+signal_max_index)*(stop_freq-start_freq)/samples
resonance_peak_frequency_hilbert = start_freq+(correlation_max_index+signal_max_index_hilbert)*(stop_freq-start_freq)/samples

