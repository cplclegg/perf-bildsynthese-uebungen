CFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

testapp: main.cpp Instance.cpp shaders/triangle.vert.spv shaders/triangle.frag.spv HelloVulkan2.hpp
	g++ $(CFLAGS) -o testapp main.cpp Instance.cpp $(LDFLAGS)

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

