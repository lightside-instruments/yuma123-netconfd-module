pkg load signal
#audiowrite('chirp.wav',chirp([0:1/48000:10],1000,10,2000,'linear'),48000); %  1khz-2khz 10sec
audiowrite('chirp.wav',[chirp([0:1/48000:10-1/48000],1000,10,2000,'linear')',zeros(1,480000)'],48000); %  1khz-2khz 10sec

