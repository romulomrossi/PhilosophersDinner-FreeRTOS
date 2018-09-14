/* FreeRTOS.org includes. */
#include "FreeRTOS_AVR.h"
#include "basic_io_avr.h"


void vPhilosopher( void *pvPhilosopherId );
static void vInitializeForks();

/*-----------------------------------------------------------*/

#define N 5
#define LEFT_FORK iPhilosopherId 
#define RIGHT_FORK (iPhilosopherId + 1) % N

/* Declare a variable of type QueueHandle_t.  This is used to store the queue
that is accessed by all three tasks. */
QueueHandle_t xQueueForks[N];
int iPhilosopherIds[N];

void setup( void )
{
  Serial.begin(9600);
  
  for(int i = 0; i<N; i++)
  {  
    iPhilosopherIds[i] = i;
    xQueueForks[i] = xQueueCreate( 1, sizeof( int ) );
  }  
  
  for(int i=0; i<N; i++)
  {
    xTaskCreate( vPhilosopher, String(i).c_str(), 125, (void *)&iPhilosopherIds[i], 1, NULL ); 
  }
  
  vTaskStartScheduler();
  
  for( ;; );
}

void vPhilosopher( void *pvPhilosopherId )
{
  int iPhilosopherId = *((int*) pvPhilosopherId);
  const TickType_t xTicksToWait = 1000 / portTICK_PERIOD_MS;
 
  const int temp;
  
  BaseType_t xStatus;
  
  for(;;)
  { 
    vPrintStringAndNumber( "I'm thinking about [...] - Philosopher ", iPhilosopherId );
    
    vTaskDelay(xTicksToWait);

    vPrintStringAndNumber( "I'm so hungry, i will eat a little! - Philosopher ", iPhilosopherId );
    
    xStatus = xQueueSendToBack( xQueueForks[LEFT_FORK], &iPhilosopherId, xTicksToWait );

    if( xStatus != pdPASS )
    {
      vPrintStringAndNumber( "Oh, the left fork is busy! - Philosopher ", iPhilosopherId );
      continue;
    }
    
    xStatus = xQueueSendToBack( xQueueForks[RIGHT_FORK], &iPhilosopherId, xTicksToWait );
    
    if( xStatus != pdPASS)
    {
      vPrintStringAndNumber( "Oh, the right fork is busy! - Philosopher ", iPhilosopherId );
      xQueueReceive( xQueueForks[LEFT_FORK], temp, 0 );    
      continue;
    }
    
    vPrintStringAndNumber("I got the forks, i'll eat! Philosopher ", iPhilosopherId);
    
    vTaskDelay( xTicksToWait);
    
    xQueueReceive( xQueueForks[LEFT_FORK], &temp, xTicksToWait );    
    xQueueReceive( xQueueForks[RIGHT_FORK], &temp, xTicksToWait );    

    vPrintStringAndNumber("I'm satisfied, let's go back to think! Philosopher ", iPhilosopherId);
  }
}

void loop() {}
