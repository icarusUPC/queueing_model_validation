#!/usr/bin/env python
#	odroid_jelly_mission.py: mu=7.8s  C=4 => lambda < 0.5128 im/s
#	using LCM for simulation (host), but remote jelly.exe (oDroid)

import lcm, sys, signal, time, subprocess32
from messages import *

if len(sys.argv)!=4:
    print ("Usage %s <#Cores> <#Threads> <Lambda>"%(sys.argv[0]))
    sys.exit(0)

#GLOBALS
C=int(sys.argv[1])
T=sys.argv[2]
L=float(sys.argv[3])
print("Running %s %d %s %f" % (sys.argv[0], C, T, L))
t0=time.time()
logger=0
camera=0
jellyfish=0
photo_name={} #dictionary to store name of images waiting for return values. Key=opn+cmd

#END of simulation (^C): save LOG
def signal_handler(s, frame):
    global logger, camera, jellyfish
    print(">>>> stopping mission: Killing logger, fake_camera and Jellyfish (x n)")
    logger.send_signal(signal.SIGINT)
    camera.send_signal(signal.SIGINT)
    for p in photo_name:
        p.send_signal(signal.SIGINT)
    sys.exit(0)

#END of child (JELLYFISH): notify it
def wait_child(s, frame):
    global photo_name

        
 
#LCM handlers for messages:
def new_image_handler(channel, data):
    global t0
    msg = new_image_t.decode(data)
    print("\tNEW_IMAGE %d_#%d %s %.1f sec\n" % (msg.command_id, msg.instance, msg.image_pathname, msg.n_sec-t0))
    #./bin/jellyfish ../../DATASET/INPUT/JELLY5M/000_5M.jpg -r -o ../../DATASET/OUTPUT/JELLY5M --num_threads 1
    key=subprocess32.Popen(["rtdp_core_exes/jellyfish/bin/jellyfish", msg.image_pathname, "-r", "-o", "OUTPUT/JELLYFISH","--num_threads", T])
    photo_name[key]=msg.image_pathname


# MAIN
signal.signal(signal.SIGINT, signal_handler)
#signal.signal(signal.SIGCHLD, wait_child)

lc=lcm.LCM()
#lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")#udpm://224.0.0.1:7667?ttl=1")#

#LCM handlers for flow: new_image, quality_ret, jellyfish_ret, resize_ret
subscription1 = lc.subscribe("new_image", new_image_handler)

#Start logger+cameras+services
logger=subprocess32.Popen(["/usr/bin/python","logger.py"])
camera=subprocess32.Popen(["/usr/bin/python","fake_camera_service.py","JAI", str(1.0/L), "0"])

#Switch ON the camera
time.sleep(1)
msg=payload_cmd_t()
msg.command_id=0
msg.operation='ON'
msg.camera_name="JAI"
lc.publish("payload_cmd", msg.encode())

while True:
    try: 
        lc.handle()
    except KeyboardInterrupt:
        break
    except IOError:
        pass
    #check for processes finished
    delete_list=[]
    for p in photo_name:
      if not p.poll() is None:
        print("\tJELLYFISH ended for %s\n" % (photo_name[p]))
        delete_list.append(p)
    for p in delete_list:
      del photo_name[p]

lc.unsubscribe(subscription1)




