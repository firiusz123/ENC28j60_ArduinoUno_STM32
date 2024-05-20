#include <EtherCard.h>
#include <IPAddress.h>
#include <SPI.h>

char payload[] = "PONGGG";
uint8_t ipDestinationAddress[IP_LEN];
char receivedData[50];
const int slaveSelectPin1 = 9; // SS pin of Arduino connected to NSS/CS pin of STM32
const int slaveSelectPin2 = 10; // SS pin of Arduino connected to NSS/CS pin of enc28j60
char  txMsg[50] ;
char  rxMsg[50] ;

#define STATIC 0  // set to 1 to disable DHCP (adjust myip/gwip values below)

#if STATIC
// Ethernet interface IP address
static byte myip[] = { 192, 168, 0, 200 };
// Gateway IP address
static byte gwip[] = { 192, 168, 0, 1 };
#endif



// Ethernet MAC address - must be unique on your network
static byte mymac[] = { 0x70, 0x69, 0x69, 0x2D, 0x30, 0x31 };

byte Ethernet::buffer[500]; // TCP/IP send and receive buffer

// the stm part ////////////////////////////////////////////////////////////////////

// This function sends the data but doesnt recieve anything. 
void STMsend(char *txMsg) {
  
  digitalWrite(slaveSelectPin1, LOW);
  for(int i = 0;txMsg[i] != '\0'; i++)
  {
     // Select STM32 as slave
    delay(1);
    SPI.transfer(txMsg[i]); // Send data to STM32
    
    
    
  }
  
  
    digitalWrite(slaveSelectPin1, HIGH); // Deselect STM32
}

// this function sends data and then recieves it 
//TODO entire function xd 

void STMsendReceive(char *txMsg) {
  digitalWrite(slaveSelectPin1, LOW);


  
  
  digitalWrite(slaveSelectPin1, HIGH); // Deselect STM32
}


// the stm part /////////////////////////////////////////////////////////////////////






// Callback that prints received packets to the serial port
void udpSerialPrint(uint16_t dest_port, uint8_t src_ip[IP_LEN], uint16_t src_port, const char *data, uint16_t len) {
  IPAddress src(src_ip[0], src_ip[1], src_ip[2], src_ip[3]);

  Serial.print("Received from ");
  ether.printIp(src_ip);
  Serial.print(":");
  Serial.print(src_port);
  Serial.print(" - ");
  Serial.println(data);
  strcpy(receivedData, data);

  // here you set what to sent to that bulshit

  STMsend(data);


  //!!!!!!!!!!!!!!!!!!!!!!!


  // Check if the received data is "ping"
  if (strcmp(data, "ping") == 0) {
      //STMsend(data);
    // Respond with "PONG" to the source IP and port
    Serial.println("japierdole czemu to sie nie drukuje ");
    ether.sendUdp(payload, sizeof(payload), 1337, src_ip, 1234);
  }
}

void setup() {
  // setting all the cs pins to high to ensure that all devices at beggining are not communicating 
  Serial.begin(57600);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
   pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);                                                                             
  
  
  Serial.println(F("\n[backSoon]"));
//////// SPI TRANSFER SPEEED //////////////////////
  
  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  //////// SPI TRANSFER SPEEED //////////////////////

  // Change 'SS' to your Slave Select pin if you aren't using the default pin
  if (ether.begin(sizeof Ethernet::buffer, mymac, SS) == 0)
    Serial.println(F("Failed to access Ethernet controller"));

  #if STATIC
    ether.staticSetup(myip, gwip);
  #else
    if (!ether.dhcpSetup())
      Serial.println(F("DHCP failed"));
  #endif

    ether.parseIp(ipDestinationAddress, "192.168.1.100"); // Change to your desired destination IP address

    ether.printIp("IP:  ", ether.myip);
    ether.printIp("GW:  ", ether.gwip);
    ether.printIp("DNS: ", ether.dnsip);

  // Register udpSerialPrint() to port 1337
    ether.udpServerListenOnPort(&udpSerialPrint, 1337);
}

void loop() {
  
  // This must be called for ethercard functions to work
  ether.packetLoop(ether.packetReceive());
  //if (strcmp(receivedData, "ping") == 0) {
    // If yes, print "cholera" to the serial monitor
    //ether.sendUdp(payload, sizeof(payload), 1337, ipDestinationAddress, 1234);
    //Serial.println("ping not pinging brain not braining");
  //}
}
