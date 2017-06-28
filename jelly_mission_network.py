#!/usr/bin/env python
#	jelly_mission.py: Image => Quality => Jellyfish => Resize

import lcm, sys, signal, time, subprocess32
from messages import *

'''
if len(sys.argv)==1:
    print ("Usage %s <Camera>"%(sys.argv[0]))
    sys.exit(0)
'''
argv1="JAI"
print("Running %s %s"%(sys.argv[0], argv1))

#GLOBALS
t0=time.time()
logger=0
photo_name={} #dictionary to store name of images waiting for return values. Key=opn+cmd

#END of simulation (^C): save LOG
def signal_handler(signal, frame):
    logger.send_signal(signal.SIGTERM)

#LCM handlers for messages:
def jellyfish_ret_handler(channel, data):
    global t0
    msg = jellyfish_ret_t.decode(data)
    #print("JELLYFISH %d: %s)\n" % (msg.command_id, msg.photo_file))
    msg2 =resize_cmd_t()
    msg2.command_id=msg.command_id
    key="jellyfish_%d"%msg.command_id
    msg2.photo_file=photo_name[key]
    msg2.mega_pixels=1
    lc.publish("resize_cmd",msg2.encode())   

def quality_ret_handler(channel, data):
    global t0, photo_name
    msg = quality_ret_t.decode(data)
    #print("QUALITY %d\n" % (msg.command_id))
    msg2 =jellyfish_cmd_t()
    msg2.command_id=msg.command_id
    key="quality_%d"%msg.command_id
    msg2.photo_file=photo_name[key]
    if msg.blur>1 or msg.entropy<1:
        print ("Low quality Image %s. Discarted"%msg2.photo_file) 
    else:
        photo_name["jellyfish_%d"%msg.command_id]=msg2.photo_file
        lc.publish("jellyfish_cmd",msg2.encode())

def resize_ret_handler(channel, data):
    global t0
    msg = resize_ret_t.decode(data)
    print("RESIZE %d: %s\n" % (msg.command_id, msg.photo_file))  

def new_image_handler(channel, data):
    global t0
    msg = new_image_t.decode(data)
    print("NEW_IMAGE %d_#%d %s %.1f sec\n" % (msg.command_id, msg.instance, msg.image_pathname, msg.n_sec-t0))
    key="quality_%d"%msg.instance
    photo_name[key]=msg.image_pathname
    msg2 =quality_cmd_t()
    msg2.command_id=msg.instance
    msg2.photo_file=msg.image_pathname
    lc.publish("quality_cmd",msg2.encode())

# main
signal.signal(signal.SIGINT, signal_handler)
lc=lcm.LCM()
#lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")#udpm://224.0.0.1:7667?ttl=1")#

#LCM handlers for flow: new_image, quality_ret, jellyfish_ret, resize_ret
subscription1 = lc.subscribe("new_image", new_image_handler)
subscription2 = lc.subscribe("quality_ret", quality_ret_handler)
subscription3 = lc.subscribe("jellyfish_ret", jellyfish_ret_handler)
subscription4 = lc.subscribe("resize_ret", resize_ret_handler)

#Start logger+cameras+services
logger=subprocess32.Popen(["/usr/bin/python","logger.py"])
camera=subprocess32.Popen(["/usr/bin/python","fake_camera_service.py","JAI", "5", "0"])
quality=subprocess32.Popen(["rtdp_services/quality_service"])
jellyfish=subprocess32.Popen(["rtdp_services/jellyfish_service", "-o", "OUTPUT/JELLYFISH","-n", "1"])
resize=subprocess32.Popen(["rtdp_services/resize_service"])

#Switch ON the camera
time.sleep(1)
msg=payload_cmd_t()
msg.command_id=0
msg.operation='ON'
msg.camera_name=argv1
lc.publish("payload_cmd", msg.encode())

try:
    while True:
        lc.handle()
except KeyboardInterrupt:
    pass
except IOError:
    pass

lc.unsubscribe(subscription1)
lc.unsubscribe(subscription2)
lc.unsubscribe(subscription3)
lc.unsubscribe(subscription4)

logger.terminate()
camera.terminate()
quality.terminate()
jellyfish.terminate()
resize.terminate()


