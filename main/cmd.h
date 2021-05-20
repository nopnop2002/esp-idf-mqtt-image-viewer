#define CMD_IMAGE	100

#define typeJpeg	1
#define typePng		2

typedef struct {
    uint16_t command;
	int imageType;
	size_t imageSize;
	char imageFile[64];
    TaskHandle_t taskHandle;
} CMD_t;
