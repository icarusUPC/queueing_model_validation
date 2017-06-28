#!/usr/bin/env python
#	odroid_hotspots_mission.py: C=3 (HGQ + F x2t) => lambda = 1 im/s

import lcm, sys, signal, time, subprocess32, re, threading
from messages import *

if len(sys.argv)!=2:
    print ("Usage %s <Lambda>"%(sys.argv[0]))
    sys.exit(0)
L=float(sys.argv[1])
print("Running %s with lambda=%f" % (sys.argv[0], L))

#GLOBALS
TER_CAM='FLIR-A320'
VIS_CAM='JAI'
BLUR_THRESHOLD=2.0
ENTROPY_THRESHOLD=2.0
photo_name_vis={} #dictionary to store name of images
photo_name_ter={} #dictionary to store name of images
photo_position={} #dictionary to store the positions of the images
pending={}        #dictionary of pending VIS images for quality_cmd
fusion_on=False   #shared variable to sequence fusion jobs
lk_fusion=threading.Lock()

#END of simulation (^C)
def signal_handler(s, frame):
    print("\tMISION>>>>Stopping mission")
    sys.exit(0)
 
#LCM handlers for messages:
def new_image_handler(channel, data):
    global lc, TER_CAM, VIS_CAM, photo_name_ter, photo_name_vis, pending
    msg = new_image_t.decode(data)
    iname=msg.image_pathname
    instance=msg.instance
    pattern=re.compile(TER_CAM)
    if pattern.search(iname): # thermal image
      photo_name_ter[instance]=msg.image_pathname
      out_msg=hotspot_cmd_t()
      out_msg.command_id=instance
      out_msg.photo_file=iname
      lc.publish("hotspot_cmd", out_msg.encode())
      '''
	int64_t command_id;
	string photo_file;
      '''
    pattern=re.compile(VIS_CAM)
    if pattern.search(iname): # visual image
      photo_name_vis[instance]=msg.image_pathname
      if instance in pending: # in case thermal+hotspots is faster than visual
        out_msg=pending[instance]
        out_msg.photo_file=msg.image_pathname
        lc.publish("quality_cmd", out_msg.encode())

def hotspot_ret_handler(channel, data):
    global lc, TER_CAM, photo_name_ter, photo_name_vis, pending
    msg = hotspot_ret_t.decode(data)
    if msg.result!=0: # check for error
      return
    if msg.hotspot_count==0: # check for HS
      return
    print ("\tMISION>>>>Found %d hotspots in %s"%(msg.hotspot_count, photo_name_ter[msg.command_id]))
    larger=0
    max_pixels=-1
    for hs in msg.hotspot_list:
      if hs.num_pixels>max_pixels:
        max_pixels=hs.num_pixels
        larger=hs
    #calling georef for the larger hotspot in the list
    out_msg=georef_cmd_t()
    out_msg.command_id=msg.command_id
    out_msg.pixel=hs.center
    out_msg.location=photo_position[msg.command_id][0]
    out_msg.attitude=photo_position[msg.command_id][1]
    lc.publish("georef_cmd", out_msg.encode())
    '''
	int64_t command_id;
	int32_t pixel[2];
	geolocation_t location;
	orientation_t attitude;
    '''
    #calling quality
    out_msg=quality_cmd_t()
    out_msg.command_id=msg.command_id
    try:
      out_msg.photo_file=photo_name_vis[msg.command_id]
      lc.publish("quality_cmd", out_msg.encode())
      '''
	int64_t command_id;
	string photo_file;
      '''
    except KeyError:
      pending[msg.command_id]=out_msg

def georef_ret_handler(channel, data):
    global photo_name_ter
    msg = georef_ret_t.decode(data)
    if msg.result!=0: # check for error
      return
    msg.pixel_loc
    print ("\tMISION>>>>Geolocation of the larger hotspot in %s is [%f, %f, %f]"%(photo_name_ter[msg.command_id], msg.pixel_loc.lat, msg.pixel_loc.lon, msg.pixel_loc.alt))
    '''
	int64_t command_id;
	byte result;
	geolocation_t pixel_loc;
   '''

def quality_ret_handler(channel, data):
    global lc, photo_name_ter, photo_name_vis, photo_position, BLUR_THRESHOLD, ENTROPY_THRESHOLD, fusion_on
    msg = quality_ret_t.decode(data)
    '''
	int64_t command_id;
	byte result;
	float entropy;
	float blur;
    '''
    if msg.result!=0: # check for error
      return
    if msg.entropy < ENTROPY_THRESHOLD and msg.blur < BLUR_THRESHOLD: # check for quality
      return
    lk_fusion.acquire()  # force sequenced execution for fusions
    if fusion_on:
      lk_fusion.release()
      print("\tMISION>>>>fusion service still busy... loosing image")
      return      
    fusion_on=True
    lk_fusion.release()
    out_msg=fusion_cmd_t()
    out_msg.command_id=msg.command_id
    out_msg.photo_file[0]=photo_name_vis[msg.command_id]
    out_msg.photo_file[1]=photo_name_ter[msg.command_id]
    out_msg.photo_location[0]=photo_position[msg.command_id][0]
    out_msg.photo_location[1]=photo_position[msg.command_id][0]
    out_msg.photo_attitude[0]=photo_position[msg.command_id][1]
    out_msg.photo_attitude[1]=photo_position[msg.command_id][1]
    lc.publish("fusion_cmd", out_msg.encode())
    '''
	int64_t command_id;
	string photo_file[2];		// [0] Visual, [1] Thermal
	geolocation_t photo_location[2];// [0] Visual, [1] Thermal
	orientation_t photo_attitude[2];// [0] Visual, [1] Thermal
    '''

def fusion_ret_handler(channel, data):
    global fusion_on
    msg = fusion_ret_t.decode(data)     
    fusion_on=False
    '''
	int64_t command_id;
	byte result;
	string photo_file;
    '''
    if msg.result!=0: # check for error
      return
    print ("\tMISION>>>>Fusion results in %s"%(msg.photo_file))    
    

### MAIN

#Preparing the telemetry
with open("cfg_files/PhotoLog.txt","r") as f:
  for line in f:
    w = line.strip().split()
    pos=geolocation_t()
    pos.lat=float(w[3])
    pos.lon=float(w[4])
    pos.alt=float(w[5])
    att=orientation_t()
    att.yaw=float(w[6])
    att.pitch=float(w[7])
    att.roll=float(w[8])
    photo_position[int(w[0])]=(pos,att) #indexed by number of image instance

#Start SERVICES:
print("\tMISION>>>>Starting services...") 
logger    = subprocess32.Popen(["/usr/bin/python","logger.py"])
camera_vis= subprocess32.Popen(["/usr/bin/python","fake_camera_service.py",VIS_CAM, str(1.0/L), "0"])
camera_ter= subprocess32.Popen(["/usr/bin/python","fake_camera_service.py",TER_CAM, str(1.0/L), "0"])
hotspot   = subprocess32.Popen(["rtdp_lcm_services/hotspot_service", "--num_threads", "1"])
georef    = subprocess32.Popen(["rtdp_lcm_services/georef_service", "-c", TER_CAM, "--num_threads", "1"])
quality   = subprocess32.Popen(["rtdp_lcm_services/quality_service", "--num_threads", "1"])
fusion    = subprocess32.Popen(["rtdp_lcm_services/fusion_service", "-vc", VIS_CAM, "-ic", TER_CAM, "--num_threads", "2" ])

#Subscribe to events
print("\tMISION>>>>Subscribing to LCM ret messages...") 
signal.signal(signal.SIGINT, signal_handler)
lc=lcm.LCM() #lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")
subscription_im = lc.subscribe("new_image", new_image_handler)
subscription_hs = lc.subscribe("hotspot_ret", hotspot_ret_handler)
subscription_gr = lc.subscribe("georef_ret", georef_ret_handler)
subscription_qu = lc.subscribe("quality_ret", quality_ret_handler)
subscription_fs = lc.subscribe("fusion_ret", fusion_ret_handler)

#Switch ON the cameras
print("\tMISION>>>>Switching cameras ON...") 
time.sleep(1)
msg=payload_cmd_t()
msg.command_id=0
msg.operation='ON'
msg.camera_name=VIS_CAM
lc.publish("payload_cmd", msg.encode())
msg.camera_name=TER_CAM
lc.publish("payload_cmd", msg.encode())

#wait for events
print("\tMISION>>>>Main loop...") 
while True:
    try: 
        lc.handle()
    except KeyboardInterrupt:
        break
    except IOError:
        pass

print("\tMISION>>>>Cleaning up...")
lc.unsubscribe(subscription_im)
lc.unsubscribe(subscription_hs)
lc.unsubscribe(subscription_gr)
lc.unsubscribe(subscription_qu)
lc.unsubscribe(subscription_fs)



