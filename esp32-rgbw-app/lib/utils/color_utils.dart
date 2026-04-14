// 颜色工具类

import '../models/color.dart';

class ColorUtils {
  // 颜色转换缓存
  static final Map<String, Map<String, double>> _rgbwToHsvCache = {};
  static final Map<String, ColorModel> _hsvToRgbwCache = {};

  // RGBW转HSV
  static Map<String, double> rgbwToHsv(ColorModel color) {
    // 生成缓存键
    String cacheKey = '${color.r},${color.g},${color.b},${color.w}';
    
    // 检查缓存
    if (_rgbwToHsvCache.containsKey(cacheKey)) {
      return _rgbwToHsvCache[cacheKey]!;
    }

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

    Map<String, double> result = {
      'h': h,
      's': s * 100,
      'v': v * 100,
      'w': w * 100,
    };

    // 缓存结果
    _rgbwToHsvCache[cacheKey] = result;
    // 限制缓存大小
    if (_rgbwToHsvCache.length > 100) {
      _rgbwToHsvCache.remove(_rgbwToHsvCache.keys.first);
    }

    return result;
  }

  // HSV转RGBW
  static ColorModel hsvToRgbw(double h, double s, double v, double w) {
    // 生成缓存键
    String cacheKey = '${h.round()},${s.round()},${v.round()},${w.round()}';
    
    // 检查缓存
    if (_hsvToRgbwCache.containsKey(cacheKey)) {
      return _hsvToRgbwCache[cacheKey]!;
    }

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

    // 优化RGBW转换，与ESP32端保持一致
    int rInt = (r * 255).round();
    int gInt = (g * 255).round();
    int bInt = (b * 255).round();
    int wInt = (w * 255).round();

    // 利用白色通道提高亮度
    int minRgb = [rInt, gInt, bInt].reduce((a, b) => a < b ? a : b);
    rInt -= minRgb;
    gInt -= minRgb;
    bInt -= minRgb;
    wInt += minRgb;

    // 确保值在有效范围内
    rInt = rInt.clamp(0, 255);
    gInt = gInt.clamp(0, 255);
    bInt = bInt.clamp(0, 255);
    wInt = wInt.clamp(0, 255);

    ColorModel result = ColorModel(
      r: rInt,
      g: gInt,
      b: bInt,
      w: wInt,
      brightness: 100,
    );

    // 缓存结果
    _hsvToRgbwCache[cacheKey] = result;
    // 限制缓存大小
    if (_hsvToRgbwCache.length > 100) {
      _hsvToRgbwCache.remove(_hsvToRgbwCache.keys.first);
    }

    return result;
  }

  // 调整亮度
  static ColorModel adjustBrightness(ColorModel color, double factor) {
    return ColorModel(
      r: (color.r * factor).round().clamp(0, 255),
      g: (color.g * factor).round().clamp(0, 255),
      b: (color.b * factor).round().clamp(0, 255),
      w: (color.w * factor).round().clamp(0, 255),
      brightness: color.brightness,
    );
  }

  // 清除缓存
  static void clearCache() {
    _rgbwToHsvCache.clear();
    _hsvToRgbwCache.clear();
  }
}
