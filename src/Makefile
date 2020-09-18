CPP = cl.exe /Zi /EHsc
INC = -I../cryphutil -I../fontutil -I../glslutil -I../ImageReader -I../mvcutil -I/usr/local/include -I. -I../include -I../TwitchConnection

C_FLAGS = -c /EHsc /Zi $(INC) -DGL_GLEXT_PROTOTYPES
GL_LIB_LOC = -L/usr/lib/nvidia-375
GL_LIBRARIES = kernel32.lib user32.lib shell32.lib legacy_stdio_definitions.lib ../lib/glfw3dll.lib ../lib/glew32.lib opengl32.lib ../lib/OpenAL32.lib
MAKE = nmake

LOCAL_UTIL_LIBRARIES = ../lib/libcryph.lib ../lib/libglsl.lib  ../lib/libmvc.lib ../lib/libTwitch.lib ../lib/libfont.lib

OBJS = main.obj ExtendedController.obj PhongMaterial.obj SceneElement.obj ParticleSystem.obj Cube.obj FooterText.obj SoundController.obj Semaphore.obj CWaves.obj

main: $(OBJS) $(LOCAL_UTIL_LIBRARIES)
	$(CPP) /Fe:"../bin/Notifier.exe" $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(GL_LIBRARIES)

../lib/libTwitch.lib: ../TwitchConnection/NotifyServer.h ../TwitchConnection/NotifyServer.cpp ../TwitchConnection/TwitchConnection.h ../TwitchConnection/TwitchConnection.cpp
	cd ../TwitchConnection && $(MAKE)

../lib/libcryph.lib: ../cryphutil/AffVector.h ../cryphutil/AffVector.cpp ../cryphutil/AffPoint.h ../cryphutil/AffPoint.cpp
	cd ../cryphutil && $(MAKE)

../lib/libglsl.lib: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.cpp
	cd ../glslutil && $(MAKE)

../lib/libmvc.lib: ../mvcutil/Controller.h ../mvcutil/Controller.cpp ../mvcutil/ModelView.h ../mvcutil/ModelView.cpp
	cd ../mvcutil && $(MAKE)

../lib/libfont.lib: ../fontutil/CFont.h ../fontutil/CFont.cpp ../fontutil/CGLString.h ../fontutil/CGLString.cpp
	cd ../fontutil && $(MAKE)

main.obj: main.cpp
	$(CPP) $(C_FLAGS) main.cpp
ExtendedController.obj: ExtendedController.cpp
	$(CPP) $(C_FLAGS) ExtendedController.cpp
PhongMaterial.obj: PhongMaterial.cpp
	$(CPP) $(C_FLAGS) PhongMaterial.cpp
SceneElement.obj: SceneElement.h SceneElement.cpp
	$(CPP) $(C_FLAGS) SceneElement.cpp
Cube.obj: SceneElements/Cube.h SceneElements/Cube.cpp
	$(CPP) $(C_FLAGS) SceneElements/Cube.cpp
ParticleSystem.obj: SceneElements/ParticleSystem.h SceneElements/ParticleSystem.cpp SceneElements/Cube.h
	$(CPP) $(C_FLAGS) SceneElements/ParticleSystem.cpp
FooterText.obj: SceneElements/FooterText.h SceneElements/FooterText.cpp
	$(CPP) $(C_FLAGS) SceneElements/FooterText.cpp
SoundController.obj: Sound/SoundController.h Sound/SoundController.cpp
	$(CPP) $(C_FLAGS) Sound/SoundController.cpp
Semaphore.obj: Sound/Semaphore.h Sound/Semaphore.cpp
	$(CPP) $(C_FLAGS) Sound/Semaphore.cpp
CWaves.obj: Sound/CWaves.h Sound/CWaves.cpp
	$(CPP) $(C_FLAGS) Sound/CWaves.cpp

clean:
	del *.obj
	cd ../TwitchConnection && $(MAKE) clean