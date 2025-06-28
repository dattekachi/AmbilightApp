# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include/ambilightimage)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sources/ambilightimage)

FILE ( GLOB AMBILIGHTIMAGE_SOURCES "${CURRENT_HEADER_DIR}/*.h" )

list(APPEND AMBILIGHTIMAGE_SOURCES ${CURRENT_SOURCE_DIR}/AmbilightImage.cpp)

add_library(ambilightimage ${AMBILIGHTIMAGE_SOURCES} )

target_include_directories(ambilightimage PRIVATE ${CMAKE_SOURCE_DIR}/include/ambilightimage/stb)
target_include_directories(ambilightimage PRIVATE ${CMAKE_SOURCE_DIR}/external/lunasvg/3rdparty/stb)


target_link_libraries(ambilightimage
	ambilightapp-base
	${QT_LIBRARIES}
	lunasvg
	TurboJPEG::TurboJPEG
)
