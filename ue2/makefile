CFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

sys_specs: u2main.cpp
	g++ $(CFLAGS) -o sys_specs u2main.cpp $(LDFLAGS)

.PHONY: test clean

test: sys_specs
	./sys_specs

clean:
	rm -f sys_specs