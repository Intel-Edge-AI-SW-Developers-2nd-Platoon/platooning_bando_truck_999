#CC:=arm-linux-gcc
CC:=gcc

TARGET_SRV=ccu_server
OBJECT_SRV=$(TARGET_SRV).o 

TARGET_CLN=ccu_client
OBJECT_CLN=$(TARGET_CLN).o 

TARGET_CNT=ccu_client_control
OBJECT_CNT=$(TARGET_CNT).o 

#LDFLAGS=-D_REENTRANT -pthread -lmysqlclient
LDFLAGS=-D_REENTRANT -pthread

all : $(TARGET_SRV) $(TARGET_CLN) $(TARGET_CNT)

$(TARGET_SRV):$(OBJECT_SRV)
	$(CC) -o $@ $(OBJECT_SRV) $(LDFLAGS)

$(TARGET_CLN):$(OBJECT_CLN)
	$(CC) -o $@ $(OBJECT_CLN) $(LDFLAGS)

$(TARGET_CNT):$(OBJECT_CNT)
	$(CC) -o $@ $(OBJECT_CNT) $(LDFLAGS)

$(TARGET_AUT):$(OBJECT_AUT)
	$(CC) -o $@ $(OBJECT_CNT) $(LDFLAGS)
%.o:%.c
	$(CC) -c -o $@ $<
clean:
	rm -f *.o $(TARGET_SRV) $(TARGET_CLN) $(TARGET_CNT)
