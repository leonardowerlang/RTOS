#include <Arduino_FreeRTOS.h>
#include "semphr.h"

TaskHandle_t nClientes;
SemaphoreHandle_t mCadeiras;

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
  while(true){
  	ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
    cortarCabelo();
    xSemaphoreGive(mCadeiras);
  }
}

static void cliente(void* params){
  while(true){
    vTaskDelay(random(2000, 7500) / portTICK_PERIOD_MS);
    if(uxSemaphoreGetCount(mCadeiras) > 0){
      vPrintString("Cliente-> Fila!");
      xTaskNotifyGive(nClientes);
      xSemaphoreTake(mCadeiras, portMAX_DELAY);
    }else{
      vPrintString("Cliente-> Foi embora");
    }
  }
}

void setup(){
  Serial.begin(9600);
  mCadeiras = xSemaphoreCreateCounting(2, 2);

  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 2, &nClientes);
  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
