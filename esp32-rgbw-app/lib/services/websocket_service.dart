import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:web_socket_channel/io.dart';
import 'package:esp32_rgbw_app/models/color.dart';
import 'package:esp32_rgbw_app/models/effect.dart';

// 协议类型枚举
enum ProtocolType {
  json,
  binary,
}

// 二进制协议命令定义
class BinaryCommands {
  static const int static = 0x01;
  static const int effect = 0x02;
  static const int switch_ = 0x03;
  static const int ping = 0x04;
  static const int getStatus = 0x05;
}

// 效果类型映射
class BinaryEffectTypes {
  static const int static = 0;
  static const int breath = 1;
  static const int rainbow = 2;
  static const int flashing = 3;
}

class WebSocketService {
  late WebSocketChannel _channel;
  late StreamController<Map<String, dynamic>> _messageController;
  bool _isConnected = false;
  String? _ipAddress;
  int? _port;
  Timer? _heartbeatTimer;
  Timer? _reconnectTimer;
  final Duration _heartbeatInterval = const Duration(seconds: 30);
  final Duration _reconnectInterval = const Duration(seconds: 5);
  ProtocolType _protocolType = ProtocolType.json;
  bool _protocolNegotiated = false;

  Stream<Map<String, dynamic>> get messageStream => _messageController.stream;
  bool get isConnected => _isConnected;
  ProtocolType get protocolType => _protocolType;

  Future<void> connect(String ipAddress, int port) async {
    _ipAddress = ipAddress;
    _port = port;
    
    try {
      final url = 'ws://$ipAddress:$port';
      _channel = IOWebSocketChannel.connect(url);
      _messageController = StreamController<Map<String, dynamic>>.broadcast();

      _channel.stream.listen(
        (message) {
          if (message is String) {
            // 处理JSON消息
            try {
              final Map<String, dynamic> data = jsonDecode(message);
              _handleMessage(data);
              _messageController.add(data);
            } catch (e) {
              print('Error parsing WebSocket message: $e');
            }
          } else if (message is Uint8List) {
            // 处理二进制消息
            print('Received binary message: ${message.length} bytes');
            // 二进制消息主要用于从设备到App的状态更新
            // 这里可以添加二进制消息解析逻辑
          }
        },
        onError: (error) {
          print('WebSocket error: $error');
          _isConnected = false;
          _startReconnect();
        },
        onDone: () {
          print('WebSocket connection closed');
          _isConnected = false;
          _startReconnect();
        },
      );

      _isConnected = true;
      _startHeartbeat();
      _negotiateProtocol();
      print('Connected to $url');
    } catch (e) {
      print('Error connecting to WebSocket: $e');
      _isConnected = false;
      _startReconnect();
      rethrow;
    }
  }

  void _handleMessage(Map<String, dynamic> message) {
    if (message['cmd'] == 'protocol_supported') {
      // 处理协议协商响应
      bool jsonSupported = message['json'] ?? false;
      bool binarySupported = message['binary'] ?? false;
      
      if (binarySupported) {
        _protocolType = ProtocolType.binary;
        print('Switched to binary protocol');
      } else {
        _protocolType = ProtocolType.json;
        print('Using JSON protocol');
      }
      _protocolNegotiated = true;
    }
  }

  void _negotiateProtocol() {
    // 发送协议协商请求
    sendCommand('protocol_negotiate', {});
  }

  void _startHeartbeat() {
    _heartbeatTimer?.cancel();
    _heartbeatTimer = Timer.periodic(_heartbeatInterval, (timer) {
      if (_isConnected) {
        try {
          if (_protocolType == ProtocolType.binary) {
            // 发送二进制心跳包
            final buffer = Uint8List(2);
            buffer[0] = BinaryCommands.ping;
            buffer[1] = 0; // 数据长度
            _channel.sink.add(buffer);
          } else {
            // 发送JSON心跳包
            _channel.sink.add(jsonEncode({'cmd': 'ping'}));
          }
        } catch (e) {
          print('Error sending heartbeat: $e');
          _isConnected = false;
          _startReconnect();
        }
      }
    });
  }

  void _startReconnect() {
    _heartbeatTimer?.cancel();
    _reconnectTimer?.cancel();
    
    if (_ipAddress != null && _port != null) {
      _reconnectTimer = Timer(_reconnectInterval, () async {
        print('Attempting to reconnect...');
        try {
          await connect(_ipAddress!, _port!);
        } catch (e) {
          print('Reconnection failed: $e');
          _startReconnect();
        }
      });
    }
  }

  void disconnect() {
    _heartbeatTimer?.cancel();
    _reconnectTimer?.cancel();
    
    if (_isConnected) {
      _channel.sink.close();
      _messageController.close();
      _isConnected = false;
      _protocolNegotiated = false;
      print('Disconnected from WebSocket');
    }
  }

  void sendCommand(String cmd, Map<String, dynamic> data) {
    if (_isConnected) {
      final message = {
        'cmd': cmd,
        ...data,
      };
      _channel.sink.add(jsonEncode(message));
      print('Sent JSON command: $message');
    }
  }

  void sendBinaryCommand(int cmd, List<int> data) {
    if (_isConnected) {
      final buffer = Uint8List(2 + data.length);
      buffer[0] = cmd;
      buffer[1] = data.length;
      buffer.setRange(2, 2 + data.length, data);
      _channel.sink.add(buffer);
      print('Sent binary command: 0x${cmd.toRadixString(16).padLeft(2, '0')}, length: ${data.length}');
    }
  }

  void setStaticColor(ColorModel color) {
    if (_protocolType == ProtocolType.binary && _protocolNegotiated) {
      // 发送二进制静态颜色命令
      final data = [
        color.r,
        color.g,
        color.b,
        color.w,
        color.brightness,
      ];
      sendBinaryCommand(BinaryCommands.static, data);
    } else {
      // 发送JSON静态颜色命令
      sendCommand('static', {
        'color': {
          'r': color.r,
          'g': color.g,
          'b': color.b,
          'w': color.w,
        },
        'brightness': color.brightness,
      });
    }
  }

  void setEffect(EffectType type, ColorModel color, int speed) {
    if (_protocolType == ProtocolType.binary && _protocolNegotiated) {
      // 发送二进制效果命令
      int effectType;
      switch (type) {
        case EffectType.breath:
          effectType = BinaryEffectTypes.breath;
          break;
        case EffectType.rainbow:
          effectType = BinaryEffectTypes.rainbow;
          break;
        case EffectType.flashing:
          effectType = BinaryEffectTypes.flashing;
          break;
        default:
          effectType = BinaryEffectTypes.static;
      }
      
      final data = [
        effectType,
        speed,
        color.r,
        color.g,
        color.b,
        color.w,
        color.brightness,
      ];
      sendBinaryCommand(BinaryCommands.effect, data);
    } else {
      // 发送JSON效果命令
      String effectName;
      switch (type) {
        case EffectType.breath:
          effectName = 'breath';
          break;
        case EffectType.rainbow:
          effectName = 'rainbow';
          break;
        case EffectType.flashing:
          effectName = 'flashing';
          break;
        default:
          effectName = 'static';
      }

      sendCommand('effect', {
        'name': effectName,
        'speed': speed,
        'color': {
          'r': color.r,
          'g': color.g,
          'b': color.b,
          'w': color.w,
        },
        'brightness': color.brightness,
      });
    }
  }

  void setPower(bool on) {
    if (_protocolType == ProtocolType.binary && _protocolNegotiated) {
      // 发送二进制开关命令
      final data = [on ? 1 : 0];
      sendBinaryCommand(BinaryCommands.switch_, data);
    } else {
      // 发送JSON开关命令
      sendCommand('switch', {
        'on': on,
      });
    }
  }

  void getStatus() {
    if (_protocolType == ProtocolType.binary && _protocolNegotiated) {
      // 发送二进制获取状态命令
      sendBinaryCommand(BinaryCommands.getStatus, []);
    } else {
      // 发送JSON获取状态命令
      sendCommand('get_status', {});
    }
  }

  void setProtocolType(ProtocolType type) {
    _protocolType = type;
    print('Protocol type set to: $type');
  }
}
