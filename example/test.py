#!/usr/bin/python

from lxml import etree
import time
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

yangcli(yconns["generator0"],"""delete /channels""")
tntapi.network_commit(conns)

data_b64 = generate_data()
print("""data=%s"""%(data_b64.decode('ascii')))
#ok=yangcli(yconns["generator0"],"""create /channels/channel[name='default']/arbitrary-waveform -- data=%s"""%(data_b64.decode('ascii'))).xpath('./ok')
#assert(len(ok)==1)
edit_config_rpc = """<edit-config>
   <target>
     <candidate/>
   </target>
   <default-operation>merge</default-operation>
   <test-option>set</test-option>
<config xmlns="urn:ietf:params:xml:ns:netconf:base:1.0">
  <channels xmlns="urn:lsi:params:xml:ns:yang:ivi-function-generator">
    <channel>
      <name>%s</name>
      <arbitrary-waveform>
        <data>%s</data>
      </arbitrary-waveform>
    </channel>
  </channels>
</config>
</edit-config>"""%('default', data_b64.decode('ascii'))
result = conns['generator0'].rpc(edit_config_rpc)
print(etree.tostring(result))
rpc_error = result.xpath('rpc-error')
assert(len(rpc_error)==0)


print("committing")
tntapi.network_commit(conns)

print("waiting 10")

time.sleep(10)

print("deleting")

ok=yangcli(yconns["generator0"],"""delete /channels""").xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

