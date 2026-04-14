import 'package:flutter/material.dart';

class ColorModel {
  int r;
  int g;
  int b;
  int w;
  int brightness;

  ColorModel({
    this.r = 0,
    this.g = 0,
    this.b = 0,
    this.w = 0,
    this.brightness = 100,
  });

  factory ColorModel.fromMap(Map<String, dynamic> map) {
    return ColorModel(
      r: map['r'] ?? 0,
      g: map['g'] ?? 0,
      b: map['b'] ?? 0,
      w: map['w'] ?? 0,
      brightness: map['brightness'] ?? 100,
    );
  }

  Map<String, dynamic> toMap() {
    return {
      'r': r,
      'g': g,
      'b': b,
      'w': w,
      'brightness': brightness,
    };
  }

  // 转换为Flutter Color
  Color toFlutterColor() {
    double brightnessFactor = brightness / 100.0;
    return Color.fromARGB(
      255,
      (r * brightnessFactor).round(),
      (g * brightnessFactor).round(),
      (b * brightnessFactor).round(),
    );
  }

  // 从Flutter Color创建
  factory ColorModel.fromFlutterColor(Color color, {int w = 0, int brightness = 100}) {
    return ColorModel(
      r: color.red,
      g: color.green,
      b: color.blue,
      w: w,
      brightness: brightness,
    );
  }

  @override
  String toString() {
    return 'ColorModel(r: $r, g: $g, b: $b, w: $w, brightness: $brightness)';
  }
}
