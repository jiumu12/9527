import 'dart:async';
import 'package:mdns/mdns.dart';
import 'package:esp32_rgbw_app/models/device.dart';

class DiscoveryService {
  late MDnsClient _client;
  final List<Device> _devices = [];
  final StreamController<List<Device>> _devicesController = StreamController<List<Device>>.broadcast();

  Stream<List<Device>> get devicesStream => _devicesController.stream;

  Future<void> startDiscovery() async {
    _client = MDnsClient();
    await _client.start();
    
    // 搜索ESP32设备
    const String serviceType = '_ws._tcp.local';
    await for (final PtrResourceRecord ptr in _client.lookup<PtrResourceRecord>(
      ResourceRecordQuery.service(serviceType),
    )) {
      await for (final SrvResourceRecord srv in _client.lookup<SrvResourceRecord>(
        ResourceRecordQuery.service(ptr.domainName),
      )) {
        final String name = ptr.domainName.split('.').first;
        final String target = srv.target;
        final int port = srv.port;
        
        // 解析IP地址
        await for (final IPAddressResourceRecord ip in _client.lookup<IPAddressResourceRecord>(
          ResourceRecordQuery.addressIPv4(target),
        )) {
          final String ipAddress = ip.address.address;
          
          final device = Device(
            name: name,
            ipAddress: ipAddress,
            port: port,
          );
          
          if (!_devices.contains(device)) {
            _devices.add(device);
            _devicesController.add([..._devices]);
          }
        }
      }
    }
  }

  Future<void> stopDiscovery() async {
    await _client.stop();
    _devices.clear();
    _devicesController.add([]);
  }

  Future<void> refreshDevices() async {
    _devices.clear();
    await startDiscovery();
  }

  void dispose() {
    _client.stop();
    _devicesController.close();
  }
}
