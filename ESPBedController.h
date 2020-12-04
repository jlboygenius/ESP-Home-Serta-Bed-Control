#include "esphome.h"
#include "cc2500_REG.h"
#include "cc2500_VAL.h"

#include <SPI.h>

 
 #define CC2500_IDLE    0x36      // Exit RX / TX, turn
#define CC2500_TX      0x35      // Enable TX. If in RX state, only enable TX if CCA passes
#define CC2500_RX      0x34      // Enable RX. Perform calibration if enabled
#define CC2500_FTX     0x3B      // Flush the TX FIFO buffer. Only issue SFTX in IDLE or TXFIFO_UNDERFLOW states
#define CC2500_FRX     0x3A      // Flush the RX FIFO buffer. Only issue SFRX in IDLE or RXFIFO_OVERFLOW states
#define CC2500_SWOR    0x38
#define CC2500_TXFIFO  0x3F
#define CC2500_RXFIFO  0x3F
#define ONBOARD_LED  15

#define TX_TIMEOUT 10 // Timeouts are added
long previousTXTimeoutMillis = 0;
long previousMillis = 0;
long sendInterval = 2000; // in milliseconds

#define SCK_PIN 12
#define MISO_PIN 14
#define MOSI_PIN 13
#define CS_PIN 27

//int SS = CS_PIN;

int REPEATTIMES = 50; 

int isSetup =0;

class CCBedControl {
    public:
       
       	void setup()  {
            SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
            //SPI.pins;
            init_CC2500();
 
            pinMode(ONBOARD_LED,OUTPUT);
             ESP_LOGD("custom", "setting up Things for stuff!");
             isSetup=1;
             
        }

    
       
        void sendPacket(char commandCode) {
          
          WriteReg(REG_IOCFG1,0x06);
          // Make sure that the radio is in IDLE state before flushing the FIFO
          SendStrobe(CC2500_IDLE);
          // Flush TX FIFO
          SendStrobe(CC2500_FTX);
          // prepare Packet
          int length = 3;
          unsigned char packet[length];
          // First Byte = Length Of Packet
          //packet[0] = length;
          packet[0] = 0x02; // packet length is 2 
          packet[1] = 0x5F; // same for all packets
          packet[2] = commandCode; // keypress code

          
          // SIDLE: exit RX/TX
          SendStrobe(CC2500_IDLE);
          
          Serial.println("Transmitting ");
          for(int i = 0; i < length; i++)
          {	  
            // SendStrobe(packet[i]);
              WriteReg(CC2500_TXFIFO,packet[i]);
          }
          // STX: enable TX
          SendStrobe(CC2500_TX);
          // Wait for GDO0 to be set -> sync transmitted
          previousTXTimeoutMillis = millis();
          while (!digitalRead(MISO) && (millis() - previousTXTimeoutMillis) <= TX_TIMEOUT) {
             //Serial.println("GDO0 = 0");
          }
           
          // Wait for GDO0 to be cleared -> end of packet
          previousTXTimeoutMillis = millis();
          while (digitalRead(MISO) && (millis() - previousTXTimeoutMillis) <= TX_TIMEOUT) {
              //Serial.println("GDO0 = 1");
          }
          Serial.println("Finished sending");
          SendStrobe(CC2500_IDLE);
        }
    
        void WriteReg(char addr, char value){
          digitalWrite(CS_PIN,LOW);
          
          while (digitalRead(MISO) == HIGH) {
            };
            
          SPI.transfer(addr);
          delay(10);
          SPI.transfer(value);
          digitalWrite(CS_PIN,HIGH);
        }

        char ReadReg(char addr){
          addr = addr + 0x80;
          digitalWrite(CS_PIN,LOW);
          while (digitalRead(MISO) == HIGH) {
            };
          char x = SPI.transfer(addr);
          delay(10);
          char y = SPI.transfer(0);
          digitalWrite(CS_PIN,HIGH);
          return y;  
        }

        char SendStrobe(char strobe){
          digitalWrite(CS_PIN,LOW);
         
            
          while (digitalRead(MISO) == HIGH) {
          };
            
          char result =  SPI.transfer(strobe);
          digitalWrite(CS_PIN,HIGH);
          
          delay(10);
          return result;
        }
        void SendEnd(){
          SendStrobe(0x39);
        }
            
        void SendCommand(char BedCommand){
          digitalWrite(ONBOARD_LED,HIGH);
          SendStrobe(0x7E); // Start
          SendStrobe(0xFE); // 8 packetes
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 
          SendStrobe(0xFE); 

          SendStrobe(0x7F);
          SendStrobe(0x02);  
          SendStrobe(0x5F); 
          SendStrobe(BedCommand); 
          SendStrobe(REG_MARCSTATE); 
          digitalWrite(ONBOARD_LED,LOW);
        }

        void init_CC2500(){
          SendStrobe(0x30); 
          delay(10);
          WriteReg(REG_PARTNUM,0x00);
          WriteReg(REG_VERSION,0xFF);
          
        //  Serial.println("REG_IOCFG0 ");
          WriteReg(REG_IOCFG0,0x06); //02 06
          
        //  Serial.println("REG_PKTLEN ");
          WriteReg(REG_PKTLEN,0x03); //06 03
          
        //  Serial.println("REG_PKTCTRL1 ");
          WriteReg(REG_PKTCTRL1,0x84);
        //  Serial.println("REG_PKTCTRL0 ");
          WriteReg(REG_PKTCTRL0, 0x04);
        //  Serial.println("REG_FSCTRL1 ");
          WriteReg(REG_FSCTRL1,0x09);
        //  Serial.println("REG_FREQ2 ");
          WriteReg(REG_FREQ2,0x5C);
        //  Serial.println("REG_FREQ1 ");
          WriteReg(REG_FREQ1,0x4E);
        //  Serial.println("REG_FREQ0 ");
          WriteReg(REG_FREQ0,0xC4);
        //  Serial.println("REG_MD ");
          WriteReg(REG_MDMCFG4,0x2A);
        //  Serial.println("REG_MDMCFG3 ");
          WriteReg(REG_MDMCFG3,0x3B);
        ///  Serial.println("REG_MDMCFG2 ");
          WriteReg(REG_MDMCFG2,0x73);
        //  Serial.println("REG_MDMCFG1 ");
          WriteReg(REG_MDMCFG1,0xD2);
        //  Serial.println("REG_MDMCFG0 ");
          WriteReg(REG_MDMCFG0,0xF8);
        //  Serial.println("REG_DEVIATN ");
          WriteReg(REG_DEVIATN,0x01);
        //  Serial.println("REG_MCSM2 ");
          WriteReg(REG_MCSM2,0x07);
        //  Serial.println("REG_MCSM1 ");
          WriteReg(REG_MCSM1,0x30);
        //  Serial.println("REG_MCSM0 ");
          WriteReg(REG_MCSM0,0x18);
        //  Serial.println("REG_FOCCFG ");
          WriteReg(REG_FOCCFG,0x1D);
        //  Serial.println("REG_BSCFG ");
          WriteReg(REG_BSCFG,0x1C);
        //  Serial.println("REG_AGCCTRL2 ");
          WriteReg(REG_AGCCTRL2,0xC7);
        //  Serial.println("REG_AGCCTRL1 ");
          WriteReg(REG_AGCCTRL1,0x00);
        //  Serial.println("REG_AGCCTRL0 ");
          WriteReg(REG_AGCCTRL0,0xB2);
        //  Serial.println("REG_FREND1 ");
          WriteReg(REG_FREND1,0xB6);
        //  Serial.println("REG_FREND0 ");
          WriteReg(REG_FREND0,0x10);
        //  Serial.println("REG_FSCAL3 ");
          WriteReg(REG_FSCAL3,0xCA);
        //  Serial.println("REG_FSCAL0 ");
          WriteReg(REG_FSCAL0,0x11);
        //  Serial.println("REG_SYNC1 ");
          WriteReg(REG_SYNC1,0x7A);
        //  Serial.println("REG_SYNC0 ");
          WriteReg(REG_SYNC0,0xA7);
        //  Serial.println("REG_CHANNR ");
          WriteReg(REG_CHANNR,0x00);
          

            SendStrobe(0x7E); // Start
            SendStrobe(0xFE); // 8 packetes
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            SendStrobe(0xFE); 
            
            SendStrobe(0x3B); 
            



        }
            
        void ExecBedCmd(String command) {
             
            
            if (command=="FLAT"){
              sendPacket(BED_FLAT);
            }
            if (command=="TVPC"){
              sendPacket(BED_TVPC);
            }
            if (command=="LOUNGE"){
              sendPacket(BED_LOUNGE);
            }
            if (command=="ZEROG"){
              sendPacket(BED_ZEROG);
            }
            if (command=="HEADUP"){
              for(int i=0;i<REPEATTIMES;i++){sendPacket(BED_HEADUP);}
              
            }
            if (command=="HEADDOWN"){
              for(int i=0;i<REPEATTIMES;i++){sendPacket(BED_HEADDOWN);}
              
            }
            if (command=="FOOTUP"){
                for(int i=0;i<REPEATTIMES;i++){sendPacket(BED_FOOTUP);}
              
               
            }
            if (command=="FOOTDOWN"){
              for(int i=0;i<REPEATTIMES;i++){sendPacket(BED_FOOTDOWN);}
               
            } 
            
            digitalWrite(ONBOARD_LED, HIGH);
            delay(50);
            digitalWrite(ONBOARD_LED, LOW);
             delay(50);
            digitalWrite(ONBOARD_LED, HIGH);
            delay(50);
            digitalWrite(ONBOARD_LED, LOW);
            delay(50);
             digitalWrite(ONBOARD_LED, HIGH);
            delay(50);
            digitalWrite(ONBOARD_LED, LOW);
             delay(50);
            digitalWrite(ONBOARD_LED, HIGH);
            delay(50);
            digitalWrite(ONBOARD_LED, LOW);
            
        }

        void listenForPacket() {
          SendStrobe(CC2500_RX);
             delay(250);
                digitalWrite(ONBOARD_LED, HIGH);
                delay(20);
                digitalWrite(ONBOARD_LED, LOW);
          // Switch MISO to output if a packet has been received or not
          WriteReg(REG_IOCFG1,0x01);
          delay(20);
          unsigned long currentMillis = millis();
          if (digitalRead(MISO)) {      
            char PacketLength = ReadReg(CC2500_RXFIFO);
            char recvPacket[PacketLength];
             
          
            if(PacketLength >= -550) {
              //Serial.println("Packet Received!");
              //Serial.print("Packet Length: ");
              //Serial.println(PacketLength, DEC);
              Serial.print("Data: '");
              for(int i = 0; i < PacketLength; i++){
                recvPacket[i] = ReadReg(CC2500_RXFIFO);
                Serial.print(recvPacket[i], HEX);
                Serial.print("_");
              }
              Serial.println("' ");
              // Print quality information
              byte rssi = ReadReg(CC2500_RXFIFO);
              byte lqi = ReadReg(CC2500_RXFIFO);
              Serial.print("RSSI: ");
              Serial.println(rssi);
              Serial.print("LQI: ");
              Serial.println(lqi);
            }
            
            // Make sure that the radio is in IDLE state before flushing the FIFO
            // (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point) 
            SendStrobe(CC2500_IDLE);
            // Flush RX FIFO
            SendStrobe(CC2500_FRX);
          } else {
            
          }
        }
 };
 
CCBedControl *BC = new CCBedControl();
 
 
class BedSwitch_Off:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
        BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.
      
        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("FLAT");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 
  
class BedSwitch_On:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
         

    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.
     
        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("LOUNGE");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 

class BedSwitch_HeadDown:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.
     
        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("HEADDOWN");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 

class BedSwitch_HeadUp:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.
   
        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("HEADUP");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 


class BedSwitch_FootDown:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.

        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("FOOTDOWN");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 

class BedSwitch_FootUp:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.

        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("FOOTUP");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 

class BedSwitch_TVPC:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
       // BC->setup();
        
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.
   
        ESP_LOGD("custom", "Switch commanded!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("TVPC");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 

class BedSwitch_ZeroG:   public Component , public Switch{
    public: 
    void setup () override{
        //SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
    ESP_LOGD("custom", "setting up BC!");
        BC->setup();
        pinMode(ONBOARD_LED,OUTPUT);
    ESP_LOGD("custom", "set up  BC!");
    }
    
    void write_state(bool state) override {
        // This will be called every time the user requests a state change.

        ESP_LOGD("custom", "Switch commanded ZEROG!");
        //digitalWrite(5, state);
        BC->ExecBedCmd("ZEROG");
        // Acknowledge new state by publishing it
        publish_state(!state);
    }
}; 