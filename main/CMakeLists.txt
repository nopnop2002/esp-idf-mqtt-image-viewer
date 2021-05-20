set(component_srcs "main.c"
	"ili9340.c"
	"fontx.c"
	"decode_jpeg.c"
	"decode_png.c"
	"pngle.c"
	"mqtt_sub.c"
	"tft.c"
	)

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")
