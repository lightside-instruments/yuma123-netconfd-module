% Method for system characterization based on frequency
% sweep probing signal and calculation of pairs of orthogonal
% signal components of the returned signal.
%
% The calculation of the orthogonal signal components can be implemented by
% calculation of a dot product of the samples representing short periods
% of the signal sent and the signal received back and a dot product of the
% orthogonal signal to the one sent and the signal received back.
%
% The orthogonal signals can be cosine and sine frequency sweeps.

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

signal = signal_wo_delay-mean(signal_wo_delay);
chirp_cos = chirp1_cos;
chirp_sin = chirp1_sin;
delta=1000

offset = [0:delta:length(signal)-1];
for i = 1:length(offset)
  dotproduct_chirp_cos(i)=dot(signal(1+offset(i):offset(i)+delta),chirp_cos(1+offset(i):offset(i)+delta));                                                                                   
  dotproduct_chirp_sin(i)=dot(signal(1+offset(i):offset(i)+delta),chirp_sin(1+offset(i):offset(i)+delta));
end
result=complex(dotproduct_chirp_cos,dotproduct_chirp_sin);

figure(1)
magnitude = abs(result);
plot([start_freq:(stop_freq-start_freq)/length(magnitude):stop_freq-(stop_freq-start_freq)/length(magnitude)],magnitude);
[value,index]=max(magnitude)
resonant_peak_frequency = start_freq + (stop_freq-start_freq)*(index-1)/length(magnitude)
hold on
plot([resonant_peak_frequency resonant_peak_frequency],[min(magnitude) max(magnitude)])
hold on
title(['Magnitude (max_index) = ' num2str(resonant_peak_frequency)])

figure(2)
plot(angle(result))
title('Angle')

figure(3)
plot(signal)
title('Raw signal')
