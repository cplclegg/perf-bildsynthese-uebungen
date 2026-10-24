CFLAGS = -g -std=c++20 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

HEADER_FILES = \
HelloVulkan7.hpp \
Instance.hpp \
Device.hpp \
Vertex.hpp \
VertexBuffer.hpp \
ImageBuffer.hpp

SOURCE_FILES = \
main.cpp \
Instance.cpp \
Device.cpp \
VertexBuffer.cpp \
ImageBuffer.cpp

SHADERS = \
shaders/testapp.vert.spv \
shaders/testapp.frag.spv

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
	rm -f shaders/*.spv

