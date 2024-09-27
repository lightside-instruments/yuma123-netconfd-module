#!/usr/bin/bash -xe
export directory=`date +%Y%m%d%H%M%S`
mkdir report/${directory}
python3 test-w-generator.py --config=networks.xml --scope-name=scope0 --generator-name=generator0 --scope-channel-name="hw:CODEC,0" --scope-channel-range=4.0 --scope-channel-parameters="-c 2 -r 48000 -f S16_LE" --generator-channel-name="hw:CODEC,0" --generator-channel-sample-rate=48000.0 --samples=960000 --sample-rate=48000 --scope-trigger-source=ch3 --scope-trigger-level=0.4 --scope-trigger-slope=positive > report/${directory}/log.txt
cp signal.wav report/${directory}
octave-cli --persist find-resonance.m | tee report/${directory}/find_resonance.log
