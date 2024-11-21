
pkg load signal
sample_rate=50000.000000
start_freq=1595.000000
stop_freq=1610.000000
sweep_time=10.000000
gain=1.000000
samples=sweep_time*sample_rate

chirp1_cos = chirp([0:1/sample_rate:sweep_time-1/sample_rate],start_freq,sweep_time,stop_freq,'linear');
chirp1_sin = chirp([0:1/sample_rate:sweep_time-1/sample_rate],start_freq,sweep_time,stop_freq,'linear',-90);
chirp1=chirp1_cos;
audiowrite('signal-out.wav',[gain*chirp1',zeros(1,samples)'],sample_rate);
