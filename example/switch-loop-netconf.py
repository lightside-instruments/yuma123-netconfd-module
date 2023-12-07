#!/usr/bin/python3

from lxml import etree
import time
import sys, os
import argparse
import tntapi
import lxml


def copy_config_and_commit(conn,rpc):
	ret = conn.send(rpc)
	if ret == False:
		print("[FAILED] Sending <copy-config>")
		sys.exit(-1)

	reply_xml=conns[node_name].receive()
#	print(lxml.etree.tostring(reply_xml))
	if reply_xml == None:
		print("[FAILED] Receiving <copy-config> reply")
		return(False)
	ret = conn.send('''<commit xmlns="urn:ietf:params:xml:ns:netconf:base:1.0"/>''')
	reply_xml=conns[node_name].receive()
	if reply_xml == None:
		print("[FAILED] Receiving <commit> reply")
		return(False)

#	tntapi.network_commit(conns)
	return(True)


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
	copy_config_and_commit(conns[node_name],rpc);

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
	copy_config_and_commit(conns[node_name],rpc);


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
	copy_config_and_commit(conns[node_name],rpc);

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
	copy_config_and_commit(conns[node_name],rpc);

	#time.sleep(0.5)

