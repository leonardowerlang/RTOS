#include <Arduino_FreeRTOS.h>
#include <event_groups.h>
#include <queue.h>

QueueHandle_t cadeiras = NULL;
EventGroupHandle_t clientes;

void vPrintString(const char *pcString){
	vTaskSuspendAll();
	{
      Serial.println(pcString);
      Serial.flush();
	}
	xTaskResumeAll();
	if(Serial.available()){
		vTaskEndScheduler();
	}
}


void cortarCabelo(){
  vPrintString("Cortando Cabelo");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  vPrintString("Acabou de cortar o cabelo");
}

static void barbeiro(void* params){
  int id;
  while(true){
    if(xQueueReceive(cadeiras, &id, portMAX_DELAY) == pdPASS){
      cortarCabelo();
      xEventGroupSetBits(clientes, id);
    }
  }
}

static void cliente(void* params){
  int id = (int)params;
  while(true){
    vTaskDelay(random(2000, 5000) / portTICK_PERIOD_MS);
    if(xQueueSendToBack(cadeiras, &id, 0) == pdPASS){
      vPrintString("Cliente-> Fila!");
      xEventGroupWaitBits(clientes, id, pdTRUE, pdTRUE, portMAX_DELAY);
    }else{
      vPrintString("Cliente-> Foi embora!");
    }
  }
}

void setup(){
  Serial.begin(9600);

  clientes = xEventGroupCreate();
  cadeiras = xQueueCreate(2, sizeof(int));

  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, (void*)1, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, (void*)2, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, (void*)4, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, (void*)8, 1, NULL);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
