// 网络工具类

import 'dart:io';
import '../constants/app_constants.dart';

class NetworkUtils {
  // 构建WebSocket URL
  static String buildWebSocketUrl(String ipAddress) {
    return 'ws://$ipAddress:${AppConstants.websocketPort}/ws';
  }

  // 构建HTTP URL
  static String buildHttpUrl(String ipAddress) {
    return 'http://$ipAddress';
  }

  // 检查网络连接
  static Future<bool> checkNetworkConnection() async {
    try {
      final result = await InternetAddress.lookup('google.com');
      return result.isNotEmpty && result[0].rawAddress.isNotEmpty;
    } on SocketException catch (_) {
      return false;
    }
  }

  // 验证IP地址格式
  static bool isValidIpAddress(String ipAddress) {
    try {
      final parts = ipAddress.split('.');
      if (parts.length != 4) return false;
      for (final part in parts) {
        final intValue = int.tryParse(part);
        if (intValue == null || intValue < 0 || intValue > 255) return false;
      }
      return true;
    } catch (_) {
      return false;
    }
  }

  // 格式化MAC地址
  static String formatMacAddress(String macAddress) {
    // 移除所有非十六进制字符
    final cleanMac = macAddress.replaceAll(RegExp(r'[^0-9A-Fa-f]'), '');
    // 每两个字符添加一个冒号
    final formatted = List.generate(
      cleanMac.length ~/ 2,
      (i) => cleanMac.substring(i * 2, i * 2 + 2),
    ).join(':');
    return formatted.toUpperCase();
  }
}
