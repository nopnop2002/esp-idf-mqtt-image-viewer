typedef struct {
	int sequence;
	int total_data_len;
	int current_data_offset;
	int topic_type;
	int topic_len;
	char topic[64];
	int data_len;
	char data[1024];
} MQTT_t;

