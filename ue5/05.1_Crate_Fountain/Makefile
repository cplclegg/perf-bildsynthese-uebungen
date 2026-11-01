CFLAGS = -g -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

HEADER_FILES = \
HelloVulkan9.hpp \
Instance.hpp \
Device.hpp \
Vertex.hpp \
VertexBuffer.hpp \
ImageBuffer.hpp \
Simulation.hpp

SOURCE_FILES = \
main.cpp \
Instance.cpp \
Device.cpp \
VertexBuffer.cpp \
ImageBuffer.cpp \
Simulation.cpp

SHADERS = \
shaders/testapp.vert.spv \
shaders/testapp.frag.spv \
shaders/testapp.comp.spv 

testapp: $(SOURCE_FILES) $(HEADER_FILES) $(SHADERS)
	g++ $(CFLAGS) -o testapp $(SOURCE_FILES) $(LDFLAGS)

%.vert.spv: %.vert
	glslangValidator -V -o $@ $<

%.frag.spv: %.frag
	glslangValidator -V -o $@ $<

%.comp.spv: %.comp
	glslangValidator -V -o $@ $<

.PHONY: test clean

test: testapp
	./testapp

clean:
	rm -f testapp
	rm -f $(SHADERS)

