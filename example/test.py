#!/usr/bin/python

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
	for i in range(480):
		for j in range(640):
			if(j<(640/3)):
				f.write(b'\x00\x00\x00\x00\xFF\xFF') # red
			elif(j<(2*640/3)):
				f.write(b'\x00\x00\xFF\xFF\x00\x00') # green
			else:
				f.write(b'\xFF\xFF\x00\x00\x00\x00') # blue
	f.close()
	os.system("cat /tmp/image.rgb |  convert -verbose  -size 640x480 RGB:- /tmp/image.jpg")

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

for node_name in yconns.keys():
	ok=yangcli(yconns[node_name],"""delete /displays""").xpath('./ok')
tntapi.network_commit(conns)

image_b64 = generate_image()

for node_name in yconns.keys():
	# Set output voltage
	ok=yangcli(yconns[node_name],"""create /displays/display[name='default'] -- image=%s"""%(image_b64.decode('ascii'))).xpath('./ok')
	assert(len(ok)==1)

tntapi.network_commit(conns)

