cmake_minimum_required(VERSION 3.24)
project(HoverX)
add_executable(HoverX
    main.cpp
    poly2d.cpp
	client.cpp
	entity.cpp
	glex.cpp
	level.cpp
	line2d.cpp
	obj.cpp
	renderer.cpp
	server.cpp
	SimpleFont.cpp
	tinerxml.cpp
	ui.cpp
)

find_package(LibXml2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

target_include_directories(HoverX PUBLIC /usr/include)
target_include_directories(HoverX PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(HoverX PUBLIC ${OPENGL_INCLUDE_DIR})

target_link_libraries(HoverX ${SDL2_LIBRARIES})
target_link_libraries(HoverX ${OPENGL_LIBRARIES})
target_link_libraries(HoverX LibXml2)


