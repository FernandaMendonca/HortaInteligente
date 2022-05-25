
// Inclusão das bibliotecas

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Configuracoes da rede WiFi

const char* ssid = "2.4G_Felipe"; //SSID da rede WiFi
const char* password = "@@tricolor1020"; //senha da rede WiFi

const char* mqtt_server = "broker.hivemq.com"; //URL do broker MQTT
const int mqtt_server_port = 1883; //porta do broker MQTT

const char* mqtt_user = "41890280";              //user
const char* mqtt_password = "#v146?a32";      //password

String clientID = "ESP8266Client"; //identificacao do cliente

// Variaveis globais e objetos

#define MSG_BUFFER_SIZE (500) //define MSG_BUFFER_SIZE como 500
WiFiClient client; //cria o objeto client
PubSubClient mqtt_client(client); //cria o objeto mqtt_client
long lastMsg = 0;

// Variaveis tópicos

String topicoPrefixo = "MACK41890280"; //para o prefixo do topico, utilizar MACK seguido do TIA
String topicoTodos = topicoPrefixo + "/#"; //para retornar todos os topicos
String topico_0 = topicoPrefixo + "/testeIrrigador"; //topico para o sensor
String topico_1 = topicoPrefixo + "/sensor"; //topico para o sensor
String topico_2 = topicoPrefixo + "horta/bomba"; //topico para o atuador

#define pino_sinal_analogico A0
#define pino_bomba 1

int valor_analogico;
String strMSG = "0";
char mensagem[30];

void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  pinMode(pino_sinal_analogico, INPUT);
  pinMode(pino_bomba, OUTPUT);
  setup_wifi();

  mqtt_client.setServer(mqtt_server, mqtt_server_port);
  mqtt_client.setCallback(callback);

}

void callback(char* topic, byte* payload, unsigned int length) {

  payload[length] = '\0';
  strMSG = String((char*)payload);

  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);

}

void reconnect() {
  while (!mqtt_client.connected()) {
    Serial.print("Tentando conectar ao servidor MQTT");

    bool conectado = strlen(mqtt_user) > 0 ?
                     mqtt_client.connect(clientID.c_str(), mqtt_user, mqtt_password) :
                     mqtt_client.connect(clientID.c_str());

    if (conectado) {
      Serial.println("Conectado!");
      mqtt_client.subscribe(topico_0.c_str(), 1);
    } else {
      Serial.println("Falha durante a conexão. Error: ");
      Serial.print(mqtt_client.state());
      Serial.println("Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void loop() {

  if (!mqtt_client.connected()) {
    reconnect();
  }

  mqtt_client.loop(); // mantem a conexão com o MQTT
 
  valor_analogico = analogRead(pino_sinal_analogico);

  Serial.print("Porta analogica: ");
  Serial.print(valor_analogico);

    if (valor_analogico >= 0 && valor_analogico < 500){
      Serial.println("Status: Solo seco");
    
      digitalWrite (pino_bomba, HIGH);

      sprintf(mensagem, "0");
      Serial.print("Mensagem enviada: ");
      Serial.println(mensagem);
      mqtt_client.publish(topico_2.c_str(), mensagem);
      Serial.println("Sucesso");
    }

    if (valor_analogico >= 500 && valor_analogico <= 1024){
      Serial.println("Status: Solo úmido");
      
      digitalWrite (pino_bomba, LOW);
  
      sprintf(mensagem, "1");
      Serial.print("Mensagem enviada: ");
      Serial.println(mensagem); 
      
      mqtt_client.publish(topico_2.c_str(), mensagem);
      Serial.println("Sucesso");
    }
  delay(5000);
}
