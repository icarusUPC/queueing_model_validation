#!/usr/bin/env python
#	V0: HMI to set up a mission
# ONLY WORKS THE CAMERA start/stop

import lcm, sys, subprocess32, signal, time
from messages import *

print("Running %s"%(sys.argv[0]))


#MAIN
lc = lcm.LCM() #"udpm://224.0.0.1:7667?ttl=1")
active_ser={} #pid del proceso servidor
loger=subprocess32.Popen(["/usr/bin/python","logger.py", "-s"])

print("Select cameras to start \n\tJAI\n\tFLIR-A320\n\tJELLY5M\n\tJELLY12M\n\t(0 to stop mission): "),
cam=sys.stdin.readline()[:-1]
while cam!='0':
    if cam in active_ser:
        print("stop to be done...")
    else:
        active_ser[cam]=subprocess32.Popen(["/usr/bin/python","fake_camera_service.py",cam, "2", ".5"])
        time.sleep(1)
        msg=payload_cmd_t()
        msg.camera_name=cam
        msg.operation='ON'
        print("--%s ON--"%cam)
        lc.publish("payload_cmd", msg.encode())
    print("Select cameras to start/stop \n\tJAI\n\tFLIR-A320\n\tJELLY5M\n\tJELLY12M\n\t(0 to stop mission): "),
    cam=sys.stdin.readline()[:-1]

def activate_server(s):
    global active_ser,lc
    active_ser[s]=subprocess32.Popen(["./rtdp_services/"+s+"_service"],stdout=open('/dev/null','w'))
    

options={'f':'fusion', 'g':'georef','t':'geotif','h':'thotspot',
         'j':'jellyfish','m':'mosaic','q':'quality','r':'resize','s':'stitch'}

while True:
    print("Select services to start. Options:")
    for o in options:
        print("\t(%s)\t%s"%(o,options[o]))
    print("\t(0) to end. Enter your option: "),
    op=sys.stdin.readline()[:-1]
    if op=='0':
        break
    try:
        cam=options[op]
    except KeyError:
        print("Incorrect option %s. Try again"%op)
        continue
    if cam in active_ser:
        print("stop to be done...")
    else:
        activate_server(cam)

for s in active_ser:
    active_ser[s].terminate()
    active_ser[s].wait()

loger.send_signal(signal.SIGTERM)
loger.wait()
