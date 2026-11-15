# Offline tests. Works with g++ or clang++ on Linux (the Mac build is in
# scripts/build_vst_mac.sh). Everything is compiled as C++98 to prove it builds
# on old toolchains.
#   make run        preset store + DSP test suite, writes demo WAVs to ./renders
#   make vst-run    builds the plugin as a shared library and loads it in a mini VST host
#   make asan       both, under AddressSanitizer and UndefinedBehaviorSanitizer

CXX      ?= c++
CXXFLAGS ?= -O2 -std=c++98 -Wall -Wextra -pedantic -Wno-long-long
DSP_SRC   = ../Source/DSP/Synth.cpp ../Source/DSP/Moog.cpp ../Source/DSP/OBX.cpp ../Source/DSP/Solina.cpp \
            ../Source/DSP/Reeds.cpp ../Source/DSP/Hammond.cpp ../Source/DSP/Clav.cpp ../Source/DSP/Multiman.cpp
HEADERS   = ../Source/DSP/*.h ../Source/VST2/*.h

synth_test: $(DSP_SRC) synth_test.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(DSP_SRC) synth_test.cpp -o $@

TBSynth.so: $(DSP_SRC) ../Source/VST2/SynthVST.cpp ../Source/VST2/Presets.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -fPIC -fvisibility=hidden -shared ../Source/VST2/SynthVST.cpp ../Source/VST2/Presets.cpp $(DSP_SRC) -o $@

vst_host_test: vst_host_test.cpp $(DSP_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) vst_host_test.cpp $(DSP_SRC) -ldl -o $@

preset_test: preset_test.cpp ../Source/VST2/Presets.cpp ../Source/VST2/*.h
	$(CXX) $(CXXFLAGS) preset_test.cpp ../Source/VST2/Presets.cpp -o $@

run: synth_test preset_test
	./preset_test
	mkdir -p renders
	./synth_test renders

vst-run: TBSynth.so vst_host_test
	./vst_host_test ./TBSynth.so

asan:
	$(MAKE) clean
	$(MAKE) run vst-run CXXFLAGS="-O1 -g -std=c++98 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer" LDFLAGS="-fsanitize=address,undefined"
	$(MAKE) clean

clean:
	rm -rf synth_test preset_test vst_host_test TBSynth.so renders

.PHONY: run vst-run asan clean
