#!/usr/bin/python3

from lxml import etree
import time
import base64
import sys, os
import argparse
import subprocess
import tntapi
import yangrpc
from yangcli import yangcli

def generate_data():
	#generate image.jpg.b64
	os.system("octave-cli generate-chirp.m")
	res = subprocess.check_output(["base64", "--wrap=0", "chirp.wav"])
	return res


namespaces={"nc":"urn:ietf:params:xml:ns:netconf:base:1.0",
	"nd":"urn:ietf:params:xml:ns:yang:ietf-network",
	"nt":"urn:ietf:params:xml:ns:yang:ietf-network-topology"}

global args
args=None

parser = argparse.ArgumentParser()
parser.add_argument("--config", help="Path to the netconf configuration *.xml file defining the configuration according to ietf-networks, ietf-networks-topology and netconf-node models e.g. ../networks.xml")
parser.add_argument("--generator-channel", help="Name of generator channel e.g. 'default' or 'hw:1,0'")
parser.add_argument("--generator-waveform-type", help="Type of generated waveform e.g. 'sine', 'square' or 'dc'")
parser.add_argument("--generator-frequency", help="Frequency generated in Hz e.g. '1352.3'")
parser.add_argument("--generator-amplitude", help="Amplitude of generated waveform in Volts e.g. '1.1'")
parser.add_argument("--generator-dc-offset", help="DC offset of generated waveform in Volts e.g. '0.55'")
parser.add_argument("--scope-channel-name", help="Name of sope channel e.g. 'default' or 'hw:1,0'")
parser.add_argument("--scope-channel-range", help="Scope range in Volts e.g. 0.1")
parser.add_argument("--scope-channel-parameters", help="Scope implementation specific parameters e.g. '-c 2'")
parser.add_argument("--scope-trigger-source", help="Scope trigger source e.g. ch1")
parser.add_argument("--scope-trigger-level", help="Scope trigger level e.g. 0.6")
parser.add_argument("--scope-trigger-slope", help="Scope trigger slope e.g. positive or negative")
parser.add_argument("--samples", help="Scope acquisition total samples e.g. 480000")
parser.add_argument("--sample-rate", help="Sample rate for acquisition e.g. 48000")
args = parser.parse_args()

generator_frequency="1000"
scope_channel_name=args.scope_channel_name
scope_channel_range=float(args.scope_channel_range)
scope_channel_parameters=args.scope_channel_parameters
scope_trigger_source=args.scope_trigger_source
scope_trigger_level=float(args.scope_trigger_level)
scope_trigger_slope=args.scope_trigger_slope
generator_channel=args.generator_channel
generator_frequency=args.generator_frequency
generator_waveform_type=args.generator_waveform_type
generator_amplitude=float(args.generator_amplitude)
generator_dc_offset=float(args.generator_dc_offset)

samples = int(args.samples)
sample_rate = int(args.sample_rate)

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network)
yconns = tntapi.network_connect_yangrpc(network)

yangcli(yconns["scope0"],"""delete /acquisition""")
yangcli(yconns["generator0"],"""delete /channels""")
tntapi.network_commit(conns)

ok=yangcli(yconns["generator0"],"""replace /channels/channel[name='%s']/standard-function -- waveform-type=%s frequency=%s amplitude=%f dc-offset=%f"""%("default", generator_waveform_type, generator_frequency, generator_amplitude, generator_dc_offset)).xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

ok=yangcli(yconns["scope0"],"""replace /acquisition -- samples=%d sample-rate=%d"""%(samples, sample_rate)).xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns["scope0"],"""merge /acquisition/trigger -- source=%s level=%f slope=%s"""%(scope_trigger_source, scope_trigger_level, scope_trigger_slope)).xpath('./ok')
assert(len(ok)==1)


ok=yangcli(yconns["scope0"],"""merge /acquisition/channels/channel[name='%s'] -- range=%f parameters='%s'"""%(scope_channel_name, scope_channel_range, scope_channel_parameters)).xpath('./ok')
assert(len(ok)==1)


tntapi.network_commit(conns)


print("waiting 2 sec +  %u sec"%(samples/sample_rate))

time.sleep(480000/48000 + 2)

result=yangcli(yconns["scope0"],"""xget /acquisition/channels/channel[name='%s']"""%(scope_channel_name))

print(etree.tostring(result))
data=result.xpath('./data/acquisition/channels/channel/data')
#print(len(data))
#print(etree.tostring(data[0]))
print(len(data))
assert(len(data)==1)

data_b64 = data[0].text

f = open("signal.wav", "wb")
f.write(base64.b64decode(data_b64))
f.close()

ok=yangcli(yconns["scope0"],"""delete /acquisition""").xpath('./ok')
assert(len(ok)==1)
ok=yangcli(yconns["generator0"],"""delete /channels""").xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

