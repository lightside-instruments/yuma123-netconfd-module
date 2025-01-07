#!/usr/bin/python

from lxml import etree
import time
import base64
import sys, os
import argparse
import subprocess


namespaces={"nc":"urn:ietf:params:xml:ns:netconf:base:1.0",
	"nd":"urn:ietf:params:xml:ns:yang:ietf-network",
	"nt":"urn:ietf:params:xml:ns:yang:ietf-network-topology",
        "netconf-node":"urn:tntapi:netconf-node",
        "scope":"urn:lsi:params:xml:ns:yang:ivi-scope"}

global args
args=None
parser = argparse.ArgumentParser()
parser.add_argument("--data", help="Path to the netconf network data *.xml file ietf-networks, ietf-networks-topology and netconf-node models e.g. get-net networks.xml data.xml")
args = parser.parse_args()

tree=etree.parse(args.data)
data = tree.xpath('/nc:config/nd:networks/nd:network/nd:node/netconf-node:data/scope:acquisition/scope:channels/scope:channel/scope:data', namespaces=namespaces)
print(len(data))

for mydata in data:
	scope_name = mydata.getparent().getparent().getparent().getparent().getparent().xpath("nd:node-id", namespaces=namespaces)[0].text 
	channel_name = mydata.getparent().xpath("scope:name", namespaces=namespaces)[0].text 

	data_b64 = data[0].text

	print("Writing signal_%s_%s.wav"%(scope_name,channel_name))
	f = open("signal_%s_%s.wav"%(scope_name,channel_name), "wb")
	f.write(base64.b64decode(data_b64))
	f.close()

