# Communication interface User's Manual https://cdn.tmi.yokogawa.com/IM701530-11E.pdf

import sys
import time
import os
import datetime

import vxi11
instr = vxi11.vxi11.Instrument("TCPIP::10.13.37.195::gpib,1::INSTR")
instr.write("*IDN?")

samples=2000000

print(instr.read())

con = instr

def cmd_binary(instr, cmd):
        instr.write(cmd)
        c = instr.read_raw(4*1024*1024)
        return c

def cmd(instr, cmd):
        result = ""
        instr.write(cmd)
        c = instr.read(4*1024*1024)
        result=c
        #result=c.decode("utf-8")
        return result

def setup_channel(channel):
        reply=cmd(con, ':CHANnel'+str(channel)+'?\n')
        print (reply)
        con.write(':CHAN'+str(channel)+':MODE ON\n')
        con.write(':CHAN'+str(channel)+':POS 0\n')
        con.write(':CHAN'+str(channel)+':PROBE 1\n')
        con.write('CHAN'+str(channel)+':VDIV:VALue 0.1V\n')

def disable_channel(channel):
        reply=cmd(con, ':CHANnel'+str(channel)+'?\n')
        print (reply)
        con.write(':CHAN'+str(channel)+':MODE OFF\n')

def read_waveform(trace, type="ASCII", vdiv=1) :
        con.write('WAVeform:TRACE '+ str(trace) + '\n')

        con.write('WAVeform:FORMAT %s\n'%(type))

        reply=cmd(con, "WAVeform:START?\n")
        print(reply)
        reply=cmd(con, "WAVeform:END?\n")
        print(reply)

        reply=cmd(con, "WAVeform:BITS?\n")
        print(reply)

        reply=cmd(con, "WAVeform:TYPE?\n")
        print(reply)

        reply=cmd(con, "WAVeform:SIGN?\n")
        print(reply)

        float_array = []
        block_size_max=10000

        for offset in range(0,samples,block_size_max):
            con.write('WAVeform:START ' + str(offset) + '\n')
            if((offset+block_size_max) > samples):
                block_size=samples-offset
            else:
                block_size=block_size_max
            con.write('WAVeform:END '+ str(offset+block_size-1) + '\n')

            reply=cmd(con, "WAVeform:START?\n")
            print(reply)
            reply=cmd(con, "WAVeform:END?\n")
            print(reply)


            reply=cmd(con, 'WAVeform?\n')
            print(reply)

            start = time.time()


            if(type=="ASCII"):
                reply=cmd(con, "WAVeform:SEND?\n")
            else:
                c=cmd_binary(con, "WAVeform:SEND?\n")

            end = time.time()

            if(type=="BYTE"):
                reply=""
                for byte in c[10:-1]:
                    val = float(vdiv)*(float(byte)-128)*1.0/25;
                    if(reply==""):
                        reply = str(val)
                    else:
                        reply = reply + "," + str(val)
            elif(type=="WORD"):
                reply=""
                for i in range(10,(len(c)-1),2):
                    num=int.from_bytes(c[i:i+2], byteorder='big', signed=True)

                    val = float(vdiv)*(num)*1.0/3200
                    if(reply==""):
                        reply = str(val);
                    else:
                        reply = reply + "," + str(val)

            if(type=="ASCII"):
                print("Read %d bytes in %lf seconds" %(len(reply), (end - start)))
                #print(reply)
            else:
                print("Read %d bytes in %lf seconds" %(len(c), (end - start)))
                #print(c)


            float_array.append([float(i) for i in reply.split(',')])

        print("signal%d=%s"%(trace,str(float_array)))

#con=gpib.dev(0,3)

reply = cmd(instr,"*IDN?")
print(reply)

con.write('*RST\n')
reply=cmd(con, '*IDN?\n')
print (reply)


# datetime object containing current date and time
now = datetime.datetime.now()
date_string = now.strftime("%Y/%m/%d")
time_string = now.strftime("%H:%M:%S")

print(date_string)
print(time_string)
con.write('SYSTem:CLOCk:DATE "%s"\n'%(date_string[2:]))
con.write('SYSTem:CLOCk:TIME "%s"\n'%(time_string))

con.write(':TIM:TDIV 1000ms\n')

reply2=cmd(con, "WAVeform:LENGth?\n")
print(reply2)

setup_channel(1)
disable_channel(2)
disable_channel(3)
disable_channel(4)


reply=cmd(con, 'ACQuire:RECordlength?\n')
print (reply)


reply=cmd(con, 'TRIGger?\n')
print (reply)

con.write('TRIGger:MODE SINGLE\n')
con.write('TRIGger:POSITION -3\n')
con.write('TRIGGER:SIMPLE:EDGE:SLOPE FALL\n')
con.write('TRIGger:SOURce:CHANnel1:LEVel -0.0 V\n')
con.write('TRIGger:SOURce:COUPling AC\n')

reply=cmd(con, 'TRIGger?\n')
print (reply)

reply=cmd(con, 'ACQuire:RECordlength?\n')
print (reply)

reply=cmd(con, "TIMebase:SRATe?\n")
print(reply)


con.write('ACQuire:RECordlength %d\n'%(samples))


#con.write('*WAI\n')
#print (reply)
#time.sleep(4)

reply=cmd(con, "TIMebase:SRATe?\n")
print(reply)

reply=cmd(con, 'ACQuire:RECordlength?\n')
print (reply)


reply=cmd(con, 'ACQuire:RECordlength?\n')
print (reply)

reply=cmd(con, "ACQuire?\n")
print(reply)

#con.write('WAVEFORM:DATASELECT ACQDATA\n')
#con.write('TRIGger:ACTion:STARt\n')

con.write('START\n')

print("Waiting for trigger ...")

while (1):
    reply=cmd(con, 'STATus:CONDition?')
    print(reply)
    if(reply.strip()=="0"):
        break
    time.sleep(1)

con.write(':STOP\n')
#con.write('TRIGger:ACTion:STOP\n')


reply=cmd(con, "ACQuire?\n")
print(reply)


reply=cmd(con, "WAVeform:LENGth?\n")
print(reply)

reply=cmd(con, "WAVeform:TRIGger?\n")
print(reply)


#os.system("cat /proc/interrupts > interrupts-before.txt")
#read_waveform(1, type="ASCII")
#os.system("cat /proc/interrupts > interrupts-after.txt")
#os.system("diff interrups-before.txt interrupts-after.txt")
read_waveform(1, type="BYTE", vdiv=0.1)
#read_waveform(1, type="WORD", vdiv=0.1)
#read_waveform(2, type="ASCII")
#read_waveform(2, type="BYTE")
#read_waveform(2, type="WORD")
#read_waveform(3)
#read_waveform(4)


sys.exit(0)

