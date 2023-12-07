#!/usr/bin/python3

from lxml import etree
import time
import sys, os
import argparse
import tntapi
import lxml


def copy_config_send(conn,rpc):
	ret = conn.send(rpc)
	if ret == False:
		print("[FAILED] Sending <copy-config>")
		sys.exit(-1)

def reply_recv(conn):
	reply_xml=conn.receive()
	return (reply_xml)

def commit_send(conn):
	ret = conn.send('''<commit xmlns="urn:ietf:params:xml:ns:netconf:base:1.0"/>''')

def copy_config_and_commit_network(conns, rpc):
	for node_name in conns.keys():
		copy_config_send(conns[node_name],rpc);

	for node_name in conns.keys():
		reply_recv(conns[node_name]);

	for node_name in conns.keys():
		commit_send(conns[node_name])

	for node_name in conns.keys():
		reply_recv(conns[node_name]);



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

for i in range(1,int(args.loops)):
	rpc='''
  <copy-config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
    <target>
      <candidate/>
    </target>

    <source>
    <config>
      <channels xmlns="urn:lsi:params:xml:ns:yang:ivi-switch">
        <channel>
          <name>c1</name>
        </channel>
        <channel>
          <name>a1</name>
          <connections>
            <connection>c1</connection>
          </connections>
        </channel>
      </channels>
    </config>
    </source>
  </copy-config>
'''

	copy_config_and_commit_network(conns, rpc)

	rpc='''
  <copy-config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
    <target>
      <candidate/>
    </target>

    <source>
    <config>
      <channels xmlns="urn:lsi:params:xml:ns:yang:ivi-switch">
        <channel>
          <name>c1</name>
        </channel>
        <channel>
          <name>a1</name>
          <connections>
            <connection>c1</connection>
          </connections>
        </channel>
        <channel>
          <name>c2</name>
        </channel>
        <channel>
          <name>a2</name>
          <connections>
            <connection>c2</connection>
          </connections>
        </channel>
      </channels>
    </config>
    </source>
  </copy-config>
'''
	copy_config_and_commit_network(conns,rpc);


	rpc='''
  <copy-config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
    <target>
      <candidate/>
    </target>

    <source>
    <config>
      <channels xmlns="urn:lsi:params:xml:ns:yang:ivi-switch">
        <channel>
          <name>c1</name>
        </channel>
        <channel>
          <name>a1</name>
          <connections>
            <connection>c1</connection>
          </connections>
        </channel>
        <channel>
          <name>c2</name>
        </channel>
        <channel>
          <name>a2</name>
          <connections>
            <connection>c2</connection>
          </connections>
        </channel>
        <channel>
          <name>c3</name>
        </channel>
        <channel>
          <name>a3</name>
          <connections>
            <connection>c3</connection>
          </connections>
        </channel>
      </channels>
    </config>
    </source>
  </copy-config>
'''
	copy_config_and_commit_network(conns,rpc);

		#time.sleep(0.5)

	rpc='''
  <copy-config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
    <target>
      <candidate/>
    </target>

    <source>
    <config>
    </config>
    </source>
  </copy-config>
'''
	copy_config_and_commit_network(conns,rpc);

	#time.sleep(0.5)

