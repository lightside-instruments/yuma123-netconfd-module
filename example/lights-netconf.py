#!/usr/bin/python

from lxml import etree
import time
import sys, os
import argparse
import tntapi
import yangrpc
from yangcli import yangcli

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


brightness = 0
while(brightness<256):
	for node_name in yconns.keys():
		# Set output voltage
		ok=yangcli(yconns[node_name],"""replace /lights/light[name='main'] -- red=%d green=%d blue=%d"""%(brightness, brightness, brightness)).xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
#	time.sleep(10/256)
	brightness = brightness + 1

