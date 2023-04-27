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

for node_name in yconns.keys():
	ok=yangcli(yconns[node_name],"""delete /outputs""").xpath('./ok')
tntapi.network_commit(conns)

print("# Voltage Current\n\n")

# Step from 0.6 to 0.8 volt in 0.02 step
voltage = 0.6
while(voltage<0.8001):
	voltage = voltage + 0.02
	for node_name in yconns.keys():
		# Set output voltage
		ok=yangcli(yconns[node_name],"""replace /outputs/output[name='out1'] -- voltage-level=%.9f current-limit=%.9f"""%(voltage, 0.5)).xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)

	# Measure output current
	state = tntapi.network_get_state(network, conns, filter="""<filter type="xpath" select="/*[local-name()='outputs' or local-name()='outputs-state']/output"/>""")
	state_wo_ns=tntapi.strip_namespaces(state)
	for node_name in yconns.keys():
		current = state_wo_ns.xpath("node[node-id='%s']/data/outputs-state/output[name='%s']/measurement/current"%(node_name, "out1"))[0].text
		print("# %.3f %6.4f\n"%(voltage, float(current)))

for node_name in yconns.keys():
	ok=yangcli(yconns[node_name],"""delete /outputs""").xpath('./ok')
tntapi.network_commit(conns)
