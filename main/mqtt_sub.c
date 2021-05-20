/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "esp_vfs.h"

#include "cmd.h"
#include "mqtt.h"

static const char *TAG = "MQTT";

QueueHandle_t xQueueSubscribe;
EventGroupHandle_t xEventGroupHandle;
extern QueueHandle_t xQueueCmd;

#define MQTT_CONNECTED_BIT BIT0

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	static int sequence = 0;
	// your_context_t *context = event->context;
	switch (event->event_id) {
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
			xEventGroupSetBits(xEventGroupHandle, MQTT_CONNECTED_BIT);
			break;
		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
			xEventGroupClearBits(xEventGroupHandle, MQTT_CONNECTED_BIT);
			break;
		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_UNSUBSCRIBED:
			ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
			break;
		case MQTT_EVENT_DATA:
			ESP_LOGD(TAG, "MQTT_EVENT_DATA");
			ESP_LOGD(TAG, "TOPIC=%.*s\r", event->topic_len, event->topic);
			//ESP_LOGI(TAG, "DATA=%.*s\r", event->data_len, event->data);
			ESP_LOGD(TAG, "event->topic_len=%d", event->topic_len);
			ESP_LOGD(TAG, "event->data_len=%d", event->data_len);
			ESP_LOGD(TAG, "event->total_data_len=%d", event->total_data_len);
			ESP_LOGD(TAG, "event->current_data_offset=%d", event->current_data_offset);
			
			MQTT_t mqttBuf;
			mqttBuf.topic_type = SUBSCRIBE;
			if (event->topic_len != 0) sequence = 0;
			mqttBuf.sequence = sequence;
			sequence++;
			mqttBuf.total_data_len = event->total_data_len;
			mqttBuf.current_data_offset = event->current_data_offset;
			mqttBuf.topic_len = event->topic_len;
			for(int i=0;i<event->topic_len;i++) {
				mqttBuf.topic[i] = event->topic[i];
				mqttBuf.topic[i+1] = 0;
			}
			mqttBuf.data_len = event->data_len;
			for(int i=0;i<event->data_len;i++) {
				mqttBuf.data[i] = event->data[i];
				mqttBuf.data[i+1] = 0;
			}
			//xQueueSend(xQueueSubscribe, &mqttBuf, 0);
			xQueueSend(xQueueSubscribe, &mqttBuf, portMAX_DELAY);
			break;
		case MQTT_EVENT_ERROR:
			ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
			break;
		default:
			ESP_LOGI(TAG, "Other event id:%d", event->event_id);
			break;
	}
	return ESP_OK;
}

#if 0
static void SPIFFS_Directory(char * path) {
	DIR* dir = opendir(path);
	assert(dir != NULL);
	while (true) {
		struct dirent *pe = readdir(dir);
		if (!pe) break;
		ESP_LOGI(__FUNCTION__,"d_name=%s d_ino=%d d_type=%x", pe->d_name,pe->d_ino, pe->d_type);
	}
	closedir(dir);
}
#endif

void mqtt_sub(void *pvParameters)
{
	ESP_LOGI(TAG, "Start Subscriber Broker:%s", CONFIG_BROKER_URL);

	/* Create Queue */
	//xQueueSubscribe = xQueueCreate( 20, sizeof(MQTT_t) );
	xQueueSubscribe = xQueueCreate( 10, sizeof(MQTT_t) );
	configASSERT( xQueueSubscribe );

	/* Create Eventgroup */
	xEventGroupHandle = xEventGroupCreate();
	configASSERT( xEventGroupHandle );
	xEventGroupClearBits(xEventGroupHandle, MQTT_CONNECTED_BIT);

	esp_mqtt_client_config_t mqtt_cfg = {
		.uri = CONFIG_BROKER_URL,
		.event_handle = mqtt_event_handler,
	};

	esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(mqtt_client);
	xEventGroupWaitBits(xEventGroupHandle, MQTT_CONNECTED_BIT, false, true, portMAX_DELAY);
	ESP_LOGI(TAG, "Connect to MQTT Server");

	esp_mqtt_client_subscribe(mqtt_client, CONFIG_MQTT_TOPIC, 0);
	MQTT_t mqttBuf;
	FILE* file = NULL;
	char markerJPEG[] = {0xFF, 0xD8};
	char markerPNG[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	char fileName[64];
	int total_data_len = 0;

	CMD_t cmdBuf;
	cmdBuf.command = CMD_IMAGE;
	cmdBuf.taskHandle = xTaskGetCurrentTaskHandle();

	while (1) {
		xQueueReceive(xQueueSubscribe, &mqttBuf, portMAX_DELAY);
		ESP_LOGD(TAG, "type=%d", mqttBuf.topic_type);

		if (mqttBuf.topic_type == SUBSCRIBE) {
			ESP_LOGD(TAG, "TOPIC=%.*s\r", mqttBuf.topic_len, mqttBuf.topic);
			ESP_LOGD(TAG, "DATA=%.*s\r", mqttBuf.data_len, mqttBuf.data);
			if (mqttBuf.current_data_offset == 0) {
				ESP_LOG_BUFFER_HEXDUMP(TAG, mqttBuf.data, 10, ESP_LOG_INFO);
				if (strncmp(mqttBuf.data, markerJPEG, sizeof(markerJPEG)) == 0) {
					ESP_LOGI(TAG, "JPEG file");
					//strcpy(fileName, "/spiffs/image.jpeg");
					sprintf(fileName, "/spiffs/image%d.jpeg",xTaskGetTickCount());
					//file = fopen("/spiffs/image.jpeg", "wb");
					file = fopen(fileName, "wb");
					if (file == NULL) {
						ESP_LOGE(TAG, "Failed to open file for writing");
					} else {
						cmdBuf.imageType = typeJpeg;
						total_data_len = mqttBuf.total_data_len;
						fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
					}
				} else if (strncmp(mqttBuf.data, markerPNG, sizeof(markerPNG)) == 0) {
					ESP_LOGI(TAG, "PNG file");
					//strcpy(fileName, "/spiffs/image.png");
					sprintf(fileName, "/spiffs/image%d.png",xTaskGetTickCount());
					//file = fopen("/spiffs/image.png", "wb");
					file = fopen(fileName, "wb");
					if (file == NULL) {
						ESP_LOGE(TAG, "Failed to open file for writing");
					} else {
						cmdBuf.imageType = typePng;
						total_data_len = mqttBuf.total_data_len;
						fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
					}
				} else {
					ESP_LOGI(TAG, "Unknown Image file type");
				}
			} else {
				if (file == NULL) continue;
				fwrite(mqttBuf.data, mqttBuf.data_len, 1, file);
				size_t received_data_size = mqttBuf.current_data_offset + mqttBuf.data_len;
				ESP_LOGI(TAG, "sequence=%d received_data_size=%d total_data_len=%d", mqttBuf.sequence, received_data_size, mqttBuf.total_data_len);
				if (mqttBuf.current_data_offset + mqttBuf.data_len == mqttBuf.total_data_len) {
					fclose(file);
					ESP_LOGI(TAG, "file close");

					// verify file size
					file = fopen(fileName, "rb");
					struct stat st;
					fstat(fileno(file), &st);
					ESP_LOGI(TAG, "%s st.st_size=%ld", fileName, st.st_size);
					fclose(file);
					file = NULL;

					if (st.st_size != total_data_len) {
						ESP_LOGE(TAG, "total_data_len miss match");
						unlink(fileName);
					} else {
						cmdBuf.imageSize = total_data_len;
						strcpy(cmdBuf.imageFile, fileName);
						if (xQueueSend(xQueueCmd, &cmdBuf, 10) != pdPASS) {
							ESP_LOGE(TAG, "xQueueSend fail");
						}
					}
				}

			}
		} else if (mqttBuf.topic_type == STOP) {
			ESP_LOGI(TAG, "Task Delete");
			esp_mqtt_client_stop(mqtt_client);
			vTaskDelete(NULL);
		}
	}

}
