CPP = cl.exe
C_FLAGS = -c /MDd /EHsc -I../include

LINK = lib.exe

OBJS = ShaderIF.obj

libglsl.lib: $(OBJS)
	$(LINK) /OUT:libglsl.lib $(OBJS)
	copy "libglsl.lib" "../lib/"

ShaderIF.obj: ShaderIF.h ShaderIF.cpp
	$(CPP) $(C_FLAGS) ShaderIF.cpp
