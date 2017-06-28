#!/usr/bin/env python
#	logger.py

import lcm, sys, signal, time
from messages import *

silent=(len(sys.argv)>1) and (sys.argv[1]=='-s')
if not silent:
    print("Running %s"%(sys.argv[0]))

now=time.localtime()
flog=open("OUTPUT/Log%02d%02d_%02d%02d.txt"%(now.tm_mday,now.tm_mon, now.tm_hour,now.tm_min),'w')
t0=time.time()

def signal_handler(signal, frame):
    global flog, silent, now
    print ("\tLOGGER>>>>stopping logger") 
    flog.close()
    if not silent:
        print ("\tLOGGER>>>>See log data in OUTPUT/Log%02d%02d_%02d%02d.txt <<<<<<<"%(now.tm_mday,now.tm_mon, now.tm_hour,now.tm_min))
    sys.exit(0)

#LCM handlers for messages:
#CMD: fusion_cmd_t,georef_cmd_t,geotif_cmd_t, hotspot_cmd_t, jellyfish_cmd_t, mosaic_cmd_t, quality_cmd_t, resize_cmd_t, stitch_cmd_t, payload_cmd_t
def fusion_cmd_handler(channel, data):
    global flog, silent
    msg = fusion_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>FUSION %d: (%s+%s)" % (msg.command_id, msg.photo_file[0], msg.photo_file[1]))
    flog.write("FUSION %d: (%s+%s)\n" % (msg.command_id, msg.photo_file[0], msg.photo_file[1]))
    #geolocation_t photo_location[2];// [0] Visual, [1] Thermal
    #orientation_t photo_attitude[2]

def georef_cmd_handler(channel, data):
    global flog, silent
    msg = georef_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>GEOREF %d: (%s+%s)" %(msg.command_id, msg.pixel[0], msg.pixel[1])) 
    flog.write("GEOREF %d: (%s+%s)\n" %(msg.command_id, msg.pixel[0], msg.pixel[1])) 
    #geolocation_t location
    #orientation_t attitude

def geotif_cmd_handler(channel, data):
    global flog, silent
    msg = geotif_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>:GEOTIF %d: %d" % (msg.command_id, msg.photo_file))
    flog.write("GEOTIF %d: %d\n" % (msg.command_id, msg.photo_file))
    #geolocation_t location
    #orientation_t attitude

def hotspot_cmd_handler(channel, data):
    global flog, silent
    msg = hotspot_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>HOTSPOT %d: %s" % (msg.command_id, msg.photo_file))
    flog.write("HOTSPOT %d: %s\n" % (msg.command_id, msg.photo_file))

def jellyfish_cmd_handler(channel, data):
    global flog, silent
    msg = jellyfish_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>JELLYFISH %d: %s" % (msg.command_id, msg.photo_file))
    flog.write("JELLYFISH %d: %s\n" % (msg.command_id, msg.photo_file))

def mosaic_cmd_handler(channel, data):
    global flog, silent
    msg = mosaic_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>MOSAIC %d: (%s+3)" % (msg.command_id, msg.photo_file[0]))
    flog.write("MOSAIC %d: (%s+3)\n" % (msg.command_id, msg.photo_file[0]))
    #geolocation_t photo_location[4];
    #orientation_t photo_attitude[4]

def quality_cmd_handler(channel, data):
    global flog, silent
    msg = quality_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>QUALITY %d: %s" % (msg.command_id, msg.photo_file))
    flog.write("QUALITY %d: %s\n" % (msg.command_id, msg.photo_file))

def resize_cmd_handler(channel, data):
    global flog, silent
    msg = resize_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>RESIZE %d: %s to %d" % (msg.command_id, msg.photo_file, msg.mega_pixels))
    flog.write("RESIZE %d: %s to %d\n" % (msg.command_id, msg.photo_file, msg.mega_pixels))

def stitch_cmd_handler(channel, data):
    global flog, silent
    msg = stitch_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>STITCH %d: (%s+3)" % (msg.command_id, msg.photo_file[0]))
    flog.write("STITCH %d: (%s+3)\n" % (msg.command_id, msg.photo_file[0]))

def payload_cmd_handler(channel, data):
    global flog, t0, silent
    msg = payload_cmd_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>PAYLOAD %d: %s %s" % (msg.command_id, msg.camera_name, msg.operation))
    flog.write("PAYLOAD %d: %s %s\n" % (msg.command_id, msg.camera_name, msg.operation))

###RET: fusion_ret_t,georef_ret_t,geotif_ret_t, hotspot_ret_t, jellyfish_ret_t, mosaic_ret_t, quality_ret_t, resize_ret_t, stitch_ret_t, new_image_t
def fusion_ret_handler(channel, data):
    global flog, silent
    msg = fusion_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>FUSION ret %d: (%s)" % (msg.command_id, msg.photo_file))
    flog.write("FUSION ret %d: (%s)\n" % (msg.command_id, msg.photo_file))

def georef_ret_handler(channel, data):
    global flog, silent
    msg = georef_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>GEOREF ret %d: (%f,%f,%f)" %(msg.command_id, msg.pixel_loc.lat, msg.pixel_loc.lon, msg.pixel_loc.alt)) 
    flog.write("GEOREF ret %d: (%f,%f,%f)\n" %(msg.command_id, msg.pixel_loc.lat, msg.pixel_loc.lon, msg.pixel_loc.alt)) 
    #geolocation_t location
    #orientation_t attitude

def geotif_ret_handler(channel, data):
    global flog, silent
    msg = geotif_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>GEOTIF ret %d: %d" % (msg.command_id, msg.photo_file))
    flog.write("GEOTIF ret %d: %d\n" % (msg.command_id, msg.photo_file))
    #geolocation_t location
    #orientation_t attitude

def hotspot_ret_handler(channel, data):
    global flog, silent
    msg = hotspot_ret_t.decode(data)
    if not silent:
        print("LOGGER:HOTSPOT ret %d: error=%d, num hs=%d" % (msg.command_id, msg.result, msg.hotspot_count))
    flog.write("HOTSPOT ret %d: error=%d, num hs=%d\n" % (msg.command_id, msg.result, msg.hotspot_count))

def jellyfish_ret_handler(channel, data):
    global flog, silent
    msg = jellyfish_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>JELLYFISH ret %d: %d" % (msg.command_id, msg.jellyfish_count))
    flog.write("JELLYFISH ret %d: %d\n" % (msg.command_id, msg.jellyfish_count))

def mosaic_ret_handler(channel, data):
    global flog, silent
    msg = mosaic_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>MOSAIC ret %d: (%s+3)" % (msg.command_id, msg.photo_file))
    flog.write("MOSAIC ret %d: (%s+3)\n" % (msg.command_id, msg.photo_file))
    #geolocation_t photo_location[4];
    #orientation_t photo_attitude[4]

def quality_ret_handler(channel, data):
    global flog, silent
    msg = quality_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>QUALITY ret %d: %d (b=%f,e=%f)" % (msg.command_id, msg.result, msg.blur, msg.entropy))
    flog.write("QUALITY ret %d: %d (b=%f,e=%f)\n" % (msg.command_id, msg.result, msg.blur, msg.entropy))

def resize_ret_handler(channel, data):
    global flog, silent
    msg = resize_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>RESIZE ret %d: %s" % (msg.command_id, msg.photo_file))
    flog.write("RESIZE ret %d: %s\n" % (msg.command_id, msg.photo_file))

def stitch_ret_handler(channel, data):
    global flog, silent
    msg = stitch_ret_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>STITCH ret %d: (%s+3)" % (msg.command_id, msg.photo_file[0]))
    flog.write("STITCH ret %d: (%s+3)\n" % (msg.command_id, msg.photo_file[0]))

def new_image_handler(channel, data):
    global flog, t0, silent
    msg = new_image_t.decode(data)
    if not silent:
        print("\tLOGGER>>>>NEW_IMAGE %d: #%d %s %d sec" % (msg.command_id, msg.instance, msg.image_pathname, msg.n_sec-t0))
    flog.write("NEW_IMAGE %d: #%d %s %d sec\n" % (msg.command_id, msg.instance, msg.image_pathname, msg.n_sec-t0))

# main
signal.signal(signal.SIGINT, signal_handler)

lc=lcm.LCM()
#lc = lcm.LCM("udpm://239.255.76.67:7667?ttl=1")#udpm://224.0.0.1:7667?ttl=1")#

#LCM handlers for messages: fusion_cmd_t,georef_cmd_t,geotif_cmd_t, hotspot_cmd_t, jellyfish_cmd_t, mosaic_cmd_t, quality_cmd_t, resize_cmd_t, stitch_cmd_t, payload_cmd_t, fusion_ret_t,georef_ret_t,geotif_ret_t, hotspot_ret_t, jellyfish_ret_t, mosaic_ret_t, quality_ret_t, resize_ret_t, stitch_ret_t, new_image_t
subscription1 = lc.subscribe("fusion_cmd", fusion_cmd_handler)
subscription2 = lc.subscribe("georef_cmd", georef_cmd_handler)
subscription3 = lc.subscribe("geotif_cmd", geotif_cmd_handler)
subscription4 = lc.subscribe("hotspot_cmd", hotspot_cmd_handler)
subscription5 = lc.subscribe("jellyfish_cmd", jellyfish_cmd_handler)
subscription6 = lc.subscribe("mosaic_cmd", mosaic_cmd_handler)
subscription7 = lc.subscribe("quality_cmd", quality_cmd_handler)
subscription8 = lc.subscribe("resize_cmd", resize_cmd_handler)
subscription9 = lc.subscribe("stitch_cmd", stitch_cmd_handler)
subscription10 = lc.subscribe("payload_cmd", payload_cmd_handler)
subscription11 = lc.subscribe("fusion_ret", fusion_ret_handler)
subscription12 = lc.subscribe("georef_ret", georef_ret_handler)
subscription13 = lc.subscribe("geotif_ret", geotif_ret_handler)
subscription14 = lc.subscribe("hotspot_ret", hotspot_ret_handler)
subscription15 = lc.subscribe("jellyfish_ret", jellyfish_ret_handler)
subscription16 = lc.subscribe("mosaic_ret", mosaic_ret_handler)
subscription17 = lc.subscribe("quality_ret", quality_ret_handler)
subscription18 = lc.subscribe("resize_ret", resize_ret_handler)
subscription19 = lc.subscribe("stitch_ret", stitch_ret_handler)
subscription20 = lc.subscribe("new_image", new_image_handler)
#if not silent:
#    print("\tLOGGER>>>>LOG all")
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
lc.unsubscribe(subscription5)
lc.unsubscribe(subscription6)
lc.unsubscribe(subscription7)
lc.unsubscribe(subscription8)
lc.unsubscribe(subscription9)
lc.unsubscribe(subscription10)
lc.unsubscribe(subscription11)
lc.unsubscribe(subscription12)
lc.unsubscribe(subscription13)
lc.unsubscribe(subscription14)
lc.unsubscribe(subscription15)
lc.unsubscribe(subscription16)
lc.unsubscribe(subscription17)
lc.unsubscribe(subscription18)
lc.unsubscribe(subscription19)
lc.unsubscribe(subscription20)







