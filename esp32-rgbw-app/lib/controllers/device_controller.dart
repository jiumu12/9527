import 'package:flutter/foundation.dart';
import 'package:esp32_rgbw_app/models/device.dart';
import 'package:esp32_rgbw_app/services/discovery_service.dart';
import 'package:esp32_rgbw_app/services/websocket_service.dart';

class DeviceController extends ChangeNotifier {
  final DiscoveryService _discoveryService = DiscoveryService();
  final WebSocketService _webSocketService = WebSocketService();
  List<Device> _devices = [];
  Device? _currentDevice;
  bool _isScanning = false;

  List<Device> get devices => _devices;
  Device? get currentDevice => _currentDevice;
  bool get isScanning => _isScanning;
  bool get isConnected => _webSocketService.isConnected;

  DeviceController() {
    _startDiscovery();
  }

  Future<void> _startDiscovery() async {
    _isScanning = true;
    notifyListeners();

    try {
      await _discoveryService.startDiscovery();
      _discoveryService.devicesStream.listen((devices) {
        _devices = devices;
        notifyListeners();
      });
    } catch (e) {
      print('Error during discovery: $e');
    } finally {
      _isScanning = false;
      notifyListeners();
    }
  }

  Future<void> refreshDevices() async {
    _isScanning = true;
    notifyListeners();

    try {
      await _discoveryService.refreshDevices();
    } catch (e) {
      print('Error refreshing devices: $e');
    } finally {
      _isScanning = false;
      notifyListeners();
    }
  }

  Future<void> connectToDevice(Device device) async {
    try {
      await _webSocketService.connect(device.ipAddress, device.port);
      _currentDevice = device;
      device.isConnected = true;
      notifyListeners();
    } catch (e) {
      print('Error connecting to device: $e');
      throw e;
    }
  }

  void disconnectFromDevice() {
    _webSocketService.disconnect();
    if (_currentDevice != null) {
      _currentDevice!.isConnected = false;
    }
    _currentDevice = null;
    notifyListeners();
  }

  WebSocketService get webSocketService => _webSocketService;

  @override
  void dispose() {
    _discoveryService.dispose();
    _webSocketService.disconnect();
    super.dispose();
  }
}
