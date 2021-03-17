// Lorenzo Neri Arduino Day 2021


// Librerie necessarie per il funzionamento
// ArduinoJson versione 6
// Esp core versione 2.5.2


#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <WiFiClientSecure.h>
// Libreria disponibile al seguente link:
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>

const char* ssid = "NOME_RETE_WI-FI";
const char* password = "PASSWORD_RETE_WI-FI";


// Token del bot Telegram, quello che viene dato da Botfather
#define BOTtoken "INSERIRE_TOKEN"  

// chat_id della nostra chat con il nostro chatbot
#define CHAT_ID "INSERIRE_CHAT_ID"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Verifichiamo ogni secondo se riceviamo un messaggio 
int delayRichiestaBot = 1000;
unsigned long lastTimeBotRan;


bool statoLED = LOW;

// funzione per gestire ogni messaggio ricevuto e capire come dobbiamo comportarti
void gestisciMessaggioBot(int numNuovoMessaggio) {
  Serial.println("gestisciMessaggioBot");
  Serial.println(String(numNuovoMessaggio));

  for (int i=0; i<numNuovoMessaggio; i++) {
    
    // Otteniamo il chat_id del messaggio che stiamo attualmente ciclando
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Ciao non sei autorizzato ad usare i miei servizi!", "");
      continue;
    }
    
    // Stampiamo a monitor seriale il testo del messaggio che stiamo attualmente ciclando
    String text = bot.messages[i].text;
    Serial.println(text);

    // Otteniamo il nome utente dell'utente stesso che ha inviato il messaggio al bot
    String nome_utente = bot.messages[i].from_name;

    // Utente ha scritto il comando start
    if (text == "/start") {
      String messaggio_benvenuto = "Benvenuto, " + nome_utente + ".\n";
      messaggio_benvenuto += "Puoi usare i seguenti comandi per interagire con me.\n\n";
      messaggio_benvenuto += "/led_on serve ad accendere il LED presente sulla scheda\n";
      messaggio_benvenuto += "/led_off serve a spegnere il LED presente sulla scheda \n";
      messaggio_benvenuto += "/stato_led serve per capire in che stato si trova il LED\n";
      // Mandiamo il messaggio
      bot.sendMessage(chat_id, messaggio_benvenuto, "");
    }

    // Utente ha scritto il comando per accendere il LED
    if (text == "/led_on") {

      // Che accendiamo solo se è spento
      if (digitalRead(LED_BUILTIN) == LOW){
          String messaggio = nome_utente + " il LED è già acceso.\n\n";
          bot.sendMessage(chat_id, messaggio, "");  
      }
      else{
        statoLED = LOW;
        digitalWrite(LED_BUILTIN, statoLED);  
        String messaggio = nome_utente + " ho acceso il LED.\n\n";
        bot.sendMessage(chat_id, messaggio, "");  
      }
      
      
    }

    // Utente ha scritto il comando per spegnere il LED
    if (text == "/led_off") {
      // Che spegniamo solo se è acceso
       if (digitalRead(LED_BUILTIN) == HIGH){
            String messaggio = nome_utente + " il LED è già spento.\n\n";
            bot.sendMessage(chat_id, messaggio, "");  
        }
        else{
          statoLED = HIGH;
          digitalWrite(LED_BUILTIN, statoLED);  
          String messaggio = nome_utente + " ho SPENTO il LED.\n\n";
          bot.sendMessage(chat_id, messaggio, "");  
        }
    }

    // Utente ha scritto il comando per capire in che stato sia il LED
    if (text == "/stato_led") {
      String messaggio = nome_utente + " il LED è ";
      if (digitalRead(LED_BUILTIN)){
        messaggio += " spento.\n\n";
        bot.sendMessage(chat_id, messaggio, "");
      }
      else{
        messaggio += " acceso.\n\n";
        bot.sendMessage(chat_id, messaggio, "");
      }
    }

    // Diversamente l'utente ha scritto un messaggio che non può essere accettato 

    else{
      String messaggio = nome_utente + " mi dispiace, ma non riconsoco questo comando.";
      bot.sendMessage(chat_id, messaggio, "");
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    client.setInsecure();
  #endif

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, statoLED);
  
  // Ci connettiamo al Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Mi connetto..");
  }
   // Stampiamo a video l'indirizzo IP della scheda
  Serial.println(WiFi.localIP());
}

void loop() {
  // Se è passato un secondo dall'ultima lettura, procediamo a leggere i nuovi messaggi
  
  if (millis() > lastTimeBotRan + delayRichiestaBot)  {
    
    Serial.print("leggo I nuovi");
    
    int numNuovoMessaggio = bot.getUpdates(bot.last_message_received + 1);
    Serial.println(numNuovoMessaggio);
    
    while(numNuovoMessaggio) {
      
      Serial.println("Rispondo al messaggio ricevuto. ");
      gestisciMessaggioBot(numNuovoMessaggio);
      numNuovoMessaggio = bot.getUpdates(bot.last_message_received + 1);
    }

    // Aggiorno il tempo trascorso.
    lastTimeBotRan = millis();
  }
}
