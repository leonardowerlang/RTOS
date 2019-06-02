#include <Arduino_FreeRTOS.h>
#include <event_groups.h>

EventGroupHandle_t clientes;

void cortarCabelo(){
  Serial.println("Cortando Cabelo");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  Serial.println("Acabou de cortar o cabelo");
}

static void barbeiro(void* params){
  while(true){
		int temp = xEventGroupWaitBits(clientes, B111, pdTRUE, pdFALSE, portMAX_DELAY);
		if(temp == B111){
		  xEventGroupSetBits(clientes, B011);
		}else if(temp == B011){
			xEventGroupSetBits(clientes, B001);
		}else if(temp == B001){
			xEventGroupSetBits(clientes, B000);
		}
    cortarCabelo();
  }
}

static void cliente(void* params){
  while(true){
		Serial.println("Cliente -> Chegou");
		int temp = xEventGroupWaitBits(clientes, B111, pdFALSE, pdFALSE, 0);
		if(temp <= B000){
		  xEventGroupSetBits(clientes, B001);
			Serial.println("Cliente -> Primeiro da fila");
		}else if(temp == B001){
			xEventGroupSetBits(clientes, B011);
			Serial.println("Cliente -> Segundo da fila");
		}else if(temp == B011){
			xEventGroupSetBits(clientes, B111);
			Serial.println("Cliente -> Ultimo da fila");
		}else{
      Serial.println("Cliente -> Falou troxa");
    }
    vTaskDelay(random(10000) / portTICK_PERIOD_MS);
  }
}

void setup(){
  Serial.begin(9600);

	clientes = xEventGroupCreate();

  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 1, NULL);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
