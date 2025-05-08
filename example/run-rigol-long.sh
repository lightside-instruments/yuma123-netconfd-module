#!/usr/bin/bash -xe
export directory=`date +%Y%m%d%H%M%S`
mkdir report/${directory}
git log -1 > git.txt
git diff >> report/${directory}/git.txt
python3 test-w-generator.py --config=networks.xml --scope-name=scope-rigol --generator-name=generator0 --generator-amplitude=5.00 --scope-channel-name="ch1" --scope-channel-range=16.0 --generator-channel-name="ch1" --samples=6000000 --sample-rate=500000 --scope-trigger-source=ch2 --scope-trigger-level=1.00 --scope-trigger-slope=negative --scope-channel-parameters=" " --start-frequency=1580.00 --stop-frequency=1610.00 --sweep-time=10.0 | tee report/${directory}/log.txt
cp signal.wav report/${directory}
octave-cli find-resonance.m | tee report/${directory}/find_resonance.log
octave-cli --persist find-resonance.m

