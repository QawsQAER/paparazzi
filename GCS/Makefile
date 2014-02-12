CC = g++
CFLAGS = -Wall -g
##############################################
#OPTION = -D_USE_XBEE_ESC=1
#############################################


##########DEBUG OPTION#######################
#DEBUG_MACRO = -D_DEBUG_CHECKSUM=1 
#DEBUG_MACRO += -D_DEBUG_XBEE_parse_XBEE_msg=1 
#DEBUG_MACRO += -D_DEBUG=1
#DEBUG_MACRO += -D_DEBUG_VERIFY_CHECKSUM=1
#DEBUG_MACRO += -D_DEBUG_XBEE_SEND_MSG=1
#############################################


TARGETS= main test test_sender test_pprz_msg test_GCS

OBJECT= XBEE.o Ground_Station.o XBEE_msg.o pprz_msg.o Serial.o Swarm.o

GROUND_STATION_SOURCES= Ground_Station.cpp #Swarm.cpp Serial.cpp XBEE.cpp XBEE_msg.cpp pprz_msg.cpp
all: main test
	rm -f *.o	

main: main.o XBEE.o Serial.o 
	$(CC) -o main *.o $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
	rm -f *.o
###############################################################################
###############################################################################
##############################################################################
test: test.o $(OBJECT)
	$(CC) -o test test.o $(OBJECT) $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
	rm -f *.o
#-----------------------------------------------------------------------------#
test_sender: test_sender.o Serial.o XBEE.o XBEE_msg.o
	$(CC) -o test_sender test_sender.o Serial.o XBEE.o XBEE_msg.o $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
	rm -f *.o
#-----------------------------------------------------------------------------#
test_pprz_msg: test_pprz_msg.o pprz_msg.o XBEE_msg.o
	$(CC) -o test_pprz_msg test_pprz_msg.o pprz_msg.o XBEE_msg.o $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
	rm -f *.o

#-----------------------------------------------------------------------------#
test_GCS: test_GCS.o $(OBJECT)
	$(CC) -o test_GCS test_GCS.o $(OBJECT) $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
	rm -f *.o
##############################################################################
##############################################################################
##############################################################################

Ground_Station.o: Ground_Station.h Ground_Station.cpp
	$(CC) -o Ground_Station.o -c $(GROUND_STATION_SOURCES) $(CFLAGS) $(DEBUGS_MACRO) $(OPTION)
test.o: test.cpp
	$(CC) -o test.o -c test.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
test_sender.o: test_sender.cpp
	$(CC) -o test_sender.o -c test_sender.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
test_pprz_msg.o: test_pprz_msg.cpp
	$(CC) -o test_pprz_msg.o -c test_pprz_msg.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
main.o: main.cpp main.h
	$(CC) -o main.o -c main.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
Swarm.o : Swarm.cpp Swarm.h main.h address.h
	$(CC) -o Swarm.o -c Swarm.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
XBEE.o: XBEE.h XBEE.cpp XBEE_msg.cpp
	$(CC) -o XBEE.o -c XBEE.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
XBEE_msg.o: XBEE_msg.h XBEE_msg.cpp
	$(CC) -o XBEE_msg.o -c XBEE_msg.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
Serial.o: Serial.h Serial.cpp
	$(CC) -o Serial.o -c Serial.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
pprz_msg.o: pprz_msg.h pprz_msg.cpp
	$(CC) -o pprz_msg.o -c pprz_msg.cpp $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
geo.o: geo.h geo.cpp GCS_math.h
	$(CC) -o geo.o -c geo.cpp GCS_math.h $(CFLAGS) $(DEBUG_MACRO) $(OPTION)
###############################################################################
###############################################################################
###############################################################################
.PHONY: clean 
clean:
	rm -f $(TARGETS)
	rm -f *.o
