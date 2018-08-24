C++?=c++
CXXFLAGS = -Wall -Werror -O2 -std=c++14 -pthread
LDFLAGS = -lminifb -L. -framework Cocoa
HEADERS = \
	aabb.h \
	aarect.h \
	box.h \
	bvh.h \
	camera.h \
	constant_medium.h \
	hitable.h \
	hitable_list.h \
	material.h \
	moving_sphere.h \
	perlin.h \
	ray.h \
	rng.h \
	sphere.h \
	stb_image.h \
	stb_image_write.h \
	surface_texture.h \
	texture.h \
	vec3.h

ENKITS_HEADERS = \
	enkiTS/Atomics.h \
	enkiTS/LockLessMultiReadPipe.h \
	enkiTS/TaskScheduler.h \
	enkiTS/Threads.h

.PHONY: all
all: rtiow

enkiTS.o: enkiTS/TaskScheduler.cpp $(ENKITS_HEADERS)
	$(C++) -c -o $@ $< $(CXXFLAGS)

main.o: main.cpp $(HEADERS)
	$(C++) -c -o $@ $< $(CXXFLAGS)

rtiow: enkiTS.o main.o
	$(C++) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f a.out rtiow *.o
