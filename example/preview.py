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


image_b64 = generate_image().decode('ascii')


result=yangcli(yconns["display"],"""delete /displays""")
if (result==None):
	exit(0)

if(len(result.xpath('./ok'))!=1):
	print(etree.tostring(result))


tntapi.network_commit(conns)

while 1:
	filter ="""<filter type="xpath" select="/cameras"/>"""
	state = tntapi.network_get_state(network, conns, filter=filter)
	state_wo_ns=tntapi.strip_namespaces(state)
	images = state_wo_ns.xpath("node/data/cameras/camera/image")

#	results=yangcli(yconns["camera1"],"""xget /cameras/camera/image""").xpath('./data/cameras/camera/image')
#	for result in results:
#		name = "default"
#		image_b64 = result.text

	i=0
	for image in images:
		name=str(i)
		image_b64 = image.text
		print(image_b64)
		result=yangcli(yconns["display"],"""replace /displays/display[name='%s'] -- image=%s"""%(name, image_b64))
		if(len(result.xpath('./ok'))!=1):
			print(etree.tostring(result))
			assert(0)
		i=i+1
	tntapi.network_commit(conns)


