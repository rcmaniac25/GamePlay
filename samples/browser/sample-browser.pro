#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT -= core gui

TARGET = sample-browser
TEMPLATE = app

SOURCES += src/Audio3DSample.cpp \
    src/BillboardSample.cpp \
    src/CreateSceneSample.cpp \
    src/FirstPersonCamera.cpp \
    src/FormsSample.cpp \
    src/GamepadSample.cpp \
    src/GestureSample.cpp \
    src/Grid.cpp \
    src/InputSample.cpp \
    src/LightSample.cpp \
    src/LoadSceneSample.cpp \
    src/MeshBatchSample.cpp \
    src/MeshPrimitiveSample.cpp \
	src/ParticlesSample.cpp \
    src/PhysicsCollisionObjectSample.cpp \
    src/PostProcessSample.cpp \
    src/SpriteBatchSample.cpp \
    src/TerrainSample.cpp \
    src/Sample.cpp \
    src/SamplesGame.cpp \
    src/TextSample.cpp \
    src/TextureSample.cpp \
    src/TriangleSample.cpp \
    src/WaterSample.cpp

HEADERS += src/Audio3DSample.h \
    src/BillboardSample.h \
    src/CreateSceneSample.h \
    src/FirstPersonCamera.h \
    src/FormsSample.h \
    src/GamepadSample.h \
    src/GestureSample.h \
    src/Grid.h \
    src/InputSample.h \
    src/LightSample.h \
    src/LoadSceneSample.h \
    src/MeshBatchSample.h \
    src/MeshPrimitiveSample.h \
	src/ParticlesSample.h \
    src/PhysicsCollisionObjectSample.h \
    src/PostProcessSample.h \
    src/SpriteBatchSample.h \
    src/TerrainSample.h \
    src/Sample.h \
    src/SamplesGame.h \
    src/TextSample.h \
    src/TextureSample.h \
    src/TriangleSample.h \
    src/WaterSample.h

INCLUDEPATH += $$PWD/../../gameplay/src
INCLUDEPATH += $$PWD/../../external-deps/bullet/include
INCLUDEPATH += $$PWD/../../external-deps/lua/include
INCLUDEPATH += $$PWD/../../external-deps/png/include
INCLUDEPATH += $$PWD/../../external-deps/zlib/include
INCLUDEPATH += $$PWD/../../external-deps/ogg/include
INCLUDEPATH += $$PWD/../../external-deps/vorbis/include
INCLUDEPATH += $$PWD/../../external-deps/openal/include
linux:!android: INCLUDEPATH += $$PWD/../../external-deps/glew/include
linux:!android: INCLUDEPATH += /usr/include/gtk-2.0
linux:!android: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/gtk-2.0/include
linux:!android: INCLUDEPATH += /usr/include/atk-1.0
linux:!android: INCLUDEPATH += /usr/include/cairo
linux:!android: INCLUDEPATH += /usr/include/gdk-pixbuf-2.0
linux:!android: INCLUDEPATH += /usr/include/pango-1.0
linux:!android: INCLUDEPATH += /usr/include/gio-unix-2.0
linux:!android: INCLUDEPATH += /usr/include/freetype2
linux:!android: INCLUDEPATH += /usr/include/glib-2.0
linux:!android: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/glib-2.0/include
linux:!android: INCLUDEPATH += /usr/include/pixman-1
linux:!android: INCLUDEPATH += /usr/include/libpng12
linux:!android: INCLUDEPATH += /usr/include/harfbuzz
DEPENDPATH += INCLUDEPATH

linux:!android: DEFINES += GP_USE_GAMEPAD
linux:!android: DEFINES += __linux__
linux:!android: QMAKE_CXXFLAGS += -std=c++11 -lstdc++ -pthread -w

linux:!android: LIBS += -L$$PWD/../../gameplay/Debug/ -lgameplay
linux:!android: LIBS += -L$$PWD/../../external-deps/lua/lib/linux/x64/ -llua
linux:!android: LIBS += -L$$PWD/../../external-deps/png/lib/linux/x64/ -lpng
linux:!android: LIBS += -L$$PWD/../../external-deps/zlib/lib/linux/x64/ -lz
linux:!android: LIBS += -L$$PWD/../../external-deps/vorbis/lib/linux/x64/ -lvorbis
linux:!android: LIBS += -L$$PWD/../../external-deps/ogg/lib/linux/x64/ -logg
linux:!android: LIBS += -L$$PWD/../../external-deps/bullet/lib/linux/x64/ -lBulletDynamics
linux:!android: LIBS += -L$$PWD/../../external-deps/bullet/lib/linux/x64/ -lBulletCollision
linux:!android: LIBS += -L$$PWD/../../external-deps/bullet/lib/linux/x64/ -lLinearMath
linux:!android: LIBS += -L$$PWD/../../external-deps/openal/lib/linux/x64/ -lopenal
linux:!android: LIBS += -L$$PWD/../../external-deps/glew/lib/linux/x64/ -lGLEW
linux:!android: LIBS += -lm
linux:!android: LIBS += -lGL
linux:!android: LIBS += -lrt
linux:!android: LIBS += -ldl
linux:!android: LIBS += -lX11
linux:!android: LIBS += -lpthread
linux:!android: LIBS += -lgtk-x11-2.0
linux:!android: LIBS += -lglib-2.0
linux:!android: LIBS += -lgobject-2.0

linux:!android: PRE_TARGETDEPS += $$PWD/../../gameplay/Debug/libgameplay.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/lua/lib/linux/x64/liblua.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/bullet/lib/linux/x64/libLinearMath.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/bullet/lib/linux/x64/libBulletCollision.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/bullet/lib/linux/x64/libBulletDynamics.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/png/lib/linux/x64/libpng.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/zlib/lib/linux/x64/libz.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/ogg/lib/linux/x64/libogg.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/vorbis/lib/linux/x64/libvorbis.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/openal/lib/linux/x64/libopenal.a
linux:!android: PRE_TARGETDEPS += $$PWD/../../external-deps/glew/lib/linux/x64/libGLEW.a

linux:!android: QMAKE_POST_LINK += $$quote(rsync -rau $$PWD/../../gameplay/res/shaders ../res$$escape_expand(\n\t))
linux:!android: QMAKE_POST_LINK += $$quote(rsync -rau $$PWD/../../gameplay/res/ui ../res$$escape_expand(\n\t))
linux:!android: QMAKE_POST_LINK += $$quote(cp -rf $$PWD/../../gameplay/res/logo_powered_white.png ../res$$escape_expand(\n\t))

