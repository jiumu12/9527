import 'dart:async';
import 'dart:convert';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:web_socket_channel/io.dart';
import 'package:esp32_rgbw_app/models/color.dart';
import 'package:esp32_rgbw_app/models/effect.dart';

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

  Stream<Map<String, dynamic>> get messageStream => _messageController.stream;
  bool get isConnected => _isConnected;

  Future<void> connect(String ipAddress, int port) async {
    _ipAddress = ipAddress;
    _port = port;
    
    try {
      final url = 'ws://$ipAddress:$port';
      _channel = IOWebSocketChannel.connect(url);
      _messageController = StreamController<Map<String, dynamic>>.broadcast();

      _channel.stream.listen(
        (message) {
          try {
            final Map<String, dynamic> data = jsonDecode(message);
            _messageController.add(data);
          } catch (e) {
            print('Error parsing WebSocket message: $e');
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
      print('Connected to $url');
    } catch (e) {
      print('Error connecting to WebSocket: $e');
      _isConnected = false;
      _startReconnect();
      rethrow;
    }
  }

  void _startHeartbeat() {
    _heartbeatTimer?.cancel();
    _heartbeatTimer = Timer.periodic(_heartbeatInterval, (timer) {
      if (_isConnected) {
        try {
          _channel.sink.add(jsonEncode({'cmd': 'ping'}));
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
      print('Sent command: $message');
    }
  }

  void setStaticColor(ColorModel color) {
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

  void setEffect(EffectType type, ColorModel color, int speed) {
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

  void setPower(bool on) {
    sendCommand('switch', {
      'on': on,
    });
  }

  void getStatus() {
    sendCommand('get_status', {});
  }
}
