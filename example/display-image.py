#!/usr/bin/python

# display a 1920x1080 JPG image
python3 display-image.py --networks=networks.xml --image=/tmp/image.jpg

from lxml import etree
import time
import sys, os
import argparse
import subprocess
import tntapi
import yangrpc
from yangcli import yangcli

def generate_image(image_path):
	res = subprocess.check_output(["base64", "--wrap=0", image_path])
	return res



namespaces={"nc":"urn:ietf:params:xml:ns:netconf:base:1.0",
	"nd":"urn:ietf:params:xml:ns:yang:ietf-network",
	"nt":"urn:ietf:params:xml:ns:yang:ietf-network-topology"}

global args
args=None

parser = argparse.ArgumentParser()
parser.add_argument("--config", help="Path to the netconf configuration *.xml file defining the configuration according to ietf-networks, ietf-networks-topology and netconf-node models e.g. ../networks.xml")
parser.add_argument("--image", help="Filename of the image to be displayed e.g. ../image-1920x1080.jpg")
args = parser.parse_args()

tree=etree.parse(args.config)
network = tree.xpath('/nc:config/nd:networks/nd:network', namespaces=namespaces)[0]

conns = tntapi.network_connect(network)
yconns = tntapi.network_connect_yangrpc(network)

yangcli(yconns["display"],"""delete /displays""")
tntapi.network_commit(conns)

image_b64 = generate_image(args.image)

ok=yangcli(yconns["display"],"""create /displays/display[name='default'] -- image=%s"""%(image_b64.decode('ascii'))).xpath('./ok')
assert(len(ok)==1)

tntapi.network_commit(conns)

