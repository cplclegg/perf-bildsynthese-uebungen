CFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

triangle: main.cpp shaders/triangle.vert.spv shaders/triangle.frag.spv
	g++ $(CFLAGS) -o triangle main.cpp $(LDFLAGS)

%.vert.spv: %.vert
	glslangValidator -V -o $@ $<

%.frag.spv: %.frag
	glslangValidator -V -o $@ $<

.PHONY: test clean

test: triangle
	./triangle

clean:
	rm -f triangle
	rm -f shaders/*.spv

