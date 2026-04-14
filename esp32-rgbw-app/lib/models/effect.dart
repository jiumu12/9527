import 'package:esp32_rgbw_app/models/color.dart';

enum EffectType {
  static,
  breath,
  rainbow,
  flashing,
}

class EffectModel {
  EffectType type;
  ColorModel color;
  int speed;
  bool isOn;

  EffectModel({
    this.type = EffectType.static,
    required this.color,
    this.speed = 50,
    this.isOn = false,
  });

  factory EffectModel.fromMap(Map<String, dynamic> map) {
    return EffectModel(
      type: parseEffectType(map['type'] ?? 0),
      color: ColorModel.fromMap(map['color'] ?? {}),
      speed: map['speed'] ?? 50,
      isOn: map['isOn'] ?? false,
    );
  }

  Map<String, dynamic> toMap() {
    return {
      'type': _effectTypeToInt(type),
      'color': color.toMap(),
      'speed': speed,
      'isOn': isOn,
    };
  }

  static EffectType parseEffectType(int type) {
    switch (type) {
      case 0:
        return EffectType.static;
      case 1:
        return EffectType.breath;
      case 2:
        return EffectType.rainbow;
      case 3:
        return EffectType.flashing;
      default:
        return EffectType.static;
    }
  }

  static int _effectTypeToInt(EffectType type) {
    switch (type) {
      case EffectType.static:
        return 0;
      case EffectType.breath:
        return 1;
      case EffectType.rainbow:
        return 2;
      case EffectType.flashing:
        return 3;
    }
  }

  String get effectName {
    switch (type) {
      case EffectType.static:
        return 'Static';
      case EffectType.breath:
        return 'Breath';
      case EffectType.rainbow:
        return 'Rainbow';
      case EffectType.flashing:
        return 'Flashing';
    }
  }

  @override
  String toString() {
    return 'EffectModel(type: $type, color: $color, speed: $speed, isOn: $isOn)';
  }
}
