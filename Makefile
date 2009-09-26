
ifeq ($(CHROMIUMDIR),)
CHROMIUMDIR=chromium
endif

ifeq ($(CHROMIUMMODE),)
CHROMIUMMODE=Debug
endif

ifeq ($(CHROMIUMMODE),Debug)
DEBUGFLAGS=-D_DEBUG
else
DEBUGFLAGS=-DNDEBUG
endif

ifeq ($(CHROMIUMBUILDER),xcode)
PLAT=mac
PLATCFLAGS=-pthread
SNOW=

PLATLDFLAGS=-dynamiclib -pthread $(CHROMIUMDIR)/src/third_party/WebKit/WebKitLibraries/libWebKitSystemInterface$(SNOW)Leopard.a -framework CoreAudio -framework AudioToolbox -framework Cocoa -framework QuartzCore -framework Security -framework SecurityInterface -framework SystemConfiguration -ObjC -framework Carbon chromium/src/xcodebuild/chrome.build/$(CHROMIUMMODE)/chrome_dll.build/Objects-normal/i386/keystone_glue.o
START_GROUP=
END_GROUP=
DLLEXT=dylib

else
PLAT=linux
PLATCFLAGS=`pkg-config --cflags gtk+-2.0 glib-2.0 gio-unix-2.0`
PLATLDFLAGS=`pkg-config --libs gtk+-2.0 glib-2.0 gio-unix-2.0 gconf-2.0` -lssl3 -lnss3 -lnssutil3 -lsmime3 -lplds4 -lplc4 -lnspr4 -lpthread -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -llinux_versioninfo -lpangocairo-1.0 -lgio-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lfontconfig -lfreetype -lrt -lgconf-2 -lglib-2.0 -lX11 -lasound -lharfbuzz -lharfbuzz_interface -lsandbox
START_GROUP=-Wl,--start-group
END_GROUP=-Wl,--end-group
DLLEXT=so

ifeq ($(ARCHFLAGS),)
ARCHFLAGS=-m64
endif
endif

ifneq ($(CHROMIUMBUILDER),scons)
ifneq ($(CHROMIUMBUILDER),xcode)
#### make build system

CHLIBS=$(CHROMIUMLIBPATH)
ifeq ($(CHLIBS),)
CHLIBS=$(CHROMIUMDIR)/src/out/$(CHROMIUMMODE)/obj
endif

LIBDIRS=. app base ipc chrome net media webkit sandbox skia printing v8/tools/gyp sdch build/temp_gyp
THIRDPARTYLIBDIRS=bzip2 ffmpeg harfbuzz hunspell icu libevent libjpeg libpng libxml libxslt lzma_sdk modp_b64 sqlite zlib

CHROMIUMLDFLAGS=$(addprefix -L$(CHLIBS)/,$(LIBDIRS)) $(addprefix -L$(CHLIBS)/third_party/,$(THIRDPARTYLIBDIRS))
TPLIBS=-llibevent -lzlib -llibpng -llibxml -llibjpeg -llibxslt -lbzip2 -lsqlite

GENINCLUDES=$(CHROMIUMDIR)/src/out/$(CHROMIUMMODE)/obj/gen/chrome

else
#### xcode build (Mac)

CHLIBS=$(CHROMIUMLIBPATH)
ifeq ($(CHLIBS),)
CHLIBS=$(CHROMIUMDIR)/src/xcodebuild/$(CHROMIUMMODE)
endif

CHROMIUMLDFLAGS=-L$(CHLIBS)

TPLIBS=$(CHLIBS)/libevent.a $(CHLIBS)/libxslt.a $(CHLIBS)/libjpeg.a $(CHLIBS)/libpng.a $(CHLIBS)/libz.a $(CHLIBS)/libxml2.a $(CHLIBS)/libbz2.a $(CHLIBS)/libsqlite3.a $(CHLIBS)/libprofile_import.a

GENINCLUDES=$(CHROMIUMDIR)/src/xcodebuild/DerivedSources/$(CHROMIUMMODE)/chrome

endif
else
#### scons build system

GENINCLUDES=$(CHROMIUMDIR)/src/sconsbuild/$(CHROMIUMMODE) #not sure

ifeq ($(CHROMIUMLIBPATH),)
CHROMIUMLIBPATH=$(CHROMIUMDIR)/src/sconsbuild/$(CHROMIUMMODE)/lib
endif

CHROMIUMLDFLAGS=-L$(CHROMIUMLIBPATH)
TPLIBS=-levent -lxslt -ljpeg -lpng -lz -lxml2 -lbz2 -lsqlite
endif
CHROMIUMLIBS=$(CHROMIUMLDFLAGS) $(TPLIBS)  -ldl -lm -lcommon -lbrowser -ldebugger -lrenderer -lutility -lprinting -lapp_base -lbase -licui18n -licuuc -licudata -lbase_gfx -lskia -lnet -lgoogleurl -lsdch -lmodp_b64 -lv8_snapshot -lv8_base -lglue -lwebcore -lpcre -lwtf -lwebkit -lmedia -lffmpeg -lhunspell -lplugin -l appcache -lipc -lworker
# Flags that affect both compiling and linking
CLIBFLAGS=$(ARCHFLAGS) -fvisibility=hidden -fvisibility-inlines-hidden -fPIC -pthread -Wall -fno-rtti

CFLAGS=$(DEBUGFLAGS) $(CLIBFLAGS) $(PLATCFLAGS) -Wall -DNVALGRIND -D_REENTRANT -D__STDC_FORMAT_MACROS -DCHROMIUM_BUILD -DU_STATIC_IMPLEMENTATION -g -I ./include -I $(GENINCLUDES) $(addprefix -I$(CHROMIUMDIR)/src/,. third_party/npapi third_party/WebKit/JavaScriptCore third_party/icu/public/common skia/config third_party/skia/include/core webkit/api/public third_party/WebKit/WebCore/platform/text)

LIBS=-shared $(CLIBFLAGS) -g $(START_GROUP) $(PLATLDFLAGS) $(CHROMIUMLIBS) $(END_GROUP)

OBJDIR=$(CHROMIUMMODE)
EXEDIR=$(CHROMIUMMODE)

SRCS=$(wildcard src/*.cpp)
OBJS=$(addprefix $(OBJDIR)/,$(notdir $(SRCS:.cpp=.o)))
HEADERS=$(wildcard include/berkelium/*.hpp) $(wildcard src/*.hpp)

TARGET=$(EXEDIR)/libberkelium.$(DLLEXT)

#all: mymain

all: $(TARGET) berkelium ppmrender

$(TARGET): $(OBJS)
	mkdir -p $(EXEDIR)
	g++ $(OBJS) $(LIBS) -o $@

#src/%.cpp $(HEADERS)
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)||true
	g++ $(CFLAGS) -DBERKELIUM_BUILD -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

berkelium: $(TARGET) subprocess.cpp
	g++ -g $(CFLAGS) -L$(EXEDIR) -lberkelium subprocess.cpp -o berkelium

ppmrender: $(OBJS) ppmmain.cpp $(TARGET)
	g++ -g $(CFLAGS) -L$(EXEDIR) -lberkelium ppmmain.cpp -o ppmrender

