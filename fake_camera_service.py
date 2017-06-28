#!/usr/bin/env python
#	Simulates a camera


import lcm, sys, time, os, random, threading, signal
from messages import *

#sys.argv=["fakeCameraService", "JAI", 1, .1]
if len(sys.argv)!=4:
    print ("Usage %s <Camera> <mean-period> <stddev>"%(sys.argv[0]))
    sys.exit(0)
print("Running %s %s (mean=%s stddev=%s) ... "%(sys.argv[0], sys.argv[1], sys.argv[2], sys.argv[3]))

def signal_handler(signal, frame):
    global on
    on=False
    print ("\tCAMERA>>>>Stopping camera service")
    sys.exit(0)

# VAR GLOBALS
on=False
camera=sys.argv[1]
freq=float(sys.argv[2])
dev_freq=float(sys.argv[3])
instance=0
c_id=-1


def do_take_pictures():
    global camera, freq, dev_freq, lc, on, c_id, instance
    for image in sorted(os.listdir("INPUT/%s/"%camera)):
        if image.endswith('.jpg'):
            msg=new_image_t()
            msg.image_pathname="INPUT/%s/%s"%(camera,image)
            msg.command_id=c_id
            msg.instance=instance
            instance+=1
            time.sleep(random.gauss(freq, dev_freq))
            if not on:
                return	#should be a WAIT on a semaphor!!
            msg.n_sec=time.time()
            lc.publish("new_image", msg.encode())


def p_handler(channel, data):
    global camera, do, on, c_id
    msg = payload_cmd_t.decode(data)
    if msg.camera_name!=camera:
        return
    if msg.operation=='ON':
        print("\tCAMERA>>>>Starting %s !"%camera)
        do=threading.Thread(name=camera, target=do_take_pictures)
        on=True
        c_id=msg.command_id
        do.start()
    else:
        print("\tCAMERA>>>>Stopping %s !"%camera)
        on=False
        do.join()

# MAIN program
signal.signal(signal.SIGINT, signal_handler)
lc=lcm.LCM() #lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")#udpm://224.0.0.1:7667?ttl=1")#

random.seed(0)
psubscription = lc.subscribe("payload_cmd", p_handler)
try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass
except IOError:
    pass
lc.unsubscribe(psubscription)

