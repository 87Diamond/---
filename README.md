# 智慧電風扇控制 - AIoT 智慧整合系統



* ### 專案簡介 (Introduction)

這是一個結合**YOLO**與**IoT(ESP8266)**的智慧系統，旨在解決**節能減碳、節省資源消耗率**的問題。透過**Web儀表板**，使用者可以即時監控現場狀況並進行遠端控制。



#### \- 核心功能



\- **Edge AI 辨識**：使用 ESP8266 執行 YOLO 模型，偵測室內環境是否有人。

\- **IoT 控制**：ESP8266 接收辨識結果，自動觸發。

\- **雲端戰情室**：透過 MQTT 傳輸數據，並在網頁上視覺化呈現。



---



#### \- 技術堆疊 (Tech Stack)

\- **IoT Node**: ESP8266

\- **Connectivity**: MQTT Protocol

\- **Dashboard**: Python {Flask}



---



* ### 硬體配置 (Hardware Setup)



#### \- 接線說明

&nbsp;- 1. **Camara** ->  **MQTT**

&nbsp;- 2. **DHT11** -> ESP8266 -> **MQTT**

&nbsp;- 3. **MQTT** -> ESP8266 -> Relay -> **Fan**



