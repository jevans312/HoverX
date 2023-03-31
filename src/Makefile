CXX := g++
LD := g++
FLAGS := -g
#FLAGS := -march=native -O3
EXTRA_CFLAGS := -m64 -std=c++11 -Wall -Wextra -Wpedantic
INC := -I/usr/include
LIB := -lm -ldl -lSDL2 -lSDL2_image -lGL -lGLU -lGLEW -ltinyxml -lenet

TARGET := HoverX
all: ${TARGET}
	@mv ./HoverX ../
	@echo Build complete: HoverX executable is in the root director so it can find neccesary assets

# SOURCE FILES
SRCS := main.cpp client.cpp entity.cpp glex.cpp level.cpp line2d.cpp obj.cpp \
	poly2d.cpp renderer.cpp server.cpp font.cpp tinerxml.cpp ui.cpp

OBJS := $(addsuffix .o,$(basename ${SRCS}))

# OUTPUT
${TARGET}: ${OBJS}
	${LD} -o $@ ${FLAGS} ${OBJS} ${LIB}

# RULES
%.o : %.cpp
	${CXX} -c ${FLAGS} ${EXTRA_CFLAGS} ${INC} $< -o $@


main.o: main.cpp
client.o: client.cpp
entity.o: entity.cpp
glex.o: glex.cpp
level.o: level.cpp
line2d.o: line2d.cpp
obj.o: obj.cpp
poly2d.o: poly2d.cpp
renderer.o: renderer.cpp
server.o: server.cpp
font.o: font.cpp
tinerxml.o: tinerxml.cpp
ui.o: ui.cpp

clean:
	-rm -f ../HoverX
	-rm -f core ${OBJS} ${OUTPUT}

