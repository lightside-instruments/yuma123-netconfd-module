pkg load signal

args = argv();
nargs = length(args);

samplerate=100000

#freqs = []
#impedances = []
#phases = []

for i = 1:nargs

freq_str=args{i}
freq=str2num(args{i})

freqs(i)=freq;

filename=sprintf("results/signal-ch1-%s.wav",freq_str);
[signal1,Fs] = audioread(filename);
filename=sprintf("results/signal-ch2-%s.wav",freq_str);
[signal2,Fs] = audioread(filename);

signal1=signal1(400001:500000);
signal2=signal2(400001:500000);

signal1=signal1-mean(signal1);
signal2=signal2-mean(signal2);

signal1=2*signal1;
signal2=2*signal2;

current=(signal1-signal2)/10000;

v1_rms = rms(signal1)
v2_rms = rms(signal2)
current_rms = rms(current)

corr = xcorr(signal2,current);
period=2*floor((100000/freq)/2)
[value,index]=max(corr(100000-period/2:100000+period/2));
indexes(i)=index-period/2
current_phase=360*(index-period/2)/(samplerate/freq)

[x,y] = pol2cart(deg2rad(current_phase),v2_rms/current_rms);

impedance = complex(x,y)

phases(i)=current_phase;
impedances(i)=impedance;
endfor

plotyy(freqs, abs(impedances), freqs, phases)
