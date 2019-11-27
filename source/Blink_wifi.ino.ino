/*
lm35로 온도값 센싱
http를 이용하여 데이터 전송
@since
  20191024  init
*/


#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define Debug(s) Serial.print((s))
#define DebugLn(s) Serial.println((s))

//
const char* ssid = "AndroidHotspot1492";
const char* password = "";




//
WiFiClient client;
HTTPClient http;


/**
 * setup
 */
void setup() {
  myinit();

}


// the loop function runs over and over again forever
void loop() {
  DebugLn("");

  
  onLed();
  double tp = readTemp(); //온도값 읽기
  double hd = readHd(); //습도값 읽기
  double etc = readEtc(); //기타값 읽기
  offLed();
  
  //http로 전송
  onLed();
  sendData(tp, hd, etc);
  offLed();
  
  //delay 30sec
  delay(1000*30);
}



/**
 * 이것저것 초기화
 */
void myinit(){
  DebugLn("init");

  //
  Serial.begin(9600);

  //
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  //wifi 연결
  tryConnectToApIfNotConnected();
}


void scanWiFis(){
  DebugLn("scan wifi list");

  
  int networkCount = WiFi.scanNetworks();
  if(0 == networkCount){
    DebugLn("empty networks");
    return;
  }

  //
  for(int i=0; i<networkCount; i++){
    Debug(i);
    Debug("\t");
    DebugLn(WiFi.SSID(i));
  }
}

/**
 * 습도 값 읽기
 * @return {double} 습도값
 */
double readHd(){
  //TODO
  return 0.0;
}


/**
 * 기타 값 읽기
 * @return {double} 기타값
 */
double readEtc(){
  //TODO
  return 0.0;
}

/**
 * 온도 값 읽기
 * @return {double} 온도값
 */
double readTemp(){
  
  double analogValue = analogRead(A0);

  double analogVolts = (analogValue * 3.03) / 1024;
  //온도
  double tp = (analogVolts) * 100;

  Debug("tp:");
  Debug(tp);
  DebugLn("C");

  return tp;
}

/**
 * wifi 상태 표시
 */
void showWiFiStatus(){
    Debug("wifi status:");

    //
    switch(WiFi.status()){
      case WL_CONNECTED:
        Debug("connected ");
        Debug(WiFi.SSID());
        Debug(" ");
        Debug(WiFi.localIP());
        break;
      
      case WL_NO_SHIELD:
        Debug("no shield");
        break;
      
      case WL_IDLE_STATUS:
        Debug("idle status");
        break;
      case WL_NO_SSID_AVAIL:
        Debug("ssid avail");
        break;
      case WL_SCAN_COMPLETED:
        Debug("scan completed");
        break;
      case WL_CONNECT_FAILED:
        Debug("connect failed");
        break;
      case WL_CONNECTION_LOST:
        Debug("connection lost");
        break;
      case WL_DISCONNECTED:
        Debug("disconnected");
        break;
    }//switch

    //
    DebugLn("");
}

/**
 * ap 접속 시도
 * 접속 성공할때까지 계속 접속 시도
 * led는 0.5초 간격으로 깜빡거림
 */
boolean tryConnectToApIfNotConnected(){
  boolean b = true;

  //
  showWiFiStatus();

  if(WL_CONNECTED == WiFi.status()){
    return true;
  }

  //
  WiFi.begin(ssid, password);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  //WiFi.waitForConnectResult();

  //
  while(WL_CONNECTED != WiFi.status()){
    onLed();
    delay(500);
    offLed();
    delay(500);
    Debug(".");
  }

  //
  if(b){
    return b;
  }

  //
  DebugLn("WiFi connection false");
  return false;
}

/**
 * 
 */
void onLed(){
  digitalWrite(LED_BUILTIN, LOW); 
  //DebugLn("on");
}

/**
 * 
 */
void offLed(){
  digitalWrite(LED_BUILTIN, HIGH); 
  //DebugLn("off");
}

/**
 * http로 데이터 전송
 *  url의 parameterkey(key, temp, temp2, test)는 변경 불가
 *  key : apiKey를 보내기 위한 키
 *  temp, temp2, test : RIP에서 채널 등록시 사용한 항목 명
 * @param tp 온도
 * @param hd 습도
 * @param etc 기타
 */ 
void sendData(double tp, double hd, double etc){
  
  //
  boolean b = tryConnectToApIfNotConnected();

  //
  if(!b){
    return;
  }

  //변경 불가
  String apiKey = "w7bbe8a9ccf064133ba04a4fc43fba0ec";

  //변경 불가
  String url = "http://1.209.199.212:21882/colct/api/registData.do";
  url += "?key=" + apiKey;
  url += "&temp=" + String(tp); //건구온도
  url += "&temp2=" + String(hd);  //습도
  url += "&test=" + String(etc);  //기타
  
  http.begin(client, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  //get방식으로 호출
  int httpCode = http.GET(); 
  DebugLn(url);

  //
  Debug("http response code:");
  DebugLn(httpCode);
  if(0 > httpCode){    
    DebugLn(http.errorToString(httpCode).c_str());
  }
  
  //연결 닫기
  http.end(); 
}
