#include <TweetNaCl2.h>
TweetNaCl2 tuit;

 

     byte smWithoutZeros[105];
     //but if placed here, all is well
void setup() {
  
  byte publicArduinoSign[] 
 = { 0xb6,0xa6,0x83,0x9a,0x41,0x03,0x25,0xb1
   ,0xc8,0x04,0x33,0xc1,0x19,0x50,0xdd,0x26
   ,0x96,0x62,0xb1,0x67,0xe9,0x0b,0xa5,0x46
   ,0xe7,0x48,0x5f,0xa9,0x15,0x10,0x2e,0x51};

  unsigned char serversk[32] = {
 0x5d,0xab,0x08,0x7e,0x62,0x4a,0x8a,0x4b
,0x79,0xe1,0x7f,0x8b,0x83,0x80,0x0e,0xe6
,0x6f,0x3b,0xb1,0x29,0x26,0x18,0xb6,0xfd
,0x1c,0x2f,0x8b,0x27,0xff,0x88,0xe0,0xeb
} ;

unsigned char arduinopk[32] = {
 0x85,0x20,0xf0,0x09,0x89,0x30,0xa7,0x54
,0x74,0x8b,0x7d,0xdc,0xb4,0x3e,0xf7,0x5a
,0x0d,0xbf,0x3a,0x0d,0x26,0x38,0x1a,0xf4
,0xeb,0xa4,0xa9,0x8e,0xaa,0x9b,0x4e,0x6a
} ;

unsigned char nonce[24] = {
 0x69,0x69,0x6e,0xe9,0x55,0xb6,0x2b,0x73
,0xcd,0x62,0xbd,0xa8,0x75,0xfc,0x73,0xd6
,0x82,0x19,0xe0,0x03,0x6b,0x7a,0x0b,0x37
} ;

 const int sclen = 137;
 byte sc[sclen] = {
 0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0x59 ,0x6 ,0x25 ,0xCC ,0x32 ,0xF4 ,0x71 ,0xFE
 ,0xA9 ,0x5D ,0xA7 ,0x41 ,0xF8 ,0x88 ,0x88 ,0xBC
 ,0x23 ,0x39 ,0x7F ,0x9C ,0x7C ,0xE8 ,0x34 ,0x7D
 ,0x1E ,0x89 ,0x70 ,0x94 ,0xE6 ,0xE5 ,0xA4 ,0x78
 ,0xCE ,0xDF ,0x36 ,0x27 ,0x3 ,0xA3 ,0x81 ,0x1F
 ,0x7A ,0x32 ,0x44 ,0x85 ,0xFB ,0x11 ,0x46 ,0xB5
 ,0x14 ,0x5C ,0xE5 ,0x71 ,0xAF ,0x9D ,0xC9 ,0x3A
 ,0xEB ,0xDD ,0xE3 ,0x40 ,0x6 ,0x16 ,0x13 ,0x98
 ,0x49 ,0x68 ,0x1E ,0xC3 ,0x97 ,0x40 ,0x32 ,0x7
 ,0x28 ,0xA7 ,0xFF ,0xD8 ,0x6 ,0x71 ,0x8 ,0x1
 ,0xBA ,0xB5 ,0x6 ,0xC6 ,0x18 ,0xE1 ,0x3C ,0xE7
 ,0x77 ,0xD8 ,0x2C ,0x3A ,0xE9 ,0xD1 ,0xA6 ,0xF9
 ,0x72 ,0xD4 ,0x16 ,0x2 ,0x87 ,0xCB ,0xFE ,0x60
 ,0xBF ,0x21 ,0x30 ,0xFC ,0xA ,0x6F ,0xF6 ,0x4
 ,0x43 ,0x6B ,0x44 ,0x86 ,0xED ,0x86 ,0x90 ,0x45
 ,0x35
 };
 
 unsigned long long smlen = 137;
 byte sm[sclen];
 byte m[41];
int Suc_Crypt = 20;
int Suc_Sign = 20;
unsigned long long mlen;
int i;
  
  
  Serial.begin(9600);
  Serial.println("start");
  
Suc_Crypt = tuit.crypto_box_open(sm,sc,137,nonce,arduinopk,serversk);
Serial.println("Successful Decrypt: ");
Serial.print(Suc_Crypt);

for(i=0;i<137;i++){
       Serial.print(" ,0x");
       Serial.print(sm[i],HEX);
       if(i%8==7){
         Serial.println();
       }
     } 
     
     // if byte smWithoutZeros[105]; is placed then aign_open will face
     
     
Serial.println(" smwithoutzeros ");
for(i=0;i<105;i++){
   smWithoutZeros[i] = sm[i+32]; 
   Serial.print(" ,0x");
       Serial.print(smWithoutZeros[i],HEX);
       if(i%8==7){
         Serial.println();
       }
}
Serial.println(" ");
//for(i=0;i<107;i++){
//       Serial.print(" ,0x");
//       Serial.print(smWithoutZeros[i],HEX);
//       if(i%8==7){
//         Serial.println();
//       }
//     } 
//Serial.println(" ");

byte sme[64+41]
= {0x13 ,0xA7 ,0x1B ,0xC6 ,0x8 ,0x1 ,0xD4 ,0xDB
 ,0x13 ,0xB ,0x33 ,0x38 ,0x3F ,0xF2 ,0xDE ,0xCD
 ,0xD4 ,0xC4 ,0xDD ,0xAA ,0x59 ,0x8C ,0xDC ,0x6F
 ,0x76 ,0x3B ,0x78 ,0xDB ,0xAE ,0x94 ,0x11 ,0x23
 ,0x31 ,0x6F ,0x9E ,0xA2 ,0x4 ,0xFC ,0x54 ,0x5B
 ,0xBC ,0xBD ,0x3B ,0x85 ,0xB4 ,0x32 ,0xBB ,0xC3
 ,0x54 ,0x66 ,0xE0 ,0xCD ,0x2F ,0xE7 ,0xDC ,0x11
 ,0x12 ,0x1C ,0xFC ,0xAE ,0x54 ,0xEE ,0xC4 ,0x8
 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
 ,0xDE ,0x61 ,0x18 ,0xC ,0x6F ,0x72 ,0xB9 ,0x66
 ,0x2A };

Serial.print(sizeof(smWithoutZeros));

for(i=0;i<105;i++){
 if(sme[i] != smWithoutZeros[i]){
    Serial.print(i);
   Serial.println("notsame");  
 } 
}
if(sme != smWithoutZeros){
   //Serial.print("notsame"); 
}

Suc_Sign = tuit.crypto_sign_open(m, &mlen, smWithoutZeros,105,publicArduinoSign);

Serial.print("Successful Unsign: ");
Serial.println(Suc_Sign);

 for(i=0;i<41;i++){
       Serial.print(" ,0x");
       Serial.print(m[i],HEX);
       if(i%8==7){
         Serial.println();
       }
     } 

}

void loop() {
  // put your main code here, to run repeatedly:
// ,0xDB ,0x2E ,0x4E ,0xB4 ,0x29 ,0x33 ,0x9C ,0x9C
// ,0xC0 ,0x4D ,0xCA ,0x5C ,0x54 ,0x56 ,0x38 ,0xD
// ,0x75 ,0xAF ,0xFA ,0xC0 ,0x32 ,0x81 ,0xB9 ,0xFB
// ,0xC6 ,0xFC ,0x22 ,0x8B ,0xB1 ,0x8E ,0x73 ,0x64
// ,0x31 ,0x6F ,0x9E ,0xA2 ,0x4 ,0xFC ,0x54 ,0x5B
// ,0xBC ,0xBD ,0x3B ,0x85 ,0xB4 ,0x32 ,0xBB ,0xC3
// ,0x54 ,0x66 ,0xE0 ,0xCD ,0x2F ,0xE7 ,0xDC ,0x11
// ,0x12 ,0x1C ,0xFC ,0xAE ,0x54 ,0xEE ,0xC4 ,0x8
// ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
// ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
// ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
// ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0
// ,0xDE ,0x61 ,0x18 ,0xC ,0x6F ,0x72 ,0xB9 ,0x66
// ,0x2A 

}
