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
args = parser.parse_args()

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network)
yconns = tntapi.network_connect_yangrpc(network)

yangcli(yconns["scope0"],"""delete /acquisition""")
tntapi.network_commit(conns)

ok=yangcli(yconns["scope0"],"""create /acquisition -- samples=480000""").xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns["scope0"],"""create /acquisition/channels/channel[name='default']""").xpath('./ok')
assert(len(ok)==1)


tntapi.network_commit(conns)

time.sleep(480000/48000)

data=yangcli(yconns["scope0"],"""xget /acquisition/channels/channel[name='default']""") .xpath('./data/acquisition/channels/channel/data')
#print(len(data))
#print(etree.tostring(data[0]))
assert(len(data)==1)

data_b64 = data[0].text

f = open("signal.wav", "wb")
f.write(base64.b64decode(data_b64))
f.close()

#ok=yangcli(yconns["scope0"],"""delete /acquisition""").xpath('./ok')
#assert(len(ok)==1)

#tntapi.network_commit(conns)

