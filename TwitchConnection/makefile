CPP = cl.exe
INC = -I../include
C_FLAGS = -c /MDd /EHsc /Zi $(INC)
LINK = lib.exe
OBJS = TwitchConnection.obj json11.obj NotifyServer.obj HTTP.obj ChatBot.obj Commands.obj EventQueue.obj WebSocket.obj
LOCAL_UTIL_LIBRARIES = crypt32.lib Rpcrt4.lib Advapi32.lib Ole32.lib User32.lib ../lib/libssl.lib ../lib/libcrypto.lib Ws2_32.lib 

libTwitch.lib: $(OBJS) 
	$(LINK) /OUT:libTwitch.lib $(OBJS) $(LOCAL_UTIL_LIBRARIES)
	copy "libTwitch.lib" "../lib/"
TwitchConnection.obj: TwitchConnection.cpp
	$(CPP) $(C_FLAGS) TwitchConnection.cpp
NotifyServer.obj: NotifyServer.cpp
	$(CPP) $(C_FLAGS) NotifyServer.cpp
ChatBot.obj: ChatBot.cpp ChatBot.h
	$(CPP) $(C_FLAGS) ChatBot.cpp
WebSocket.obj: WebSocket.cpp WebSocket.h
	$(CPP) $(C_FLAGS) WebSocket.cpp
HTTP.obj: HTTP.cpp HTTP.h
	$(CPP) $(C_FLAGS) HTTP.cpp
Commands.obj: Commands.cpp Commands.h
	$(CPP) $(C_FLAGS) Commands.cpp
json11.obj: json11.cpp
	$(CPP) $(C_FLAGS) json11.cpp
EventQueue.obj: EventQueue.cpp
	$(CPP) $(C_FLAGS) EventQueue.cpp

clean:
	del *.obj
	del libTwitch.lib
	del ..\lib\libTwitch.lib