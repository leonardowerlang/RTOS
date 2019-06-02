#include <Arduino_FreeRTOS.h>

TaskHandle_t nClientes;
int clientes = 0;

void cortarCabelo(){
  Serial.println("Cortando Cabelo");
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  Serial.println("Acabou de cortar o cabelo");
}

static void barbeiro(void* params){
  while(true){
  	clientes = ulTaskNotifyTake(pdFALSE, portMAX_DELAY) - 1;
    cortarCabelo();
  }
}

static void cliente(void* params){
  while(true){
		Serial.println("Cliente -> Chegou");
		clientes++;
		Serial.println(clientes);
    if(clientes <= 3){
      xTaskNotifyGive(nClientes);
    }else{
      Serial.println("Cliente -> Falou troxa");
    }
    vTaskDelay(random(10000) / portTICK_PERIOD_MS);
  }
}

void setup(){
  Serial.begin(9600);

  xTaskCreate(cliente, "Cliente", 100, NULL, 1, NULL);
  xTaskCreate(barbeiro, "Barbeiro", 100, NULL, 1, &nClientes);
  vTaskStartScheduler();
  for(;;);
}

void loop(){}
