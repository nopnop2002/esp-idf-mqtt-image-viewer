set(srcs "main.c"
	"ili9340.c"
	"fontx.c"
	"decode_png.c"
	"decode_jpeg_v5.c"
	"pngle.c"
	"mqtt_sub.c"
	"tft.c"
	)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")

