#!/usr/bin/bash -e
mkdir -p results
while read p; do
  echo "$p"
  python3 ./test-w-generator-standard-function.py --config=networks.xml --scope-name=scope-rigol --generator-name=generator0 --scope-channel-name=ch1,ch2 --scope-channel-range="4.0,4.0" --scope-channel-parameters="-,-" --generator-channel="ch1" --generator-waveform-type=sine --generator-amplitude=1.0 --generator-dc-offset=0 --samples=600000 --sample-rate=100000 --generator-frequency=$p --scope-trigger-source=ch2 --scope-trigger-level=0.1 --scope-trigger-slope=positive
  mv signal-ch1.wav results/signal-ch1-${p}.wav
  mv signal-ch2.wav results/signal-ch2-${p}.wav
done <freqs.txt
