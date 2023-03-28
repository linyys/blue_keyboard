#include <Ticker.h>
#include <BleKeyboard.h>
#include <OneButton.h>
#include "EC11.hpp"
#include <U8g2lib.h>

#define EC11_A 25
#define EC11_B 26
#define EC11_Btn 27
#define SCL_Pin 19
#define SDA_Pin 18

BleKeyboard bleKeyboard;
EC11 encoder;
OneButton button(27, true);
Ticker timer1;
U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R2, /* clock=*/SCL_Pin, /* data=*/SDA_Pin, /* reset=*/U8X8_PIN_NONE);

int mode = 1;
bool flag = false;
void setup() {
  Serial.begin(115200);
  pinMode(EC11_A, INPUT_PULLUP);
  pinMode(EC11_B, INPUT_PULLUP);
  button.attachClick(onClick);
  button.attachDoubleClick(doubleClick);
  button.attachLongPressStop(mute);
  bleKeyboard.begin();
  u8g2.begin();
}

void loop() {
  button.tick();
  EC11Event e;
  if (encoder.read(&e) && bleKeyboard.isConnected()) {
    flag = true;
    timer1.once(3, change_flag);
    Serial.print(digitalRead(EC11_A));
    Serial.print(digitalRead(EC11_B));
    bool typeE = e.type == EC11Event::StepCW;
    switch (mode) {
      case 1:
        {
          if (typeE) {
            bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
          } else {
            bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
          }
        }
        break;
      case 2:
        {
          if (typeE) {
            bleKeyboard.write(KEY_MEDIA_NEXT_TRACK);
          } else {
            bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK);
          }
        }
        break;
    }
  }
  for (int i = 0; i < 10; i++) {
    encoder.checkPins(digitalRead(EC11_A), digitalRead(EC11_B));
    delay(1);
  };
  show();
}

void onClick() {
  mode == 2 ? mode = 1 : mode++;
}

void doubleClick() {
  bleKeyboard.write(KEY_RETURN);
  delay(2000);
  bleKeyboard.print("l123");
  delay(500);
  bleKeyboard.write(KEY_RETURN);
}

void mute() {
  if (mode == 1) {
    bleKeyboard.write(KEY_MEDIA_MUTE);
  } else if (mode == 2) {
    bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE);
  }
}

void show() {
  if (flag) {
    return;
  }
  u8g2.firstPage();
  char mode_str[2];
  // do {
  //   u8g2.setFont(u8g2_font_unifont_t_chinese2);
  //   u8g2.drawStr(27, 51, "hello");
  //   u8g2.drawStr(80, 10, bleKeyboard.isConnected() ? "BLE" : "");
  // } while (u8g2.nextPage());
  itoa(mode, mode_str, 10);
  for (int i = 0; i < 8; i++) {
    u8g2.setFont(u8g2_font_unifont_t_chinese2);
    u8g2.drawStr(20, 10, "mode");
    u8g2.drawStr(52, 10, mode_str);
    u8g2.drawStr(27, 41, "Hello Rain");
    u8g2.drawStr(87, 10, bleKeyboard.isConnected() ? "BLE" : "");
    u8g2.nextPage();
  };
  flag = true;
  timer1.once(2, change_flag);
}

void change_flag() {
  flag = false;
}
