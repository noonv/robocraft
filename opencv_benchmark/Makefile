#
# OpenCV benchmark
#
#
# Author: 	Vladimir  aka noonv
# email: 	noonv13@gmail.com
# site:	robocraft.ru
#

CC		:= g++
TARGET	:= opencv_benchmark

# объекты
OBJECTS := benchmark.o

# флаги компилятора
CFLAGS		:= -I.  -DLINUX=1
# flags for debug
CFLAGSD		:= -I.  -DLINUX=1 -O0 -g
# flags for release
CFLAGSR		:= -I. -DLINUX=1 -O3 -msse -msse2

LIBRARIES	:= 

# for use OpenCV
OPENCVFLAGS		:= -DUSE_OPENCV=1 -I/usr/local/include/opencv -L/usr/local/lib -Wl,-rpath,/usr/local/lib
OPENCVLIBRARIES := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_flann -lopencv_features2d

OPENCVFLAGS21	:= -DUSE_OPENCV=1 -I/usr/include/opencv -L/usr/lib -Wl,-rpath,/usr/lib
OPENCVLIBRARIES21 := -lcv -lcxcore -lhighgui -lml

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OPENCVFLAGS) -o $(TARGET) $(OBJECTS) main.cpp $(LIBRARIES) $(OPENCVLIBRARIES)

%.o: %.cpp
	$(CC) $(CFLAGS) $(OPENCVFLAGS) -c -o $@ $< $(LIBRARIES)  $(OPENCVLIBRARIES)

clean:
	rm -f *.o $(TARGET)
	
rebuild:
	make clean
	make

cv21:
	make OPENCVFLAGS="$(OPENCVFLAGS21)" OPENCVLIBRARIES="$(OPENCVLIBRARIES21)"

release:
	make CFLAGS="$(CFLAGSR)"
