using namespace std;

#define photoSensorPin A0 // пин цифрового входа (светоприемник)

#define SENSE 500
#define OUT 6

#define MESSAGE_END_GET 300    //понять, что сообщение кончилось
#define MIN_ZRO_GET 3    // время точки
#define MIN_ONE_GET 8   // время тире
#define MIN_TWO_GET 13
#define MIN_TIMEOUT_GET 5
 
#define PAUSE_SEND 4      // пауза между ../.-
#define MIN_ZRO_SEND 4    // время 0
#define MIN_ONE_SEND 9   // время 1
#define MIN_TWO_SEND 14
#define TIMEOUT_SEND 7    // A A

// #define MESSAGE_END_GET 500    //понять, что сообщение кончилось
// #define MIN_ZRO_GET 50    // время точки
// #define MIN_ONE_GET 100   // время тире
// #define MIN_TWO_GET 150
// #define MIN_TIMEOUT_GET 150
 
// #define PAUSE_SEND 50      // пауза между ../.-
// #define MIN_ZRO_SEND 75    // время 0
// #define MIN_ONE_SEND 125   // время 1
// #define MIN_TWO_SEND 175
// #define TIMEOUT_SEND 100    // A A

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);
  pinMode(photoSensorPin, INPUT);
  pinMode(OUT, OUTPUT);
  pinMode(13, OUTPUT);
}

String decode_table = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890`~!@$%^&*()_-=+<>?/|;:'.,[]{}#";

bool signal() {
  float current_light = analogRead(photoSensorPin);
  return current_light <= SENSE;
}


char decode(String s) {
  int c = 1, sum = 0;
  for (int i = s.length() - 1; i >= 0; i--) {
    if (s[i] == '1') {
      sum += c;
    } else if (s[i] == '2') {
      sum += c * 2;
    }
    c *= 3;
  }
  return decode_table[sum - 1];
}

String encode(char c) {
  int x = 1;
  while (decode_table[x - 1] != c && x < decode_table.length())
    x++;
  String ret;
  while (x > 0) {
    if (x % 3 == 0) {
      ret = '0' + ret;
    } else if (x % 3 == 1) {
      ret = '1' + ret;
    } else {
      ret = '2' + ret;
    }
    x /= 3;
  }
  return ret;
}


void loop() {
  if (signal()) {
    Serial.print("fren > ");                       //DESIGN
    while (true) {
      String binary_letter = "";

      while (true) {
        float signal_start = millis();
        while (signal()) {}
        float signal_end = millis();
        while (!signal() && millis() - signal_end <= MIN_TIMEOUT_GET) {}
        float timeout_end = millis();

        float signal_duration = signal_end - signal_start;
        float timeout_duration = timeout_end - signal_end;

        if (signal_duration >= MIN_ZRO_GET) {
          String simbol_part;
          if (signal_duration >= MIN_TWO_GET) {
            simbol_part = "2";
          } else if (signal_duration >= MIN_ONE_GET) {
            simbol_part = "1";
          } else {
            simbol_part = "0";
          }
          // Serial.println(signal_duration);        //DEBUG
          // Serial.println(timeout_duration);       //DEBUG
          binary_letter += simbol_part;
          if (timeout_duration >= MIN_TIMEOUT_GET) {
            Serial.print(decode(binary_letter));
            break;
          }
        }
      }
      float signal_end = millis();
      while (!signal() && millis() - signal_end <= MESSAGE_END_GET) {}
      if (millis() - signal_end >= MESSAGE_END_GET) {
        Serial.println();                           //DESIGN
        break;
      }
    }
  } else {
    if (Serial.available()) {
      String parsePacket = Serial.readString();
      Serial.print("me > ");                        //DESIGN
      Serial.println(parsePacket);                        //DESIGN
      for (byte i = 0; i < parsePacket.length(); i++) {
        String bin_symbol = encode(parsePacket[i]);
        for (byte j = 0; j < bin_symbol.length(); j++) {
          if (bin_symbol[j] == '0'){
            digitalWrite(OUT, 1);
            digitalWrite(13, 1);
            delay(MIN_ZRO_SEND);
            digitalWrite(OUT, 0);
            digitalWrite(13, 0);
          } else if (bin_symbol[j] == '1') {
            digitalWrite(OUT, 1);
            digitalWrite(13, 1);
            delay(MIN_ONE_SEND);
            digitalWrite(OUT, 0);
            digitalWrite(13, 0);
          } else {
            digitalWrite(OUT, 1);
            digitalWrite(13, 1);
            delay(MIN_TWO_SEND);
            digitalWrite(OUT, 0);
            digitalWrite(13, 0);
          }
          delay(PAUSE_SEND);
        }
        delay(TIMEOUT_SEND);
      }
    }
  }
}
