# Slow Jogging Monitor
A monitor using LILYGO T-Display-S3 ESP32S3

![](https://github.com/Jung217/Slow_Jogging_Monitor/blob/main/property/01%20pics/image-32.png)

## Main program
* ***30102beat_v0.1*** : 只有測量程式，無 tft 顯示
* ***slow_jogging_monitor_v2.0.1*** : 新增開機畫面及設定畫面
* ***slow_jogging_monitor_v2.1.0*** : 新增測量頁面數值顯示及失手警告

## Property
* ***00 Explore*** : 內含 tft 範例程式
  * ImageScroll : 捲動的德國地圖
  * TFT_Rainbow : 時鐘 + 彩虹填滿螢幕
* ***01 pics*** : 一些圖片
* ***02 Trash*** : 試不出來的 image to byte array 產物


## 參考資料
* [Xinyuan-LilyGO / T-Display-S3](https://github.com/Xinyuan-LilyGO/T-Display-S3/tree/main)
* [youjunjer / ESP32-MAX30102 / 02SPO2_BPM.ino](https://github.com/youjunjer/ESP32-MAX30102/blob/main/02SPO2_BPM.ino)
* [How to Write Parallel Multitasking Applications for ESP32 using FreeRTOS & Arduino](https://www.circuitstate.com/tutorials/how-to-write-parallel-multitasking-applications-for-esp32-using-freertos-arduino/#google_vignette)
* [ESP32 Arduino 學習篇（五）TFT_eSPI庫](https://blog.csdn.net/DOF526570/article/details/128859819)
* [【iCShop開箱趣】ep2 LILYGO T-QT ESP32-S3 Review 迷你開發板評測 | Arduino TFT_eSPI Setup 設定](https://www.youtube.com/watch?v=APCz1XeYjW4)
* [【開箱評測】LILYGO T-Display-S3 ESP32S3 大尺寸全彩 LCD 開發板](https://www.circuspi.com/index.php/2023/05/31/unboxing-lilygo-t-display-s3/)
* [image2cpp](https://javl.github.io/image2cpp/)
