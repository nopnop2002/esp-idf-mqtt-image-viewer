set(srcs "main.c"
	"ili9340.c"
	"fontx.c"
	"decode_png.c"
	"pngle.c"
	"mqtt_sub.c"
	"tft.c"
	)


# tjpgd library does not exist in ESP32-S2/C2 ROM.
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs "decode_jpeg.c")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_JPG")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_PNG")
endif()

if(IDF_TARGET STREQUAL "esp32s2")
	message(WARNING "*** JPEG NOT SUPPORTED ***")
	message(WARNING "*** JPEG NOT SUPPORTED ***")
endif()

if(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND srcs "decode_jpeg.c")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_JPG")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_PNG")
endif()

if(IDF_TARGET STREQUAL "esp32c3")
    list(APPEND srcs "decode_jpeg.c")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_JPG")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D ENABLE_PNG")
endif()

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")

