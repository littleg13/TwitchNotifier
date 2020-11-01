CPP = cl.exe
C_FLAGS = -c /MDd /EHsc -I../include -I../cryphutil

LINK = lib.exe

OBJS = CFont.obj CGLString.obj

libfont.lib: $(OBJS)
	$(LINK) /OUT:libfont.lib $(OBJS)
	copy "libfont.lib" "../lib/"

CFont.obj: CFont.h CFont.cpp
	$(CPP) $(C_FLAGS) CFont.cpp

CGLString.obj: CGLString.h CGLString.cpp
	$(CPP) $(C_FLAGS) CGLString.cpp
