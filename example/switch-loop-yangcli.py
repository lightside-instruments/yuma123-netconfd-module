#!/usr/bin/python3

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
parser.add_argument('--loops', default=[],help="Loop count.")
args = parser.parse_args()

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network)
yconns = tntapi.network_connect_yangrpc(network)

for node_name in yconns.keys():
	ok=yangcli(yconns[node_name],"""delete /channels""").xpath('./ok')
	tntapi.network_commit(conns)

for i in range(1,int(args.loops)):
	for node_name in yconns.keys():
		ok=yangcli(yconns[node_name],"""delete /channels""").xpath('./ok')
		#assert(len(ok)==1)
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='a1']/connections -- connection='c1'""").xpath('./ok')
		assert(len(ok)==1)
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='c1']""").xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
	for node_name in yconns.keys():
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='a2']/connections -- connection='c2'""").xpath('./ok')
		assert(len(ok)==1)
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='c2']""").xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
	for node_name in yconns.keys():
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='a3']/connections -- connection='c3'""").xpath('./ok')
		assert(len(ok)==1)
		ok=yangcli(yconns[node_name],"""create /channels/channel[name='c3']""").xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
#	time.sleep(0.5)

	for node_name in yconns.keys():
		ok=yangcli(yconns[node_name],"""delete /channels""").xpath('./ok')
		assert(len(ok)==1)
	tntapi.network_commit(conns)
#	time.sleep(0.5)

