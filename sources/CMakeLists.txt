if(USE_PRECOMPILED_HEADERS AND COMMAND target_precompile_headers)
	file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/precompiled_ambilightapp_headers.cpp "void precompiledHeadersAmbilightAPP(){};")
	add_library(precompiled_ambilightapp_headers ${CMAKE_CURRENT_BINARY_DIR}/precompiled_ambilightapp_headers.cpp)
	target_link_libraries(precompiled_ambilightapp_headers Qt${Qt_VERSION}::Network)

    target_precompile_headers(precompiled_ambilightapp_headers PRIVATE ${CMAKE_SOURCE_DIR}/include/precompiled_ambilightapp_headers/AllHeaders_pch.h ${CMAKE_SOURCE_DIR}/include/precompiled_ambilightapp_headers/AllHdrHeaders_pch.h )
endif()

# Define the current source locations
SET(CURRENT_HEADER_DIR ${CMAKE_SOURCE_DIR}/include)
SET(CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/sources)

add_subdirectory(api)
add_subdirectory(base)
add_subdirectory(blackborder)
add_subdirectory(commandline)
add_subdirectory(db)
add_subdirectory(effectengine)
add_subdirectory(flatbuffers)
add_subdirectory(grabber)
add_subdirectory(ambilightimage)
add_subdirectory(jsonserver)
add_subdirectory(leddevice)
add_subdirectory(sound-capture)
add_subdirectory(ssdp)
add_subdirectory(suspend-handler)
add_subdirectory(systray)
add_subdirectory(utils)
add_subdirectory(webserver)

if (ENABLE_PROTOBUF)
	add_subdirectory(proto-nano-server)
endif()

if(ENABLE_BOBLIGHT)
	add_subdirectory(boblightserver)
endif()

if (ENABLE_CEC)
	add_subdirectory(cec)
endif()

if(ENABLE_BONJOUR)
	add_subdirectory(bonjour)
endif()

if(ENABLE_MQTT)
	add_subdirectory(mqtt)
endif()

# Executables
add_subdirectory(ambilightapp)


