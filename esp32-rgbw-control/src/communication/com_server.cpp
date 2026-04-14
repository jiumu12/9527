#include "communication/com_server.h"
#include "config/config.h"
#include "communication/cmd_handler.h"
#include <Arduino.h>
#include <WebServer.h>

WebSocketsServer webSocket = WebSocketsServer(SERVER_PORT);
WebServer server(80);

// 简单的Web界面HTML
const char index_html[] PROGMEM = R"=====(<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 RGBW Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      max-width: 600px;
      margin: 0 auto;
      padding: 20px;
      background-color: #f0f0f0;
    }
    h1 {
      text-align: center;
      color: #333;
    }
    .control-panel {
      background-color: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    }
    .control-group {
      margin-bottom: 20px;
    }
    label {
      display: block;
      margin-bottom: 5px;
      font-weight: bold;
    }
    input[type="range"] {
      width: 100%;
      margin: 10px 0;
    }
    input[type="number"] {
      width: 60px;
      padding: 5px;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    button {
      background-color: #4CAF50;
      color: white;
      padding: 10px 20px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      font-size: 16px;
    }
    button:hover {
      background-color: #45a049;
    }
    .status {
      margin-top: 20px;
      padding: 10px;
      background-color: #e8f5e8;
      border-radius: 4px;
    }
  </style>
</head>
<body>
  <h1>ESP32 RGBW灯带控制</h1>
  <div class="control-panel">
    <div class="control-group">
      <label>总开关</label>
      <button id="power" onclick="togglePower()">关闭</button>
    </div>
    
    <div class="control-group">
      <label>模式选择</label>
      <select id="mode" onchange="changeMode()">
        <option value="0">静态</option>
        <option value="1">呼吸</option>
        <option value="2">彩虹</option>
        <option value="3">闪烁</option>
      </select>
    </div>
    
    <div class="control-group">
      <label>红色: <span id="r-value">0</span></label>
      <input type="range" id="r" min="0" max="255" value="0" oninput="updateColor()">
      <input type="number" id="r-num" min="0" max="255" value="0" oninput="updateColorFromNumber('r')">
    </div>
    
    <div class="control-group">
      <label>绿色: <span id="g-value">0</span></label>
      <input type="range" id="g" min="0" max="255" value="0" oninput="updateColor()">
      <input type="number" id="g-num" min="0" max="255" value="0" oninput="updateColorFromNumber('g')">
    </div>
    
    <div class="control-group">
      <label>蓝色: <span id="b-value">0</span></label>
      <input type="range" id="b" min="0" max="255" value="0" oninput="updateColor()">
      <input type="number" id="b-num" min="0" max="255" value="0" oninput="updateColorFromNumber('b')">
    </div>
    
    <div class="control-group">
      <label>白色: <span id="w-value">0</span></label>
      <input type="range" id="w" min="0" max="255" value="0" oninput="updateColor()">
      <input type="number" id="w-num" min="0" max="255" value="0" oninput="updateColorFromNumber('w')">
    </div>
    
    <div class="control-group">
      <label>亮度: <span id="brightness-value">80</span>%</label>
      <input type="range" id="brightness" min="0" max="100" value="80" oninput="updateBrightness()">
    </div>
    
    <div class="control-group">
      <label>速度: <span id="speed-value">50</span></label>
      <input type="range" id="speed" min="0" max="100" value="50" oninput="updateSpeed()">
    </div>
  </div>
  
  <div class="status" id="status">
    状态: 未连接
  </div>
  
  <script>
    let ws;
    let powerState = false;
    
    function connectWebSocket() {
      ws = new WebSocket('ws://' + window.location.host + ':' + window.location.port + '/ws');
      
      ws.onopen = function() {
        document.getElementById('status').textContent = '状态: 已连接';
        // 获取当前状态
        ws.send('{"cmd":"get_status"}');
      };
      
      ws.onmessage = function(event) {
        const data = JSON.parse(event.data);
        if (data.cmd === 'status') {
          // 更新UI
          powerState = data.on;
          document.getElementById('power').textContent = data.on ? '关闭' : '打开';
          document.getElementById('mode').value = data.mode;
          document.getElementById('r').value = data.color.r;
          document.getElementById('g').value = data.color.g;
          document.getElementById('b').value = data.color.b;
          document.getElementById('w').value = data.color.w;
          document.getElementById('r-value').textContent = data.color.r;
          document.getElementById('g-value').textContent = data.color.g;
          document.getElementById('b-value').textContent = data.color.b;
          document.getElementById('w-value').textContent = data.color.w;
          document.getElementById('r-num').value = data.color.r;
          document.getElementById('g-num').value = data.color.g;
          document.getElementById('b-num').value = data.color.b;
          document.getElementById('w-num').value = data.color.w;
          document.getElementById('brightness').value = data.brightness;
          document.getElementById('brightness-value').textContent = data.brightness;
          document.getElementById('speed').value = data.speed;
          document.getElementById('speed-value').textContent = data.speed;
        }
      };
      
      ws.onclose = function() {
        document.getElementById('status').textContent = '状态: 已断开';
        // 尝试重连
        setTimeout(connectWebSocket, 3000);
      };
    }
    
    function togglePower() {
      powerState = !powerState;
      document.getElementById('power').textContent = powerState ? '关闭' : '打开';
      ws.send('{"cmd":"switch", "on":' + powerState + '}');
    }
    
    function changeMode() {
      const mode = document.getElementById('mode').value;
      const r = document.getElementById('r').value;
      const g = document.getElementById('g').value;
      const b = document.getElementById('b').value;
      const w = document.getElementById('w').value;
      const brightness = document.getElementById('brightness').value;
      const speed = document.getElementById('speed').value;
      
      if (mode == 0) {
        // 静态模式
        ws.send('{"cmd":"static", "color":{"r":' + r + ', "g":' + g + ', "b":' + b + ', "w":' + w + '}, "brightness":' + brightness + '}');
      } else {
        // 效果模式
        const effectNames = ['', 'breath', 'rainbow', 'flashing'];
        ws.send('{"cmd":"effect", "name":"' + effectNames[mode] + '", "speed":' + speed + ', "color":{"r":' + r + ', "g":' + g + ', "b":' + b + ', "w":' + w + '}, "brightness":' + brightness + '}');
      }
    }
    
    function updateColor() {
      const r = document.getElementById('r').value;
      const g = document.getElementById('g').value;
      const b = document.getElementById('b').value;
      const w = document.getElementById('w').value;
      const brightness = document.getElementById('brightness').value;
      const mode = document.getElementById('mode').value;
      
      document.getElementById('r-value').textContent = r;
      document.getElementById('g-value').textContent = g;
      document.getElementById('b-value').textContent = b;
      document.getElementById('w-value').textContent = w;
      document.getElementById('r-num').value = r;
      document.getElementById('g-num').value = g;
      document.getElementById('b-num').value = b;
      document.getElementById('w-num').value = w;
      
      if (mode == 0) {
        ws.send('{"cmd":"static", "color":{"r":' + r + ', "g":' + g + ', "b":' + b + ', "w":' + w + '}, "brightness":' + brightness + '}');
      }
    }
    
    function updateColorFromNumber(color) {
      const value = document.getElementById(color + '-num').value;
      document.getElementById(color).value = value;
      document.getElementById(color + '-value').textContent = value;
      updateColor();
    }
    
    function updateBrightness() {
      const brightness = document.getElementById('brightness').value;
      document.getElementById('brightness-value').textContent = brightness;
      const r = document.getElementById('r').value;
      const g = document.getElementById('g').value;
      const b = document.getElementById('b').value;
      const w = document.getElementById('w').value;
      const mode = document.getElementById('mode').value;
      
      if (mode == 0) {
        ws.send('{"cmd":"static", "color":{"r":' + r + ', "g":' + g + ', "b":' + b + ', "w":' + w + '}, "brightness":' + brightness + '}');
      }
    }
    
    function updateSpeed() {
      const speed = document.getElementById('speed').value;
      document.getElementById('speed-value').textContent = speed;
      const r = document.getElementById('r').value;
      const g = document.getElementById('g').value;
      const b = document.getElementById('b').value;
      const w = document.getElementById('w').value;
      const brightness = document.getElementById('brightness').value;
      const mode = document.getElementById('mode').value;
      
      if (mode != 0) {
        const effectNames = ['', 'breath', 'rainbow', 'flashing'];
        ws.send('{"cmd":"effect", "name":"' + effectNames[mode] + '", "speed":' + speed + ', "color":{"r":' + r + ', "g":' + g + ', "b":' + b + ', "w":' + w + '}, "brightness":' + brightness + '}');
      }
    }
    
    // 初始化连接
    connectWebSocket();
  </script>
</body>
</html>
)=====";

void initWebSocketServer() {
  // 启动WebSocket服务器
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("WebSocket server started");
  
  // 启动Web服务器
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });
  server.begin();
  Serial.println("Web server started");
  Serial.println("Web interface available at http://esp32-rgbw.local");
}

void handleWebSocketEvent(uint8_t client_num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("Client %u disconnected\n", client_num);
      break;
    case WStype_CONNECTED:
      Serial.printf("Client %u connected\n", client_num);
      break;
    case WStype_TEXT:
      Serial.printf("Received message: %s\n", payload);
      // 处理心跳包
      if (strstr((char*)payload, "ping") != NULL) {
        webSocket.sendTXT(client_num, "{\"cmd\": \"pong\"}");
      } else {
        handleCommand((char*)payload);
      }
      break;
    case WStype_ERROR:
      Serial.printf("WebSocket error\n");
      break;
  }
}

void broadcastMessage(const char* message) {
  webSocket.broadcastTXT(message);
  Serial.printf("Broadcasted message: %s\n", message);
}
