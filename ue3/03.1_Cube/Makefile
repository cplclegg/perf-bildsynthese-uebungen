CFLAGS = -g -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

SOURCES = \
main.cpp \
Instance.cpp \
Device.cpp \
VertexBuffer.cpp

testapp: $(SOURCES) shaders/testapp.vert.spv shaders/testapp.frag.spv HelloVulkan4.hpp
	g++ $(CFLAGS) -o testapp $(SOURCES) $(LDFLAGS)

%.vert.spv: %.vert
	glslangValidator -V -o $@ $<

%.frag.spv: %.frag
	glslangValidator -V -o $@ $<

.PHONY: test clean

test: testapp
	./testapp

clean:
	rm -f testapp
	rm -f shaders/*.spv

