#include "communication/com_server.h"
#include "config/config.h"
#include "communication/cmd_handler.h"
#include "communication/message_queue.h"
#include "network/network_mgr.h"
#include "light/led_driver.h"
#include <Arduino.h>
#include <WebServer.h>
#include <ArduinoJson.h>

WebSocketsServer webSocket = WebSocketsServer(SERVER_PORT);
WebServer server(80);

// 配网页面HTML
const char config_html[] PROGMEM = R"=====(<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 RGBW 配网</title>
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
    .config-panel {
      background-color: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    }
    .config-group {
      margin-bottom: 20px;
    }
    label {
      display: block;
      margin-bottom: 5px;
      font-weight: bold;
    }
    input[type="text"], input[type="number"] {
      width: 100%;
      padding: 10px;
      border: 1px solid #ddd;
      border-radius: 4px;
      box-sizing: border-box;
    }
    select {
      width: 100%;
      padding: 10px;
      border: 1px solid #ddd;
      border-radius: 4px;
      box-sizing: border-box;
    }
    button {
      background-color: #4CAF50;
      color: white;
      padding: 12px 20px;
      border: none;
      border-radius: 4px;
      cursor: pointer;
      font-size: 16px;
      width: 100%;
      margin-top: 10px;
    }
    button:hover {
      background-color: #45a049;
    }
    .status {
      margin-top: 20px;
      padding: 10px;
      background-color: #e8f5e8;
      border-radius: 4px;
      text-align: center;
    }
    .error {
      background-color: #ffebee;
      color: #c62828;
    }
    .success {
      background-color: #e8f5e8;
      color: #2e7d32;
    }
  </style>
</head>
<body>
  <h1>ESP32 RGBW 配网</h1>
  <div class="config-panel">
    <form id="configForm">
      <div class="config-group">
        <label>WiFi 模式</label>
        <select id="wifiMode">
          <option value="0">AP模式 (设备作为热点)</option>
          <option value="1">Station模式 (连接到路由器)</option>
        </select>
      </div>
      
      <div class="config-group">
        <label>AP模式 SSID</label>
        <input type="text" id="apSSID" placeholder="设备热点名称">
      </div>
      
      <div class="config-group">
        <label>AP模式 密码</label>
        <input type="text" id="apPassword" placeholder="设备热点密码">
      </div>
      
      <div class="config-group">
        <label>Station模式 SSID</label>
        <input type="text" id="stationSSID" placeholder="路由器名称">
      </div>
      
      <div class="config-group">
        <label>Station模式 密码</label>
        <input type="text" id="stationPassword" placeholder="路由器密码">
      </div>
      
      <div class="config-group">
        <label>LED 引脚</label>
        <input type="number" id="ledPin" min="0" max="39" value="7">
      </div>
      
      <div class="config-group">
        <label>LED 数量</label>
        <input type="number" id="ledCount" min="1" max="100" value="30">
      </div>
      
      <div class="config-group">
        <label>LED 最大亮度</label>
        <input type="number" id="ledBrightness" min="0" max="255" value="255">
      </div>
      
      <button type="submit">保存配置</button>
      <button type="button" onclick="resetConfig()">恢复默认</button>
    </form>
  </div>
  
  <div class="status" id="status">
    状态: 就绪
  </div>
  
  <script>
    document.getElementById('configForm').addEventListener('submit', function(e) {
      e.preventDefault();
      saveConfig();
    });
    
    function saveConfig() {
      const config = {
        wifiMode: parseInt(document.getElementById('wifiMode').value),
        apSSID: document.getElementById('apSSID').value,
        apPassword: document.getElementById('apPassword').value,
        stationSSID: document.getElementById('stationSSID').value,
        stationPassword: document.getElementById('stationPassword').value,
        ledPin: parseInt(document.getElementById('ledPin').value),
        ledCount: parseInt(document.getElementById('ledCount').value),
        ledBrightness: parseInt(document.getElementById('ledBrightness').value)
      };
      
      fetch('/save-config', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(config)
      })
      .then(response => response.json())
      .then(data => {
        const statusElement = document.getElementById('status');
        if (data.status === 'ok') {
          statusElement.className = 'status success';
          statusElement.textContent = '配置保存成功，设备正在重启...';
          setTimeout(() => {
            statusElement.textContent = '请重新连接到新的网络';
          }, 2000);
        } else {
          statusElement.className = 'status error';
          statusElement.textContent = '配置保存失败: ' + data.message;
        }
      })
      .catch(error => {
        document.getElementById('status').className = 'status error';
        document.getElementById('status').textContent = '保存失败: ' + error.message;
      });
    }
    
    function resetConfig() {
      if (confirm('确定要恢复默认配置吗？')) {
        fetch('/reset-config', {
          method: 'POST'
        })
        .then(response => response.json())
        .then(data => {
          const statusElement = document.getElementById('status');
          if (data.status === 'ok') {
            statusElement.className = 'status success';
            statusElement.textContent = '已恢复默认配置，设备正在重启...';
            setTimeout(() => {
              statusElement.textContent = '请重新连接到默认网络';
            }, 2000);
          } else {
            statusElement.className = 'status error';
            statusElement.textContent = '恢复失败: ' + data.message;
          }
        })
        .catch(error => {
          document.getElementById('status').className = 'status error';
          document.getElementById('status').textContent = '恢复失败: ' + error.message;
        });
      }
    }
    
    // 加载当前配置
    function loadConfig() {
      fetch('/get-config')
      .then(response => response.json())
      .then(data => {
        if (data.status === 'ok') {
          document.getElementById('wifiMode').value = data.config.wifiMode;
          document.getElementById('apSSID').value = data.config.apSSID;
          document.getElementById('apPassword').value = data.config.apPassword;
          document.getElementById('stationSSID').value = data.config.stationSSID;
          document.getElementById('stationPassword').value = data.config.stationPassword;
          document.getElementById('ledPin').value = data.config.ledPin;
          document.getElementById('ledCount').value = data.config.ledCount;
          document.getElementById('ledBrightness').value = data.config.ledBrightness;
        }
      });
    }
    
    // 初始化加载配置
    loadConfig();
  </script>
</body>
</html>
)=====";

// 简单的Web界面HTML
const char index_html[] PROGMEM = R"=====(<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 RGBW Control</title>
  <style>
    /* 全局样式 */
    * {
      box-sizing: border-box;
      margin: 0;
      padding: 0;
    }
    
    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background-color: #f5f5f5;
      color: #333;
      line-height: 1.6;
    }
    
    .container {
      max-width: 1200px;
      margin: 0 auto;
      padding: 20px;
    }
    
    /* 头部导航 */
    .header {
      text-align: center;
      margin-bottom: 30px;
    }
    
    .nav {
      display: flex;
      justify-content: center;
      margin-bottom: 20px;
      gap: 20px;
    }
    
    .nav a {
      text-decoration: none;
      color: #4CAF50;
      font-weight: bold;
      font-size: 16px;
      padding: 8px 16px;
      border-radius: 20px;
      transition: all 0.3s ease;
    }
    
    .nav a:hover {
      background-color: #4CAF50;
      color: white;
    }
    
    h1 {
      color: #333;
      font-size: 28px;
      margin-bottom: 10px;
    }
    
    /* 控制面板 */
    .control-panel {
      background-color: white;
      border-radius: 12px;
      box-shadow: 0 4px 15px rgba(0,0,0,0.1);
      padding: 30px;
      margin-bottom: 30px;
    }
    
    /* 控制组网格布局 */
    .control-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 20px;
    }
    
    .control-group {
      margin-bottom: 0;
    }
    
    label {
      display: block;
      margin-bottom: 8px;
      font-weight: 600;
      color: #555;
    }
    
    /* 开关按钮 */
    .power-button {
      width: 100%;
      background-color: #4CAF50;
      color: white;
      padding: 12px 24px;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      font-size: 18px;
      font-weight: 600;
      transition: all 0.3s ease;
    }
    
    .power-button:hover {
      background-color: #45a049;
      transform: translateY(-2px);
      box-shadow: 0 4px 10px rgba(76, 175, 80, 0.3);
    }
    
    /* 模式选择 */
    select {
      width: 100%;
      padding: 10px;
      border: 1px solid #ddd;
      border-radius: 6px;
      font-size: 16px;
      background-color: white;
    }
    
    /* 滑块控件 */
    .slider-container {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    
    input[type="range"] {
      flex: 1;
      margin: 0;
      -webkit-appearance: none;
      height: 6px;
      background: #ddd;
      border-radius: 3px;
      outline: none;
    }
    
    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 18px;
      height: 18px;
      background: #4CAF50;
      border-radius: 50%;
      cursor: pointer;
    }
    
    input[type="range"]::-moz-range-thumb {
      width: 18px;
      height: 18px;
      background: #4CAF50;
      border-radius: 50%;
      cursor: pointer;
      border: none;
    }
    
    /* 数值输入框 */
    input[type="number"] {
      width: 60px;
      padding: 8px;
      border: 1px solid #ddd;
      border-radius: 4px;
      font-size: 14px;
      text-align: center;
    }
    
    /* 状态显示 */
    .status {
      padding: 15px;
      background-color: #e8f5e8;
      border-radius: 8px;
      text-align: center;
      font-weight: 500;
      transition: all 0.3s ease;
    }
    
    .status.error {
      background-color: #ffebee;
      color: #c62828;
    }
    
    /* 响应式设计 */
    @media (max-width: 768px) {
      .container {
        padding: 10px;
      }
      
      .control-panel {
        padding: 20px;
      }
      
      .control-grid {
        grid-template-columns: 1fr;
      }
      
      h1 {
        font-size: 24px;
      }
    }
    
    @media (max-width: 480px) {
      .nav {
        flex-direction: column;
        align-items: center;
      }
      
      .nav a {
        width: 100%;
        text-align: center;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="header">
      <div class="nav">
        <a href="/">控制</a>
        <a href="/config">配置</a>
      </div>
      <h1>ESP32 RGBW灯带控制</h1>
    </div>
    
    <div class="control-panel">
      <div class="control-grid">
        <div class="control-group">
          <label>总开关</label>
          <button id="power" class="power-button" onclick="togglePower()">关闭</button>
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
          <div class="slider-container">
            <input type="range" id="r" min="0" max="255" value="0" oninput="updateColor()">
            <input type="number" id="r-num" min="0" max="255" value="0" oninput="updateColorFromNumber('r')">
          </div>
        </div>
        
        <div class="control-group">
          <label>绿色: <span id="g-value">0</span></label>
          <div class="slider-container">
            <input type="range" id="g" min="0" max="255" value="0" oninput="updateColor()">
            <input type="number" id="g-num" min="0" max="255" value="0" oninput="updateColorFromNumber('g')">
          </div>
        </div>
        
        <div class="control-group">
          <label>蓝色: <span id="b-value">0</span></label>
          <div class="slider-container">
            <input type="range" id="b" min="0" max="255" value="0" oninput="updateColor()">
            <input type="number" id="b-num" min="0" max="255" value="0" oninput="updateColorFromNumber('b')">
          </div>
        </div>
        
        <div class="control-group">
          <label>白色: <span id="w-value">0</span></label>
          <div class="slider-container">
            <input type="range" id="w" min="0" max="255" value="0" oninput="updateColor()">
            <input type="number" id="w-num" min="0" max="255" value="0" oninput="updateColorFromNumber('w')">
          </div>
        </div>
        
        <div class="control-group">
          <label>亮度: <span id="brightness-value">80</span>%</label>
          <div class="slider-container">
            <input type="range" id="brightness" min="0" max="100" value="80" oninput="updateBrightness()">
          </div>
        </div>
        
        <div class="control-group">
          <label>速度: <span id="speed-value">50</span></label>
          <div class="slider-container">
            <input type="range" id="speed" min="0" max="100" value="50" oninput="updateSpeed()">
          </div>
        </div>
      </div>
    </div>
    
    <div class="status" id="status">
      状态: 未连接
    </div>
  </div>
  
  <script>
    class WebSocketManager {
      constructor() {
        this.ws = null;
        this.powerState = false;
        this.reconnectAttempts = 0;
        this.maxReconnectAttempts = 5;
        this.reconnectDelay = 3000;
        this.messageId = 0;
        this.pendingMessages = new Map();
        this.heartbeatInterval = null;
        this.lastHeartbeat = 0;
      }
      
      connect() {
        if (this.ws && (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING)) {
          return;
        }
        
        try {
          this.ws = new WebSocket('ws://' + window.location.hostname + ':8080');
          
          this.ws.onopen = () => {
            this.onOpen();
          };
          
          this.ws.onmessage = (event) => {
            this.onMessage(event);
          };
          
          this.ws.onclose = () => {
            this.onClose();
          };
          
          this.ws.onerror = (error) => {
            this.onError(error);
          };
        } catch (error) {
          console.error('WebSocket connection error:', error);
          this.scheduleReconnect();
        }
      }
      
      onOpen() {
        console.log('WebSocket connected');
        document.getElementById('status').textContent = '状态: 已连接';
        this.reconnectAttempts = 0;
        this.startHeartbeat();
        this.sendCommand('get_status');
      }
      
      onMessage(event) {
        try {
          const data = JSON.parse(event.data);
          
          // 处理消息确认
          if (data.msgId && this.pendingMessages.has(data.msgId)) {
            const callback = this.pendingMessages.get(data.msgId);
            if (callback) {
              callback(data);
            }
            this.pendingMessages.delete(data.msgId);
          }
          
          // 处理状态更新
          if (data.cmd === 'status') {
            this.updateUI(data);
          }
          
          // 处理心跳响应
          if (data.cmd === 'pong') {
            this.lastHeartbeat = Date.now();
          }
        } catch (error) {
          console.error('Error parsing WebSocket message:', error);
        }
      }
      
      onClose() {
        console.log('WebSocket disconnected');
        document.getElementById('status').textContent = '状态: 已断开';
        this.stopHeartbeat();
        this.scheduleReconnect();
      }
      
      onError(error) {
        console.error('WebSocket error:', error);
      }
      
      scheduleReconnect() {
        if (this.reconnectAttempts < this.maxReconnectAttempts) {
          this.reconnectAttempts++;
          const delay = this.reconnectDelay * Math.pow(1.5, this.reconnectAttempts - 1);
          console.log(`Attempting to reconnect in ${delay}ms...`);
          setTimeout(() => this.connect(), delay);
        } else {
          console.error('Max reconnect attempts reached');
          document.getElementById('status').textContent = '状态: 连接失败';
        }
      }
      
      startHeartbeat() {
        this.stopHeartbeat();
        this.heartbeatInterval = setInterval(() => {
          if (this.ws && this.ws.readyState === WebSocket.OPEN) {
            this.sendCommand('ping');
          }
        }, 30000);
      }
      
      stopHeartbeat() {
        if (this.heartbeatInterval) {
          clearInterval(this.heartbeatInterval);
          this.heartbeatInterval = null;
        }
      }
      
      sendCommand(cmd, data = {}, callback = null) {
        if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
          console.error('WebSocket not connected');
          return false;
        }
        
        const msgId = ++this.messageId;
        const message = {
          msgId,
          cmd,
          ...data
        };
        
        if (callback) {
          this.pendingMessages.set(msgId, callback);
          // 设置超时
          setTimeout(() => {
            if (this.pendingMessages.has(msgId)) {
              this.pendingMessages.delete(msgId);
              console.error(`Message ${msgId} timed out`);
            }
          }, 5000);
        }
        
        try {
          this.ws.send(JSON.stringify(message));
          return true;
        } catch (error) {
          console.error('Error sending message:', error);
          return false;
        }
      }
      
      updateUI(data) {
        this.powerState = data.on;
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
    }
    
    // 初始化 WebSocket 管理器
    const wsManager = new WebSocketManager();
    
    // 页面加载完成后连接
    window.addEventListener('DOMContentLoaded', () => {
      wsManager.connect();
    });
    
    // 事件处理函数
    function togglePower() {
      const newState = !wsManager.powerState;
      document.getElementById('power').textContent = newState ? '关闭' : '打开';
      wsManager.sendCommand('switch', { on: newState });
    }
    
    function changeMode() {
      const mode = document.getElementById('mode').value;
      const r = parseInt(document.getElementById('r').value);
      const g = parseInt(document.getElementById('g').value);
      const b = parseInt(document.getElementById('b').value);
      const w = parseInt(document.getElementById('w').value);
      const brightness = parseInt(document.getElementById('brightness').value);
      const speed = parseInt(document.getElementById('speed').value);
      
      if (mode == 0) {
        // 静态模式
        wsManager.sendCommand('static', {
          color: { r, g, b, w },
          brightness
        });
      } else {
        // 效果模式
        const effectNames = ['', 'breath', 'rainbow', 'flashing'];
        wsManager.sendCommand('effect', {
          name: effectNames[mode],
          speed,
          color: { r, g, b, w },
          brightness
        });
      }
    }
    
    function updateColor() {
      const r = document.getElementById('r').value;
      const g = document.getElementById('g').value;
      const b = document.getElementById('b').value;
      const w = document.getElementById('w').value;
      const brightness = document.getElementById('brightness').value;
      const mode = document.getElementById('mode').value;
      
      // 批量更新DOM
      const updates = [
        { id: 'r-value', value: r },
        { id: 'g-value', value: g },
        { id: 'b-value', value: b },
        { id: 'w-value', value: w },
        { id: 'r-num', value: r },
        { id: 'g-num', value: g },
        { id: 'b-num', value: b },
        { id: 'w-num', value: w }
      ];
      
      // 使用 requestAnimationFrame 优化DOM操作
      requestAnimationFrame(() => {
        updates.forEach(update => {
          document.getElementById(update.id).textContent = update.value;
        });
      });
      
      if (mode == 0) {
        wsManager.sendCommand('static', {
          color: { r: parseInt(r), g: parseInt(g), b: parseInt(b), w: parseInt(w) },
          brightness: parseInt(brightness)
        });
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
        wsManager.sendCommand('static', {
          color: { r: parseInt(r), g: parseInt(g), b: parseInt(b), w: parseInt(w) },
          brightness: parseInt(brightness)
        });
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
        wsManager.sendCommand('effect', {
          name: effectNames[mode],
          speed: parseInt(speed),
          color: { r: parseInt(r), g: parseInt(g), b: parseInt(b), w: parseInt(w) },
          brightness: parseInt(brightness)
        });
      }
    }
  </script>
</body>
</html>
)=====";

// 静态JSON文档，避免频繁动态分配
StaticJsonDocument<512> configDoc;

// 处理获取配置请求
void handleGetConfig() {
  Config config;
  loadConfig(&config);
  
  configDoc.clear();
  configDoc["status"] = "ok";
  configDoc["config"]["wifiMode"] = config.wifiMode;
  configDoc["config"]["apSSID"] = config.apSSID;
  configDoc["config"]["apPassword"] = config.apPassword;
  configDoc["config"]["stationSSID"] = config.stationSSID;
  configDoc["config"]["stationPassword"] = config.stationPassword;
  configDoc["config"]["ledPin"] = config.ledPin;
  configDoc["config"]["ledCount"] = config.ledCount;
  configDoc["config"]["ledBrightness"] = config.ledBrightness;
  
  String response;
  serializeJson(configDoc, response);
  server.send(200, "application/json", response);
}

// 处理保存配置请求
void handleSaveConfig() {
  if (server.hasArg("plain")) {
    String json = server.arg("plain");
    
    configDoc.clear();
    DeserializationError error = deserializeJson(configDoc, json);
    
    if (error) {
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid JSON\"}");
      return;
    }
    
    Config newConfig;
    newConfig.wifiMode = configDoc["wifiMode"];
    newConfig.apSSID = configDoc["apSSID"].as<String>();
    newConfig.apPassword = configDoc["apPassword"].as<String>();
    newConfig.stationSSID = configDoc["stationSSID"].as<String>();
    newConfig.stationPassword = configDoc["stationPassword"].as<String>();
    newConfig.ledPin = configDoc["ledPin"];
    newConfig.ledCount = configDoc["ledCount"];
    newConfig.ledBrightness = configDoc["ledBrightness"];
    
    // 验证配置
    if (newConfig.ledPin < 0 || newConfig.ledPin > 39) {
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid LED pin\"}");
      return;
    }
    
    if (newConfig.ledCount < 1 || newConfig.ledCount > 100) {
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid LED count\"}");
      return;
    }
    
    if (newConfig.ledBrightness < 0 || newConfig.ledBrightness > 255) {
      server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"Invalid LED brightness\"}");
      return;
    }
    
    // 保存配置
    saveConfig(&newConfig);
    
    // 重新初始化LED配置
    initLED();
    
    server.send(200, "application/json", "{\"status\": \"ok\"}");
    
    // 重启设备以应用新配置
    Serial.println("Restarting device to apply new configuration...");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "application/json", "{\"status\": \"error\", \"message\": \"No data received\"}");
  }
}

// 处理重置配置请求
void handleResetConfig() {
  resetConfig();
  server.send(200, "application/json", "{\"status\": \"ok\"}");
  
  // 重启设备以应用默认配置
  Serial.println("Restarting device to apply default configuration...");
  delay(1000);
  ESP.restart();
}

void initWebSocketServer() {
  // 启动WebSocket服务器
  webSocket.begin();
  webSocket.onEvent(handleWebSocketEvent);
  Serial.println("WebSocket server started");
  
  // 启动Web服务器
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", index_html);
  });
  
  // 配网页面
  server.on("/config", HTTP_GET, []() {
    server.send_P(200, "text/html", config_html);
  });
  
  // 配置API
  server.on("/get-config", HTTP_GET, handleGetConfig);
  server.on("/save-config", HTTP_POST, handleSaveConfig);
  server.on("/reset-config", HTTP_POST, handleResetConfig);
  
  server.begin();
  Serial.println("Web server started");
  Serial.println("Web interface available at http://esp32-rgbw.local");
  Serial.println("Configuration page available at http://esp32-rgbw.local/config");
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
      Serial.printf("Received text message: %s\n", payload);
      // 处理心跳包
      if (strstr((char*)payload, "ping") != NULL) {
        webSocket.sendTXT(client_num, "{\"cmd\": \"pong\"}");
      } else {
        // 使用消息队列处理命令
        sendMessage(MSG_TYPE_COMMAND, (const char*)payload, length);
      }
      break;
    case WStype_BIN:
      Serial.printf("Received binary message, length: %u\n", length);
      // 使用消息队列处理二进制消息
      sendMessage(MSG_TYPE_COMMAND, (const char*)payload, length);
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
