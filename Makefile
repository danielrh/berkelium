#  Berkelium - Embedded Chromium
#  Makefile
#
#  Copyright (c) 2009, Patrick Reiter Horn
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of Sirikata nor the names of its contributors may
#    be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

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

ifeq ($(shell uname),Darwin)
CHROMIUMBUILDER=xcode
endif

ifeq ($(CHROMIUMBUILDER),xcode)
PLAT=mac
PLATCFLAGS=-pthread
SNOW=

PLATLDFLAGS=-dynamiclib -pthread $(CHROMIUMDIR)/src/third_party/WebKit/WebKitLibraries/libWebKitSystemInterface$(SNOW)Leopard.a -framework CoreAudio -framework AudioToolbox -framework Cocoa -framework QuartzCore -framework Security -framework SecurityInterface -framework SystemConfiguration -ObjC -framework Carbon chromium/src/xcodebuild/chrome.build/$(CHROMIUMMODE)/chrome_dll.build/Objects-normal/i386/keystone_glue.o -framework OpenGL -framework JavaScriptCore
START_GROUP=
END_GROUP=
DLLEXT=dylib

else
PLAT=linux
PLATCFLAGS=`pkg-config --cflags gtk+-2.0 glib-2.0 gio-unix-2.0`
PLATLDFLAGS=`pkg-config --libs gtk+-2.0 glib-2.0 gio-unix-2.0 gconf-2.0` -lssl3 -lnss3 -lnssutil3 -lsmime3 -lplds4 -lplc4 -lnspr4 -lpthread -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -llinux_versioninfo -lpangocairo-1.0 -lgio-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lfontconfig -lfreetype -lrt -lgconf-2 -lglib-2.0 -lX11 -lasound -lharfbuzz -lharfbuzz_interface -lsandbox -lnonnacl_util_linux
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
CHLIBS=$(CHROMIUMDIR)/src/out/$(CHROMIUMMODE)/obj.target
endif

LIBDIRS=. app base ipc chrome net media webkit sandbox skia printing v8/tools/gyp sdch build/temp_gyp native_client/src/trusted/plugin/ native_client/src/shared/srpc native_client/src/shared/imc native_client/src/shared/platform native_client/src/trusted/nonnacl_util native_client/src/trusted/nonnacl_util/linux native_client/src/trusted/service_runtime/  native_client/src/trusted/desc/ 
THIRDPARTYLIBDIRS=bzip2 ffmpeg harfbuzz hunspell icu libevent libjpeg libpng libxml libxslt lzma_sdk modp_b64 sqlite zlib WebKit/JavaScriptCore/JavaScriptCore.gyp WebKit/WebCore/WebCore.gyp
CHROMIUMLDFLAGS=$(addprefix -L$(CHLIBS)/,$(LIBDIRS)) $(addprefix -L$(CHLIBS)/third_party/,$(THIRDPARTYLIBDIRS))
TPLIBS=-levent -lzlib -lpng -lxml -ljpeg -lxslt -lbzip2 -lsqlite -lgoogle_nacl_imc_c

GENINCLUDES=$(CHLIBS)/gen/chrome

else
#### xcode build (Mac)

CHLIBS=$(CHROMIUMLIBPATH)
ifeq ($(CHLIBS),)
CHLIBS=$(CHROMIUMDIR)/src/xcodebuild/$(CHROMIUMMODE)
endif

CHROMIUMLDFLAGS=-L$(CHLIBS)

TPLIBS=$(CHLIBS)/libevent.a $(CHLIBS)/libxslt.a $(CHLIBS)/libjpeg.a $(CHLIBS)/libpng.a $(CHLIBS)/libz.a $(CHLIBS)/libxml2.a $(CHLIBS)/libbz2.a $(CHLIBS)/libsqlite3.a $(CHLIBS)/libprofile_import.a -llibgoogle_nacl_imc_c

GENINCLUDES=$(CHROMIUMDIR)/src/xcodebuild/DerivedSources/$(CHROMIUMMODE)/chrome

endif
else
#### scons build system

GENINCLUDES=$(CHROMIUMDIR)/src/out/$(CHROMIUMMODE) #not sure

ifeq ($(CHROMIUMLIBPATH),)
CHROMIUMLIBPATH=$(CHROMIUMDIR)/src/out/$(CHROMIUMMODE)/lib
endif

CHROMIUMLDFLAGS=-L$(CHROMIUMLIBPATH)
TPLIBS=-levent -lxslt -ljpeg -lpng -lz -lxml2 -lbz2 -lsqlite -lgoogle_nacl_imc_c
endif
CHROMIUMLIBS=$(CHROMIUMLDFLAGS) $(TPLIBS)  -ldl -lm -lcommon -lbrowser -ldebugger -lrenderer -lutility -lprinting -lapp_base -lbase -licui18n -licuuc -licudata -lskia -lnet -lgoogleurl -lsdch -lmodp_b64 -lv8_snapshot -lv8_base -lglue -lpcre -lwtf -lwebkit -lwebcore -lmedia -lffmpeg -lhunspell -lplugin -l appcache -lipc -lworker -ldatabase -lcommon_constants -lnpGoogleNaClPluginChrome -lnonnacl_srpc -lplatform -lsel -lsel_ldr_launcher -lnonnacl_util_chrome -lnrd_xfer -lgio -lexpiration -lnacl -lbase_i18n
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

ifeq ($(PLAT),mac)
EXEDEPS=$(TARGET) $(EXEDIR)/plugin_carbon_interpose.dylib
else
EXEDEPS=$(TARGET)
endif

#all: mymain

all: $(TARGET) berkelium ppmrender

$(TARGET): $(OBJS)
	mkdir -p $(EXEDIR)
	g++ $(OBJS) $(LIBS) -o $@

#src/%.cpp $(HEADERS)
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(OBJDIR)||true
	g++ $(CFLAGS) -DBERKELIUM_BUILD -c $< -o $@.tmp && mv $@.tmp $@

$(EXEDIR)/plugin_carbon_interpose.dylib: $(OBJDIR)/plugin_carbon_interpose.o
	@mkdir -p $(EXEDIR)||true
	g++ $< -dynamic -dylib -dynamiclib -L$(EXEDIR) -framework Carbon -lberkelium -o $@

$(OBJDIR)/plugin_carbon_interpose.o: $(CHROMIUMDIR)/src/chrome/browser/plugin_carbon_interpose_mac.cc
	@mkdir -p $(OBJDIR)||true
	g++ $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

berkelium: $(EXEDEPS) subprocess.cpp
	g++ -g $(CFLAGS) -L$(EXEDIR) -lberkelium subprocess.cpp -o berkelium

ppmrender: $(EXEDEPS) ppmmain.cpp
	g++ -g $(CFLAGS) -L$(EXEDIR) -lberkelium ppmmain.cpp -o ppmrender

