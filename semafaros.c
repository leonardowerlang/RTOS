#include <Arduino_FreeRTOS.h>
#include "semphr.h"

SemaphoreHandle_t mBarbeiro;
SemaphoreHandle_t mCortando;
SemaphoreHandle_t mCliente;

int cortando = 0;

void cortarCabelo(){
	xSemaphoreTake(mCortando, portMAX_DELAY);
  cortando = 1;
	xSemaphoreGive(mCortando);
  Serial.println("Cortando Cabelo");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  Serial.println("Acabou de cortar o cabelo");
	xSemaphoreTake(mCortando, portMAX_DELAY);
  cortando = 0;
	xSemaphoreGive(mCortando);
}


static void barbeiro(void* params){
  while(true){
  	xSemaphoreTake(mBarbeiro, portMAX_DELAY);
    if(xSemaphoreGive(mCliente) == pdPASS){
      cortarCabelo();
    }else{
      Serial.println("Barbeiro -> Barbeiro dormindo");
      xSemaphoreTake(mBarbeiro, portMAX_DELAY);
    }
  	xSemaphoreGive(mBarbeiro);
  }
}

static void cliente(void* params){
  while(true){
    Serial.println("Cliente -> Chegou");
		xSemaphoreTake(mCortando, portMAX_DELAY);
		int temp = cortando;
		xSemaphoreGive(mCortando);
    if(temp == 0){
		  Serial.println("Cliente -> Acordando o barbeiro");
		  xSemaphoreTake(mCliente, 0);
		  xSemaphoreGive(mBarbeiro);
    }else if(xSemaphoreTake(mCliente, 0) == pdPASS){
      Serial.println("Cliente -> Sala de espera");
    }else{
      Serial.println("Cliente -> Falou troxa");
    }
    vTaskDelay(random(10000) / portTICK_PERIOD_MS);
  }
}

void setup(){
  Serial.begin(9600);
  mBarbeiro = xSemaphoreCreateBinary();
	mCortando = xSemaphoreCreateMutex();
  mCliente = xSemaphoreCreateCounting(3, 3);

  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 1, NULL);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
