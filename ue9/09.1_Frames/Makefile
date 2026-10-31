CFLAGS = -g -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

HEADER_FILES = \
Instance.hpp \
Device.hpp \
Vertex.hpp \
VertexBuffer.hpp \
Texture.hpp \
Window.hpp \
Surface.hpp \
Swapchain.hpp \
Depthbuffer.hpp \
Framebuffers.hpp \
GraphicsPipeline.hpp \
Scene.hpp \
Frame.hpp \
DescriptorPool.hpp

SOURCE_FILES = \
main.cpp \
Instance.cpp \
Device.cpp \
VertexBuffer.cpp \
Texture.cpp \
Window.cpp \
Surface.cpp \
Swapchain.cpp \
Depthbuffer.cpp \
Framebuffers.cpp \
GraphicsPipeline.cpp \
Frame.cpp \
DescriptorPool.cpp

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
	rm -f $(SHADERS)

