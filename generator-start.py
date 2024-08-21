# HEWLETT-PACKARD,33120A,0,8.0-5.0-1.0

import sys
import time
import os
import datetime

import vxi11
instr = vxi11.vxi11.Instrument("TCPIP::10.13.37.195::gpib,4::INSTR")
instr.write("*IDN?")

print(instr.read())

con = instr
con.write("APPL:SIN 1612.41 HZ, 0.200 VPP, 0 V")

#con.write("FUNCtion:USER arb0")
#con.write("DATA VOLATILE, 1, .67, .33, 0, -.33, -.67, -1")
sys.exit(0)


def cmd_binary(instr, cmd):
        instr.write(cmd)
        c = instr.read(1024*1024)
        return c

def cmd(instr, cmd):
        result = ""
        instr.write(cmd)
        c = instr.read(1024*1024)
        #result=c.decode("utf-8")
        return result

def setup_channel(channel):
        reply=cmd(con, ':CHANnel'+str(channel)+'?\n')
        print (reply)
        con.write(':CHAN'+str(channel)+':MODE ON\n')
        con.write(':CHAN'+str(channel)+':POS 0\n')
        con.write(':CHAN'+str(channel)+':PROBE 1\n')
        con.write('CHAN'+str(channel)+':VDIV:VALue 1V\n')

