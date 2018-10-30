
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

int voltaj = 2;
WidgetLED led1(V0);
BlynkTimer timer;
WidgetBridge bridge1(V4);

char auth[] = "............"; // ESP-12 TOKEN


int sanalbuton1;        // V1
int sanaldeger;         //V2
bool bildirim = false;
int bildirimsayaci = 0;
bool wifidurum = true;
int tur = 0;
int sayac = 0;
int sayici = 0;
int deger ;
bool iptal = false;
bool brigde = false;



// Büro Kablosuz Ağ bilgileri

char ssid[] = "..............";
char pass[] = "..............";




void setup()
{
  Serial.begin(9600);
  WiFi.begin (ssid, pass);
  Blynk.begin(auth, ssid, pass);
  pinMode (voltaj, INPUT_PULLUP);
  timer.setInterval(1000L, acilis);
  timer.setInterval(1000L, blynkAnotherDevice);
}



BLYNK_WRITE(V1)                 // Bildirim Butonu
{
  sanalbuton1 = param.asInt();
  if (sanalbuton1 == 1)
  {
    bildirim = true;
  }

  else
  {
    bildirim = false;
  }
}



//BLYNK_WRITE(V2)                      // IFTT LOCATION FONKSIYONU İÇİN - Telefon uygulamasına buton eklenmesine gerek yok
//{
//  if (deger == 1)                     // Bilgisayar kapalıyken
//  {
//    if (param.asInt() == 1)            // IFTT'den mevki algılandığında 1 gönder,
//    {
//      digitalWrite(3, HIGH);           // Bilgisayarı Aç ( Burada hangi pini konrtol etmek istiyorsak onu yazmalıyız. )
//      timer.setTimeout(1000L, []() {   // 1 saniye sonra timeout olsun ve
//        digitalWrite(3, LOW);          // BUTONU BIRAK
//      });
//    }
//    bridge = true;
//  }
//}




BLYNK_WRITE(V2)                     // IFTT LOCATION FONKSIYONU İÇİN - Telefon uygulamasına buton eklenmesine gerek yok
{
  if (deger == 1)                   // Bilgisayar kapalıyken
  {
    if (sanaldeger == 1)            // IFTT'den mevki algılandığında 1 gönder,
    {
      Blynk.notify("BILGISAYAR ve TV 2 DAKIKA ICINDA ACILACAK");
      while (sanaldeger)
      {
        sayici++;
        yield();
        if ( sayici == 1000 * 120 )
        {
          sanaldeger = 0;
          sayici = 0;
          bridge = true;
          blynkAnotherDevice();
        }
      }
    }
  }
}






BLYNK_WRITE(V3)                      // IFTT WIDGET BUTONU - Telefon uygulamasına buton eklenmesine gerek yok
{
  if (param.asInt() == 1)            // IFTT'den 1 geldiğinde
  {
    digitalWrite(3, HIGH);           // BUTONA BAS ( Burada hangi pini konrtol etmek istiyorsak onu yazmalıyız. )
    timer.setTimeout(1000L, []() {   // 1 saniye sonra timeout olsun ve
      digitalWrite(3, LOW);          // BUTONU BIRAK
    });
  }
}


BLYNK_WRITE(V5)                     // İPTAL BUTONU
{
  if (param.asInt() == 1)
  {
    iptal = true;
    sanaldeger = 0;
    sayici = 0;
  }
  else
  {
    iptal = false;
  }
}




void blynkAnotherDevice()           // Here we will send HIGH or LOW once per second
{
  if (bridge && iptal == false)
  {
    digitalWrite(3, HIGH);           // BUTONA BAS ( Burada hangi pini konrtol etmek istiyorsak onu yazmalıyız. )
    timer.setTimeout(1000L, []() {   // 1 saniye sonra timeout olsun ve
      digitalWrite(3, LOW);          // BUTONU BIRAK
    });

    bridge1.digitalWrite(0, HIGH);
    delay(1000);
    bridge1.digitalWrite(1, LOW);
    bridge = false;
  }
}




void acilis()
{
  if (tur <= 10)
  {
    tur++;
  }

  if (bildirimsayaci == 0 && bildirim == true)
  {
    Blynk.notify("PC Butonu AKTIF");
    bildirimsayaci = 1;
  }

  if (wifidurum)
  {
    deger = digitalRead(voltaj);
    if (deger == 0)
    {
      led1.on();
      if (sayac == 0)
      {
        Blynk.notify("BILGISAYAR ACILDI");
        sayac = 1;
      }
    }
    else
    {
      led1.off();
      sayac = 0;
    }
  }

  if (tur > 10)
  {
    if (bildirimsayaci == 0 && !bildirim)
    {
      bildirimsayaci = 1;
    }
  }
}




BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V1);                                       // Bildirim butonunun senkronu
  bridge1.setAuthToken("348a91cd2c1d4a08b64d0be277a32c5b");   // Kumanda TOKEN'i
}





void loop()
{
  if (Blynk.connected())
  {
    Blynk.run();
    timer.run();
  }

  else
  {
    wifidurum = false;
    kontrol ();
  }
}



void kontrol()
{
  if (!wifidurum)
  {
    while ( WiFi.status () != WL_CONNECTED )
    {
      Serial.println ("wifi koptu");
      delay (500);
    }
    Blynk.begin(auth, ssid, pass);
    if ( bildirim == true)
      Blynk.notify("PC BUTONU YENIDEN BASLADI");
  }
  wifidurum = true;
}
