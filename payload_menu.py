#!/usr/bin/env python
#	V0: HMI to start/stop a fake_camera_service

import lcm, sys
from messages import payload_t

print("Running %s"%(sys.argv[0]))


#MAIN
lc = lcm.LCM() #"udpm://224.0.0.1:7667?ttl=1")
active_cam={}
msg=payload_t()
while True:
   print("\tJAI\n\tFLIR-A320\n\tJELLY5M\n\tJELLY12M\nSelect a camera to start/stop: "),
   cam=sys.stdin.readline()[:-1]
   msg.camera_name=cam
   if cam in active_cam:
       del active_cam[cam]
       msg.operation='OFF'
       print("--%s OFF--"%cam)
       lc.publish("PAYLOAD_OP", msg.encode())
   else:
       active_cam[cam]=1
       msg.operation='ON'
       print("--%s ON--"%cam)
       lc.publish("PAYLOAD_OP", msg.encode())

