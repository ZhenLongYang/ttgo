#include <TFT_eSPI.h>
#include <SPI.h>
#include "grab.h"
#include <WiFi.h>

const char *ssid = "233";
const char *password = "666";

const IPAddress serverIP(192,168,0,104); //欲访问的地址
uint16_t serverPort = 8080;         //服务器端口号

WiFiClient client; //声明一个客户端对象，用于与服务器进行连接

#ifndef TFT_DISPOFF
#define TFT_DISPOFF 0x28
#endif

#ifndef TFT_SLPIN
#define TFT_SLPIN   0x10
#endif

//#define TFT_MOSI            19
//#define TFT_SCLK            18
//#define TFT_CS              5
//#define TFT_DC              16
//#define TFT_RST             23
//#define TFT_BL          4  // Display backlight control pin
// int sw = 135;
// int sh = 240;
//TFT_eSPI tft = TFT_eSPI(135, 240); // Invoke custom library

//  #define TFT_MOSI            26
//  #define TFT_SCLK            27
//  #define TFT_CS              32
//  #define TFT_DC              33
//  #define TFT_RST             25
//  #define TFT_BL          0  // Display backlight control pin

 int sw = 80;
 int sh = 160;
 TFT_eSPI tft = TFT_eSPI(sw, sh); // Invoke custom library


//! Long time delay, it is recommended to use shallow sleep, which can effectively reduce the current consumption
void espDelay(int ms)
{   
    esp_sleep_enable_timer_wakeup(ms * 1000);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_ON);
    esp_light_sleep_start();
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Start");
    tft.init();
    tft.setCursor(0, 0);
    tft.setTextDatum(MC_DATUM);
    tft.setTextSize(1);

    tft.setRotation(3);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false); //关闭STA模式下wifi休眠，提高响应速度
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
    tft.pushImage(0, 0,  sh, sw, ttgo);
}

void loop()
{
    Serial.println("尝试访问服务器");
    if (client.connect(serverIP, serverPort)) //尝试访问目标地址
    {
        int flag = 1;
        int index = 0;
        int binData0 = 0;
        int binData1 = 0;
        int result = 0;
        Serial.println("访问成功");
        while (client.connected() || client.available()) //如果已连接或有收到的未读取的数据
        {
            if (client.available()) //如果有数据可读取
            {
                // binData1 = client.read();
                if(flag % 2 == 0)
                {
                    binData1 = client.read(); // 读取一个像素的色彩数据的第二字节， 这里只能一次读取一个字节， 而从服务端发送的数据是用一个16位数字表示1像素点的 rgb色彩的，所以就需要读两次才可以获取一个完成像素色彩
                    binData1 = binData1 << 8; // 左移八位
                    result = binData0 | binData1; // 数据还原
                    ttgo[index] = (uint16_t)result; // 数据存入缓存数组
                    
                    index++;
                    if(index == 12800) // 读取完一帧就显示
                    {
                      index = 0;
                      tft.pushImage(0, 0,  sh, sw, ttgo);
                    }
                }else
                {
                    binData0 = client.read(); // 读取一个像素的色彩数据的第一字节
                }
                flag++;
            }else
            {
//                Serial.print("\n");
            }
            
        }
        Serial.println("关闭当前连接");
        client.stop(); //关闭客户端
    }
    else
    {
        Serial.println("访问失败");
        client.stop(); //关闭客户端
    }
}
