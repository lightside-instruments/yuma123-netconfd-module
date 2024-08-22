
for i in {1..10}
do
  python3 test-w-generator.py --config=networks-lab.xml --scope-channel="hw:U18dB,0" --scope-parameters="-c 2 -r 48000 -f S24_3LE" --generator-channel="hw:CODEC,0" --samples=480000 --sample-rate=48000
  octave-cli find-resonance.m
  python3 ../generator-start.py
  time python3 ../oscilloscope-acquire.py | grep 'ch1=' | tee ch1.m
done
