CC:=gcc

TARGET_CLN=ccu_client
OBJECT_CLN=$(TARGET_CLN).o 

LDFLAGS=-D_REENTRANT -pthread

$(TARGET_CLN):$(OBJECT_CLN)
	$(CC) -o $@ $(OBJECT_CLN) $(LDFLAGS)
%.o:%.c
	$(CC) -c -o $@ $<
clean:
	rm -f *.o $(TARGET_CLN)
