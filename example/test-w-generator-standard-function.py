#!/usr/bin/python

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
parser.add_argument("--scope-channel", help="Name of sope channel e.g. 'default' or 'hw:1,0'")
parser.add_argument("--scope-parameters", help="Scope parameters e.g. '-c 2 -f S16LE'")
parser.add_argument("--samples", help="Scope acquisition total samples e.g. 480000")
parser.add_argument("--sample-rate", help="Sample rate for acquisition e.g. 48000")
args = parser.parse_args()

scope_channel=args.scope_channel
scope_parameters=args.scope_parameters
generator_channel=args.generator_channel
samples = int(args.samples)
sample_rate = int(args.sample_rate)

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network)
yconns = tntapi.network_connect_yangrpc(network)

yangcli(yconns["scope0"],"""delete /acquisition""")
yangcli(yconns["generator0"],"""delete /channels""")
tntapi.network_commit(conns)

ok=yangcli(yconns["generator0"],"""replace /channels/channel[name='%s']/standard-function -- waveform-type=%s frequency=1000 amplitude=1.65 dc-offset=0.8250"""%("default", "square")).xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns["scope0"],"""replace /acquisition -- samples=%d sample-rate=%d"""%(samples, sample_rate)).xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns["scope0"],"""merge /acquisition/channels/channel[name='%s'] -- range=10.0 parameters='%s'"""%(scope_channel, scope_parameters)).xpath('./ok')
assert(len(ok)==1)


tntapi.network_commit(conns)


print("waiting 10")

time.sleep(10)

print("deleting")

time.sleep(480000/48000 + 2)

result=yangcli(yconns["scope0"],"""xget /acquisition/channels/channel[name='%s']"""%(scope_channel))

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

