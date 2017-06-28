CC = g++
RM = rm -rf
CFLAGS = -c -Wall -std=c++11 -I.
LDFLAGS = -llcm -lpthread -lglib-2.0 -ltiff -lgeotiff -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_calib3d -lopencv_stitching

RTDP_OBJS = rtdp_core/BlurMetric.o \
  	rtdp_core/CameraFrame.o \
  	rtdp_core/DirectGeoref.o \
	rtdp_core/ErosionDilation.o \
  	rtdp_core/GeoImage.o \
  	rtdp_core/Histogram.o \
  	rtdp_core/HotSpot.o \
  	rtdp_core/HotSpotList.o \
  	rtdp_core/Image.o \
  	rtdp_core/IrImage.o \
  	rtdp_core/JellyfishFunctions.o \
  	rtdp_core/LNEDFrame.o \
  	rtdp_core/Metadata.o \
  	rtdp_core/Payload.o \
  	rtdp_core/Utils.o \
  	rtdp_core/XmlParser.o


SERVICES = hotspot_service jellyfish_service georef_service fusion_service  quality_service

# All Targets
all: $(SERVICES)

# One Target
hotspot_service: $(RTDP_OBJS) hotspot_service.o
	$(CC) -o $@ $^ $(LDFLAGS)

jellyfish_service: $(RTDP_OBJS) jellyfish_service.o
	$(CC) -o $@ $^ $(LDFLAGS)

georef_service: $(RTDP_OBJS) georef_service.o
	$(CC) -o $@ $^ $(LDFLAGS)

fusion_service: $(RTDP_OBJS) fusion_service.o
	$(CC) -o $@ $^ $(LDFLAGS)

quality_service: $(RTDP_OBJS) quality_service.o
	$(CC) -o $@ $^ $(LDFLAGS)

.cpp.o: 
	$(CC) $(CFLAGS) $< -o $@

# Other Targets
clean:
	$(RM) $(RTDP_OBJS) *.o *~ 

clean-all:
	$(RM) $(RTDP_OBJS) *.o *~ $(SERVICES)

