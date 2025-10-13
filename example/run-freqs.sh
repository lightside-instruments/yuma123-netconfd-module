#!/usr/bin/bash -e
mkdir -p results
while read p; do
  echo "$p"
  cp results/signal-ch1-${p}.wav signal-ch1.wav 
  cp results/signal-ch2-${p}.wav signal-ch2.wav
  md5sum signal-ch1.wav signal-ch2.wav
done <freqs.txt
