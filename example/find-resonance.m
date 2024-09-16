pkg load signal

generate_chirp

[chirp1_stereo,Fs] = audioread('signal-out.wav');
chirp1=chirp1_stereo(:,1);
[signal1_stereo,Fs] = audioread('signal.wav');
signal1=signal1_stereo(:,2);
my_xcorr = xcorr(signal1-mean(signal1),chirp1-mean(chirp1));
%my_xcorr_positive_lags = my_xcorr((end+1)/2:end);
[correlation_max,correlation_max_index]=max(abs(my_xcorr))

signal_wo_delay = signal1(correlation_max_index-length(signal1):correlation_max_index-length(signal1)+length(chirp1)-1);

delta=1000;
offset=[0:delta:length(chirp1)-delta]';
for i = 1:length(offset)
  dotproduct_array(i)=dot(signal_wo_delay(1+offset(i):offset(i)+delta),chirp1(1+offset(i):offset(i)+delta));
end
format long
[value,index]=max(dotproduct_array)
resonant_frequency = start_freq + (stop_freq-start_freq)*index/length(dotproduct_array)
plot([start_freq:(stop_freq-start_freq)/length(dotproduct_array):stop_freq-(stop_freq-start_freq)/length(dotproduct_array)],dotproduct_array)
hold on
plot([resonant_frequency resonant_frequency],[0 value])
hold on
title(['dotproduct max index = ' num2str(resonant_frequency)])
