#!/usr/bin/python

# Display 1920x1080 test pattern
# python3 display-test-pattern.py --networks=networks.xml

from lxml import etree
import time
import sys, os
import argparse
import subprocess
import tntapi
import yangrpc
from yangcli import yangcli

def generate_image():
	#generate image.jpg.b64
	f = open("/tmp/image.rgb", "wb");
	for i in range(1080):
		for j in range(1920):
			if(j<(1920/3)):
				f.write(b'\x00\x00\x00\x00\xFF\xFF') # red
			elif(j<(2*1920/3)):
				f.write(b'\x00\x00\xFF\xFF\x00\x00') # green
			else:
				f.write(b'\xFF\xFF\x00\x00\x00\x00') # blue
	f.close()
	os.system("cat /tmp/image.rgb |  convert -verbose  -size 1920x1080 RGB:- /tmp/image.jpg")

	res = subprocess.check_output(["base64", "--wrap=0", "/tmp/image.jpg"])
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

yangcli(yconns["display"],"""delete /displays""")
tntapi.network_commit(conns)

image_b64 = generate_image()

ok=yangcli(yconns["display"],"""create /displays/display[name='default'] -- image=%s"""%(image_b64.decode('ascii'))).xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

