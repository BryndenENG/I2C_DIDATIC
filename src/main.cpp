/**********************************************************************************************************************
 * PROGRAMA EFETUA A LEITURA DE TODAS AS POSIÇÕES DE MEMORIA E EFETUA A A LEITURA E ESCRITA EM TODAS AS POSIÇÕES 
 * COM O VALOR 0xFF
 * AUTOR: ENG. TARCISIO SOUZA DE MELO
 * DATA: 17-12-2019
 **********************************************************************************************************************/


#include <Arduino.h>
//---CONSTANTES----------------------------------------------------------------------------------------------------------
#define CLK 6
#define SDA 5
//---FUNÇÕES-------------------------------------------------------------------------------------------------------------
void CLK_LOW();
void CLK_HIGH();
void SDA_LOW();
void SDA_HIGH();
void Start_Condition();
void Stop_Condition();
void Send_Bit(bool _bit);
bool Ack();
void NoAck();
void Send_Write(byte _byte);
bool Write_Men(byte _addres, byte _addressMen, byte _data);
bool Read_Men(byte _addres, byte _addressMen);
//---VARIAVEL_GLOBAL----------------------------------------------------------------------------------------------------
byte readMEN = 0x01;
byte val = 0;
void setup() 
{
  Serial.begin(9600);
}

void loop() 
{  
  /*
  bool resp = Write_Men(0xA0, 0x00, 0xFF);
  Serial.write(resp);
  for (int i = 0; i < 100; i++)
  {
    CLK_LOW();
    SDA_LOW();
  }
  */


  for (unsigned char pos_men = 0; pos_men < 0xFF; pos_men++)
  {
    Read_Men(0xA0, pos_men);
    Serial.write(readMEN);
    for (int z = 0; z < 100; z++)
    {
      CLK_HIGH();
      SDA_HIGH();
    }
  }

  for (unsigned char pos_men = 0; pos_men < 0xFF; pos_men++)
  {
    Read_Men(0xA2, pos_men);
    Serial.write(readMEN);
    for (int z = 0; z < 100; z++)
    {
      CLK_HIGH();
      SDA_HIGH();
    }
  }

  /*
  Read_Men(0xA0, 0x00);
  Serial.write(readMEN);
  for (int z = 0; z < 100; z++)
  {
        CLK_HIGH();
        SDA_HIGH();
  }
  */

  while(1)
  {
  }
  
  
}
//---FUNÇÃO_CLOCK_LOW--------------------------------------------------------------------------------------------------
void CLK_LOW()
{
  pinMode(CLK,OUTPUT);
  digitalWrite(CLK,LOW);
}
//---FUNÇÃO_CLOCK_HIGH-------------------------------------------------------------------------------------------------
void CLK_HIGH()
{
  pinMode(CLK,INPUT);
}
//---FUNÇÃO_DATA_LOW---------------------------------------------------------------------------------------------------
void SDA_LOW()
{
  pinMode(SDA,OUTPUT);
  digitalWrite(SDA,LOW);
}
//---FUNÇÃO_DATA_HIGH--------------------------------------------------------------------------------------------------
void SDA_HIGH()
{
  pinMode(SDA,INPUT);
}
//---FUNÇÃO_START_CONDITION--------------------------------------------------------------------------------------------
/*
 * CONDIÇÃO PARA INICIALIZAÇÃO DA COMUNICAÇÃO I2C. 
 */
void Start_Condition()
{
  CLK_LOW();
  SDA_LOW();
  CLK_HIGH();
  SDA_HIGH();
  SDA_LOW();
  CLK_LOW();
}
//---FUNÇÃO_STOP_CONDITION---------------------------------------------------------------------------------------------
/*
 * CONDIÇÃO PARA TERMINO  DA COMUNICAÇÃO I2C. 
 */
void Stop_Condition()
{
  CLK_LOW();
  SDA_LOW();
  CLK_HIGH();
  SDA_HIGH();
}
//---FUNÇÃO_ENVIO_DE_BIT------------------------------------------------------------------------------------------------
/*
 * CONDIÇÃO PARA O ENVIO DE UM UNICO BIT NA COMUNICAÇÃO I2C. 
 */
void Send_Bit(bool _bit)
{
  CLK_LOW();
  if (_bit)
  {
    SDA_HIGH();
  }else
  {
    SDA_LOW();
  }
  CLK_HIGH();
}
//---ACK-----------------------------------------------------------------------------------------------------------------
bool Ack()
{
  CLK_LOW();
  CLK_HIGH();
  return digitalRead(SDA);   
}
//---NoACK---------------------------------------------------------------------------------------------------------------
void NoAck()
{
  CLK_LOW();
  SDA_HIGH();
  CLK_HIGH();   
}
//---FUNÇÃO_ENVIO_DE_BYTE------------------------------------------------------------------------------------------------
void Send_Write(byte _byte)
{
  bool _bitSend = 0;
  for (byte i = 0; i < 8; i++)
  {
    _bitSend = (_byte >> 7) & 1;
    CLK_LOW();
    if (_bitSend)
    {
      Send_Bit(1);
    }else{
      Send_Bit(0);
    }
    CLK_HIGH();
    _byte <<= 1;
  }
}
//---FUNÇÃO_WRITE_MEN_I2C------------------------------------------------------------------------------------------------
bool Write_Men(byte _address, byte _addressMen, byte _data)
{
  bool writeOK = 0;
  while(1)
  {
    Start_Condition();
    Send_Write(_address);
    if(Ack())
    {
      writeOK = 0;
      break;
    }
    Send_Write(_addressMen);
    if(Ack())
    {
      writeOK = 0;
      break;
    }

    Send_Write(_data);
    if(Ack())
    {
      writeOK = 0;
    }else
    {
      writeOK = 1;
    }

    Stop_Condition();
    break; 
  }
  return writeOK;
}
//---FUNÇÃO_READ_MEN_I2C------------------------------------------------------------------------------------------------
bool Read_Men(byte _address, byte _addressMen)
{
  bool readOK = 1;
  bool bitRead = 0;

  readMEN = 0x00;
  while(1)
  {
    Start_Condition();
    Send_Write(_address);
    if(Ack())
    {
      readOK = 0;
      break;
    }
    Send_Write(_addressMen);
    if(Ack())
    {
      readOK = 0;
      break;
    }
    Start_Condition();
    _address |= 0x01;

    Send_Write(_address);
    if(Ack())
    {
      readOK = 0;
      break;
    }
    
    SDA_HIGH();
    for (int i = 0; i < 8; i++)
    {
      readMEN <<= 1;
      CLK_LOW();
      CLK_HIGH();
      bitRead = digitalRead(SDA);
      readMEN |= bitRead;
    }
    NoAck();
    Stop_Condition();
    break;
  }
  return readOK;
}