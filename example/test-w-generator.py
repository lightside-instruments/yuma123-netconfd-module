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
	os.system("octave-cli generate_chirp.m")
	res = subprocess.check_output(["base64", "--wrap=0", "signal-out.wav"])
	return res


namespaces={"nc":"urn:ietf:params:xml:ns:netconf:base:1.0",
	"nd":"urn:ietf:params:xml:ns:yang:ietf-network",
	"nt":"urn:ietf:params:xml:ns:yang:ietf-network-topology"}

global args
args=None

parser = argparse.ArgumentParser()
parser.add_argument("--config", help="Path to the netconf configuration *.xml file defining the configuration according to ietf-networks, ietf-networks-topology and netconf-node models e.g. ../networks.xml")
parser.add_argument("--generator-name", help="Name of generator node e.g. 'generator0'")
parser.add_argument("--scope-name", help="Name of scope node e.g. 'scope0'")
parser.add_argument("--generator-channel-name", help="Name of generator channel e.g. 'default' or 'hw:1,0'")
parser.add_argument("--generator-channel-sample-rate", help="Sample rate of generator channel e.g. 48000.0")
parser.add_argument("--scope-channel-name", help="Name of sope channel e.g. 'default' or 'hw:1,0'")
parser.add_argument("--scope-channel-parameters", help="Scope channel parameters e.g. '-c 2 -f S16LE'")
parser.add_argument("--scope-channel-range", help="Scope channel range e.g. '4.0'")
parser.add_argument("--samples", help="Scope acquisition total samples e.g. 480000")
parser.add_argument("--sample-rate", help="Sample rate for acquisition e.g. 48000")
parser.add_argument("--scope-trigger-level", help="Scope trigger level in volts e.g. '1.0'")
parser.add_argument("--scope-trigger-source", help="Scope trigger source e.g. 'ch1'")
parser.add_argument("--scope-trigger-slope", help="Scope trigger slope e.g. 'positive' or 'negative'")
args = parser.parse_args()

scope_channel_name=args.scope_channel_name
scope_channel_range=float(args.scope_channel_range)
scope_channel_parameters=args.scope_channel_parameters
generator_channel_name=args.generator_channel_name
generator_channel_sample_rate=float(args.generator_channel_sample_rate)
samples = int(args.samples)
sample_rate = int(args.sample_rate)
scope_trigger_source=args.scope_trigger_source
scope_trigger_level=float(args.scope_trigger_level)
scope_trigger_slope=args.scope_trigger_slope

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network, timeout=10000)
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

result = conns[args.scope_name].rpc(rpc_xml_str%{'filter':filter})
print(etree.tostring(result))
rpc_error = result.xpath('rpc-error')
assert(len(rpc_error)==0)


yangcli(yconns[args.scope_name],"""delete /acquisition""")
yangcli(yconns[args.generator_name],"""delete /channels""")
tntapi.network_commit(conns)

data_b64 = generate_data()
print("""data=%s"""%(data_b64.decode('ascii')))

ok=yangcli(yconns[args.scope_name],"""create /acquisition -- samples=%d sample-rate=%d"""%(samples, sample_rate)).xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns[args.scope_name],"""merge /acquisition/trigger -- source=%s level=%f slope=%s"""%(scope_trigger_source, scope_trigger_level, scope_trigger_slope)).xpath('./ok')
assert(len(ok)==1)

ok=yangcli(yconns[args.scope_name],"""merge /acquisition/channels/channel[name='%s'] -- range=%f parameters='%s'"""%(scope_channel_name, scope_channel_range, scope_channel_parameters)).xpath('./ok')
assert(len(ok)==1)


tntapi.network_commit(conns)


# ok=yangcli(yconns[args.generator_name],"""create /channels/channel[name='%s'] -- data=%s"""%(generator_channel_name, data_b64.decode('ascii'))).xpath('./ok')
ok=yangcli(yconns[args.generator_name],"""create /channels/channel[name='%s']/arbitrary-waveform -- sample-rate=%f"""%(generator_channel_name, generator_channel_sample_rate)).xpath('./ok')
assert(len(ok)==1)

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
</edit-config>"""%(generator_channel_name, data_b64.decode('ascii'))

result = conns[args.generator_name].rpc(edit_config_rpc)
print(etree.tostring(result))
rpc_error = result.xpath('rpc-error')
assert(len(rpc_error)==0)

tntapi.network_commit(conns)

print("waiting for acquisition to complete ...")
time.sleep(10)

while(1):
    (notification_xml,ret)=conns[args.scope_name].receive()
    if(ret!=1): #timeout
        break;
    printf("Timeout. Retrying.")
if notification_xml == None:
    print("[FAILED] Receiving <acquisition-complete> notification")
    sys.exit(-1)

print(etree.tostring(notification_xml))
print("Acquisition complete.")

result=yangcli(yconns[args.scope_name],"""xget /acquisition/channels/channel[name='%s']"""%(scope_channel_name))

print(etree.tostring(result))
data=result.xpath('./data/acquisition/channels/channel/data')
#print(len(data))
#print(etree.tostring(data[0]))
print(len(data))
assert(len(data)==1)

data_b64 = data[0].text

f = open("signal.wav", "wb")
f.write(base64.b64decode(data_b64))
f.close()

ok=yangcli(yconns[args.scope_name],"""delete /acquisition""").xpath('./ok')
assert(len(ok)==1)
ok=yangcli(yconns[args.generator_name],"""delete /channels""").xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

