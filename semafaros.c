#include <Arduino_FreeRTOS.h>
#include "semphr.h"

SemaphoreHandle_t mBarbeiro;
SemaphoreHandle_t mCortando;
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
		xSemaphoreTake(mCortando, portMAX_DELAY);
			while(uxSemaphoreGetCount(mCadeiras) < 2){
				cortarCabelo();
				xSemaphoreGive(mCadeiras);
			}
		xSemaphoreGive(mCortando);
		vPrintString("Indo dormir");
		xSemaphoreTake(mBarbeiro, portMAX_DELAY);
	}
}

static void cliente(void* params){
  while(true){
		vTaskDelay(random(5000, 10000) / portTICK_PERIOD_MS);
		if(xSemaphoreTake(mCortando, 0) == pdFAIL){
			if(uxSemaphoreGetCount(mCadeiras) > 0){
				vPrintString("Cliente-> Fila!");
				xSemaphoreTake(mCadeiras, portMAX_DELAY);
			}else{
				vPrintString("Cliente-> Foi embora!");				
			}
		}else{
			xSemaphoreGive(mCortando);
			vPrintString("Cliente-> Acordando o barbeiro!");
			xSemaphoreTake(mCadeiras, portMAX_DELAY);
			xSemaphoreGive(mBarbeiro);
		}
  }
}

void setup(){
  Serial.begin(9600);
  mBarbeiro = xSemaphoreCreateBinary();
	mCortando = xSemaphoreCreateMutex();
  mCadeiras = xSemaphoreCreateCounting(2, 2);

  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 2, NULL);
  xTaskCreate(cliente, "Cliente1", 100, NULL, 1, NULL);
	xTaskCreate(cliente, "Cliente2", 100, NULL, 1, NULL);
	xTaskCreate(cliente, "Cliente3", 100, NULL, 1, NULL);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
