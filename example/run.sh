#!/usr/bin/bash -xe
export directory=`date +%Y%m%d%H%M%S`
mkdir report/${directory}
#python3 test-w-generator.py --config=networks.xml --scope-name=scope0 --generator-name=generator0 --generator-amplitude=0.50 --scope-channel-name="hw:CODEC,0" --scope-channel-range=4.0 --scope-channel-parameters="-c 2 -r 48000 -f S16_LE" --generator-channel-name="default" --samples=960000 --sample-rate=48000 --scope-trigger-source=ch3 --scope-trigger-level=0.4 --scope-trigger-slope=positive > report/${directory}/log.txt
python3 test-w-generator.py --config=networks.xml --scope-name=scope0 --generator-name=generator0 --generator-amplitude=1.00 --scope-channel-name="ch1" --scope-channel-range=4.0 --generator-channel-name="default" --samples=1000000 --sample-rate=50000 --scope-trigger-source=ch1 --scope-trigger-level=0.4 --scope-trigger-slope=positive --scope-channel-parameters=" "
cp signal.wav report/${directory}
# octave-cli find-resonance.m | tee report/${directory}/find_resonance.log
octave-cli --persist find-resonance.m

