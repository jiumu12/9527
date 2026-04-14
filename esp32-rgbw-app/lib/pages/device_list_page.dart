import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:esp32_rgbw_app/controllers/device_controller.dart';
import 'package:esp32_rgbw_app/controllers/light_controller.dart';
import 'package:esp32_rgbw_app/pages/control_page.dart';

class DeviceListPage extends StatefulWidget {
  const DeviceListPage({super.key});

  @override
  State<DeviceListPage> createState() => _DeviceListPageState();
}

class _DeviceListPageState extends State<DeviceListPage> {
  @override
  void initState() {
    super.initState();
    // 初始化时刷新设备列表
    WidgetsBinding.instance.addPostFrameCallback((_) {
      Provider.of<DeviceController>(context, listen: false).refreshDevices();
    });
  }

  @override
  Widget build(BuildContext context) {
    final deviceController = Provider.of<DeviceController>(context);

    return Scaffold(
      appBar: AppBar(
        title: const Text('ESP32 RGBW Control'),
        actions: [
          IconButton(
            icon: const Icon(Icons.refresh),
            onPressed: () {
              deviceController.refreshDevices();
            },
          ),
        ],
      ),
      body: Center(
        child: deviceController.isScanning
            ? const CircularProgressIndicator()
            : deviceController.devices.isEmpty
                ? const Text('No devices found. Tap refresh to scan again.')
                : ListView.builder(
                    itemCount: deviceController.devices.length,
                    itemBuilder: (context, index) {
                      final device = deviceController.devices[index];
                      return Card(
                        margin: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
                        child: ListTile(
                          title: Text(device.name),
                          subtitle: Text('${device.ipAddress}:${device.port}'),
                          trailing: device.isConnected
                              ? const Icon(Icons.check_circle, color: Colors.green)
                              : null,
                          onTap: () async {
                            try {
                              await deviceController.connectToDevice(device);
                              // 设置LightController的WebSocketService并获取状态
                              final lightController = Provider.of<LightController>(context, listen: false);
                              lightController.setWebSocketService(deviceController.webSocketService);
                              lightController.getStatus();
                              // 导航到控制页面
                              Navigator.push(
                                context,
                                MaterialPageRoute(
                                  builder: (context) => ControlPage(device: device),
                                ),
                              );
                            } catch (e) {
                              // 显示连接错误
                              ScaffoldMessenger.of(context).showSnackBar(
                                SnackBar(
                                  content: Text('Failed to connect: $e'),
                                ),
                              );
                            }
                          },
                        ),
                      );
                    },
                  ),
      ),
    );
  }
}
