#include <HardwareSerial.h>
HardwareSerial SerialTFMini( 1 );
HardwareSerial SerialBrain( 2 );
////// serial(1) = pin27=RX green, pin26=TX white
////// serial(2) = pin16=RXgreen , pin17=TX white
#define SerialDataBits 115200
void setup()
{
  SerialBrain.begin( SerialDataBits );
  SerialTFMini.begin(  SerialDataBits, SERIAL_8N1, 27, 26 );
  }
  void fSendLIDAR_InfoSerialToBrain( void *pvParameters )
{
  struct stu_LIDAR_INFO pxLIDAR_INFO;
  for ( ;; )
  {
    xEventGroupWaitBits (eg, evtGetIMU, pdTRUE, pdTRUE, portMAX_DELAY);
    xSemaphoreTake( sema_LIDAR_FOR_ALARM, xSemaphoreTicksToWait );
    xQueueReceive ( xQ_LIDAR_FOR_ALARM, &pxLIDAR_INFO, QueueReceiveDelayTime );
    xSemaphoreGive( sema_LIDAR_FOR_ALARM );
    int CellCount = 1 ;
   // send LIDAR info for alarm
    String sSerial = "";
    sSerial.reserve ( 300 );
    sSerial.concat( "<#," ); //sentence begin
    sSerial.concat( String(ScanPoints) + "," );
    sSerial.concat( String(pxLIDAR_INFO.ServoSweepUp) + "," ); // get direction of scan
    for ( CellCount; CellCount <= ScanPoints; CellCount++ )
    {
      sSerial.concat( String(pxLIDAR_INFO.Range[CellCount]) + "," );
    }
    sSerial.concat( ">" ); //sentence end
    vTaskDelay( 10 );
    SerialBrain.println ( sSerial );
  }
  vTaskDelete( NULL );
} // void fSendLIDAAR_InfoSerialToBrain( void *pvParameters )