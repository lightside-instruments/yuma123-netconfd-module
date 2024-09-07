#!/usr/bin/python

from lxml import etree
import time
import base64
import sys, os
import argparse
import subprocess
import tntapi
import yangrpc
from yangcli import yangcli

def generate_data():
	#generate image.jpg.b64
	os.system("octave-cli generate-chirp.m")
	res = subprocess.check_output(["base64", "--wrap=0", "chirp.wav"])
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

filter="""
<filter xmlns="urn:ietf:params:xml:ns:netconf:notification:1.0" xmlns:netconf="urn:ietf:params:xml:ns:netconf:base:1.0" netconf:type="subtree">
 <acquisition-complete xmlns="urn:lsi:params:xml:ns:yang:ivi-scope"/>
</filter>"""

rpc_xml_str="""
<create-subscription xmlns="urn:ietf:params:xml:ns:netconf:notification:1.0">
%(filter)s
</create-subscription>
"""

print(rpc_xml_str%{'filter':filter})

result = conns['scope0'].rpc(rpc_xml_str%{'filter':filter})
print(etree.tostring(result))
rpc_error = result.xpath('rpc-error')
assert(len(rpc_error)==0)

yangcli(yconns["scope0"],"""delete /acquisition""")
tntapi.network_commit(conns)

ok=yangcli(yconns["scope0"],"""create /acquisition -- samples=480000 sample-rate=48000""").xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns["scope0"],"""create /acquisition/channels/channel[name='default'] -- range=3.47""").xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

notification_xml=conns["scope0"].receive()
if notification_xml == None:
    print("[FAILED] Receiving <acquisition-complete> notification")
    sys.exit(-1)

print(etree.tostring(notification_xml))

#notification_xml=litenc_lxml.strip_namespaces(notification_xml)
#match=notification_xml.xpath("/notification/netconf-session-end")
#assert(len(match)==1)

#print("[OK] Receiving <netconf-session-end>")

#time.sleep(2*480000/48000)

data=yangcli(yconns["scope0"],"""xget /acquisition/channels/channel[name='default']""") .xpath('./data/acquisition/channels/channel/data')
#print(len(data))
#print(etree.tostring(data[0]))
assert(len(data)==1)

data_b64 = data[0].text

f = open("signal.wav", "wb")
f.write(base64.b64decode(data_b64))
f.close()

ok=yangcli(yconns["scope0"],"""delete /acquisition""").xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

