CPP = cl.exe /Zi /EHsc
C_FLAGS = -c /MDd -I../cryphutil -I../glslutil -I../include /EHsc

LINK = lib.exe
MAKE = nmake
OBJS = Controller.obj ModelView.obj GLFWController.obj BasicShape.obj BasicShapeRenderer.obj

libmvc.lib: $(OBJS) ../lib/libcryph.lib ../lib/libglsl.lib
	$(LINK) /OUT:libmvc.lib $(OBJS) ../lib/libcryph.lib ../lib/libglsl.lib
	copy "libmvc.lib" "../lib/"

../lib/libcryph.lib: ../cryphutil/AffVector.h ../cryphutil/AffVector.cpp ../cryphutil/AffPoint.h ../cryphutil/AffPoint.cpp
	cd  ../cryphutil && $(MAKE)

../lib/libglsl.lib: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.cpp
	cd ../glslutil && $(MAKE)

Controller.obj: Controller.h Controller.cpp
	$(CPP) $(C_FLAGS) Controller.cpp

GLFWController.obj: GLFWController.h GLFWController.cpp
	$(CPP) $(C_FLAGS) GLFWController.cpp

ModelView.obj: ModelView.h ModelView.cpp
	$(CPP) $(C_FLAGS) ModelView.cpp

BasicShape.obj: BasicShape.h BasicShape.cpp
	$(CPP) $(C_FLAGS) BasicShape.cpp

BasicShapeRenderer.obj: BasicShapeRenderer.h BasicShapeRenderer.cpp
	$(CPP) $(C_FLAGS) BasicShapeRenderer.cpp
