#!/usr/bin/python

from lxml import etree
import time
import sys, os
import argparse
import tntapi
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


time.sleep(2)

for node_name in yconns.keys():
	# Home all axes
	ok=yangcli(yconns[node_name],"""replace /gcode -- command='G28 XYZ' """).xpath('./ok')
	assert(len(ok)==1)
tntapi.network_commit(conns)
for node_name in yconns.keys():
	# Home all axes
	ok=yangcli(yconns[node_name],"""replace /gcode -- command='M400' """).xpath('./ok')
	assert(len(ok)==1)
tntapi.network_commit(conns)



for i in range(1,100):
	for node_name in yconns.keys():
		# Move to A
		ok=yangcli(yconns[node_name],"""replace /gcode -- command='G0 X100Y100Z100 F600000' """).xpath('./ok')
		print(len(ok))
		assert(len(ok)==1)
	tntapi.network_commit(conns)
	for node_name in yconns.keys():
		# Wait for movement to complete
		ok=yangcli(yconns[node_name],"""replace /gcode -- command='M400' """).xpath('./ok')
		print(len(ok))
		assert(len(ok)==1)
	tntapi.network_commit(conns)
	for node_name in yconns.keys():
		# Move to B
		ok=yangcli(yconns[node_name],"""replace /gcode -- command='G0 X0Y0Z100 F600000' """).xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
	for node_name in yconns.keys():
		# Wait for movement to complete
		ok=yangcli(yconns[node_name],"""replace /gcode -- command='M400' """).xpath('./ok')
		print(len(ok))
		assert(len(ok)==1)
	tntapi.network_commit(conns)


