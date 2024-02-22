#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/mqtt.h"
#include "lwip/apps/mqtt_priv.h"

#include <dht.h>

#define WIFI_SSID       "Pixel_5725A"
#define WIFI_PASSWORD   "82208220*Tt"

#define MQTT_SERVER_HOST    "localhost"
#define MQTT_SERVER_PORT    8883

#define MQTT_TOPIC      "pico_w/test"

static const dht_model_t DHT_MODEL = DHT11;
static const uint DATA_PIN = 0;

static float celsius_to_fahrenheit(float temperature) {
    return temperature * (9.0f / 5) + 32;
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
    if (status != 0) {
        printf("Error during connection: err %d.\n", status);
    } else {
        printf("MQTT connected.\n");
    }
}

void main() { 
    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return;
    }
    cyw43_arch_enable_sta_mode();

    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect.\n");
        return;
    } else {
        printf("Connected.\n");
    }

    mqtt_client_t *mqtt_client = mqtt_client_new();
    if (mqtt_client == NULL) {
        printf("Failed to create new mqtt client\n");
        return;
    } 

    struct mqtt_connect_client_info_t ci;
    memset(&ci, 0, sizeof(ci));
    ci.client_id = "PicoW";

    mqtt_connect_client_info_set_default(&ci);
    err_t err = mqtt_client_connect(mqtt_client, MQTT_SERVER_HOST, MQTT_SERVER_PORT, mqtt_connection_cb, NULL, &ci);
    if (err != ERR_OK) {
        printf("Failed to connect to MQTT broker, err %d\n", err);
        return;
    }

    printf("MQTT connection request sent.\n");

    sleep_ms(1000); // Wait for connection to establish (adjust as needed)

    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true /* pull_up */);

    while (true) {
        dht_start_measurement(&dht);
        
        float humidity;
        float temperature_c;
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);
        if (result == DHT_RESULT_OK) {
            char message[64];
            sprintf(message, "{\"temperature\":%.1f,\"humidity\":%.1f}", temperature_c, humidity);
            printf("Message to be published: %s\n", message);
            err = mqtt_publish(mqtt_client, MQTT_TOPIC, message, strlen(message), 0, 0, NULL, NULL);
            if (err != ERR_OK) {
                printf("Failed to publish message, err %d\n", err);
            } else {
                printf("Message published successfully.\n");
            }
        } else if (result == DHT_RESULT_TIMEOUT) {
            printf("DHT sensor not responding. Please check your wiring.\n");
        } else {
            printf("Bad checksum\n");
        }

        sleep_ms(2000); // Adjust as needed
    }
}
