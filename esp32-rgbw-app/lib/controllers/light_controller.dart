import 'package:flutter/foundation.dart';
import 'package:esp32_rgbw_app/models/color.dart';
import 'package:esp32_rgbw_app/models/effect.dart';
import 'package:esp32_rgbw_app/services/websocket_service.dart';

class LightController extends ChangeNotifier {
  WebSocketService? _webSocketService;
  EffectModel _currentEffect;
  bool _isOn = false;

  EffectModel get currentEffect => _currentEffect;
  bool get isOn => _isOn;

  LightController([this._webSocketService]) 
    : _currentEffect = EffectModel(
        type: EffectType.static,
        color: ColorModel(),
        speed: 50,
        isOn: false,
      ) {
    if (_webSocketService != null) {
      _setupWebSocketListener();
    }
  }

  void setWebSocketService(WebSocketService webSocketService) {
    _webSocketService = webSocketService;
    _setupWebSocketListener();
  }

  void _setupWebSocketListener() {
    if (_webSocketService == null) return;
    _webSocketService!.messageStream.listen((message) {
      if (message['cmd'] == 'status') {
        _updateFromStatus(message);
      }
    });
  }

  void _updateFromStatus(Map<String, dynamic> status) {
    _isOn = status['on'] ?? false;
    
    final int mode = status['mode'] ?? 0;
    final EffectType type = EffectModel.parseEffectType(mode);
    
    final Map<String, dynamic> colorData = status['color'] ?? {};
    final ColorModel color = ColorModel(
      r: colorData['r'] ?? 0,
      g: colorData['g'] ?? 0,
      b: colorData['b'] ?? 0,
      w: colorData['w'] ?? 0,
      brightness: status['brightness'] ?? 100,
    );
    
    final int speed = status['speed'] ?? 50;
    
    _currentEffect = EffectModel(
      type: type,
      color: color,
      speed: speed,
      isOn: _isOn,
    );
    
    notifyListeners();
  }

  void setPower(bool on) {
    _isOn = on;
    _currentEffect.isOn = on;
    _webSocketService?.setPower(on);
    notifyListeners();
  }

  void setStaticColor(ColorModel color) {
    _currentEffect.type = EffectType.static;
    _currentEffect.color = color;
    _webSocketService?.setStaticColor(color);
    notifyListeners();
  }

  void setEffect(EffectType type, ColorModel color, int speed) {
    _currentEffect.type = type;
    _currentEffect.color = color;
    _currentEffect.speed = speed;
    _webSocketService?.setEffect(type, color, speed);
    notifyListeners();
  }

  void updateColor(ColorModel color) {
    _currentEffect.color = color;
    if (_currentEffect.type == EffectType.static) {
      _webSocketService?.setStaticColor(color);
    } else {
      _webSocketService?.setEffect(_currentEffect.type, color, _currentEffect.speed);
    }
    notifyListeners();
  }

  void updateSpeed(int speed) {
    _currentEffect.speed = speed;
    if (_currentEffect.type != EffectType.static) {
      _webSocketService?.setEffect(_currentEffect.type, _currentEffect.color, speed);
    }
    notifyListeners();
  }

  void updateEffectType(EffectType type) {
    _currentEffect.type = type;
    if (type == EffectType.static) {
      _webSocketService?.setStaticColor(_currentEffect.color);
    } else {
      _webSocketService?.setEffect(type, _currentEffect.color, _currentEffect.speed);
    }
    notifyListeners();
  }

  void getStatus() {
    _webSocketService?.getStatus();
  }
}
