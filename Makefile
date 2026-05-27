CXX ?= clang++
LD := $(CXX)
TARGET ?= HoverX
PREFIX ?= /usr/local
DESTDIR ?= ~/HoverX

# Build mode: debug or release
BUILD ?= debug
CXXFLAGS := -std=c++20 -m64 -Wall -Wextra -Wpedantic -Wconversion -fno-exceptions
CXXFLAGS += -Wshadow -Wduplicated-cond -Wlogical-op -MMD -MP
LDFLAGS := -fuse-ld=lld
LDLIBS := -lSDL2 -lSDL2_image -lX11 -lGL -lGLU -lGLEW -ltinyxml -lenet

# Include directories
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
INC := -I/usr/include
else ifeq ($(UNAME_S),Darwin)
INC := -I/usr/local/include
else
INC := -IC:/msys64/ucrt64/include
endif


# BUILD=release for release build
ifeq ($(BUILD),release)
CXXFLAGS += -O3 -march=native -ffast-math -fstack-protector-strong -D_FORTIFY_SOURCE=2 -DNDEBUG
else
CXXFLAGS += -g
endif

SRCDIR := src
SRCS := $(SRCDIR)/main.cpp $(SRCDIR)/sdl.cpp $(SRCDIR)/client.cpp $(SRCDIR)/entity.cpp $(SRCDIR)/glex.cpp $(SRCDIR)/level.cpp $(SRCDIR)/line2d.cpp $(SRCDIR)/obj.cpp \
	$(SRCDIR)/poly2d.cpp $(SRCDIR)/renderer.cpp $(SRCDIR)/server.cpp $(SRCDIR)/font.cpp $(SRCDIR)/tinerxml.cpp $(SRCDIR)/ui.cpp

OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(SRCDIR)/%.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

-include $(DEPS)

install: $(TARGET)
	install -d $(DESTDIR)
	install -m 755 $(TARGET) $(DESTDIR)/$(TARGET)
	install -d $(DESTDIR)/img $(DESTDIR)/levels $(DESTDIR)/models
	cp -a img $(DESTDIR)/
	cp -a levels $(DESTDIR)/
	cp -a models $(DESTDIR)/

uninstall:
	rm -f $(DESTDIR)/$(TARGET)
	rm -rf $(DESTDIR)/img $(DESTDIR)/levels $(DESTDIR)/models
	rmdir $(DESTDIR)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

