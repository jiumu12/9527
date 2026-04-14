import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:esp32_rgbw_app/controllers/device_controller.dart';
import 'package:esp32_rgbw_app/pages/device_list_page.dart';

void main() {
  runApp(
    MultiProvider(
      providers: [
        ChangeNotifierProvider(create: (_) => DeviceController()),
      ],
      child: const MyApp(),
    ),
  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'ESP32 RGBW Control',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: const DeviceListPage(),
    );
  }
}
