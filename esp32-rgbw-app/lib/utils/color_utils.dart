// 颜色工具类

import '../models/color.dart';

class ColorUtils {
  // RGBW转HSV
  static Map<String, double> rgbwToHsv(Color color) {
    double r = color.r / 255.0;
    double g = color.g / 255.0;
    double b = color.b / 255.0;
    double w = color.w / 255.0;

    double max = [r, g, b].reduce((a, b) => a > b ? a : b);
    double min = [r, g, b].reduce((a, b) => a < b ? a : b);
    double h, s, v;

    v = max;

    double delta = max - min;
    s = max == 0 ? 0 : delta / max;

    if (delta == 0) {
      h = 0;
    } else if (max == r) {
      h = ((g - b) / delta) * 60;
    } else if (max == g) {
      h = (2 + (b - r) / delta) * 60;
    } else {
      h = (4 + (r - g) / delta) * 60;
    }

    h = h < 0 ? h + 360 : h;

    return {
      'h': h,
      's': s * 100,
      'v': v * 100,
      'w': w * 100,
    };
  }

  // HSV转RGBW
  static Color hsvToRgbw(double h, double s, double v, double w) {
    s /= 100;
    v /= 100;
    w /= 100;

    int hi = ((h / 60) % 6).floor();
    double f = (h / 60) - hi;
    double p = v * (1 - s);
    double q = v * (1 - f * s);
    double t = v * (1 - (1 - f) * s);

    double r, g, b;

    switch (hi) {
      case 0:
        r = v; g = t; b = p; break;
      case 1:
        r = q; g = v; b = p; break;
      case 2:
        r = p; g = v; b = t; break;
      case 3:
        r = p; g = q; b = v; break;
      case 4:
        r = t; g = p; b = v; break;
      case 5:
        r = v; g = p; b = q; break;
      default:
        r = 0; g = 0; b = 0; break;
    }

    return Color(
      (r * 255).round(),
      (g * 255).round(),
      (b * 255).round(),
      (w * 255).round(),
      100,
    );
  }

  // 调整亮度
  static Color adjustBrightness(Color color, double factor) {
    return Color(
      (color.r * factor).round().clamp(0, 255),
      (color.g * factor).round().clamp(0, 255),
      (color.b * factor).round().clamp(0, 255),
      (color.w * factor).round().clamp(0, 255),
      color.brightness,
    );
  }
}
