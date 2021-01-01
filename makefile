XRES=128
YRES=96
CPP=g++
LD=g++
CXXFLAGS=-std=c++17 -Wall -g -O3 -m32 -DImage_Width=$(XRES) -DImage_Height=$(YRES)
LDFLAGS=-s -m32 -nostdlib -static -N -T bitmap.ld -Wl,--gc-sections -Wl,--build-id=none
LDLIBS=
OBJCOPY=objcopy

all: raytracing.bmp

raytracing.bmp: raytracing.elf
	$(OBJCOPY) -O binary $^ $@

raytracing.elf: raytracing.o bitmap.ld
	$(LD) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f *.bmp *.o *.elf
