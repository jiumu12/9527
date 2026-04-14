import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:flutter_colorpicker/flutter_colorpicker.dart';
import 'package:esp32_rgbw_app/models/device.dart';
import 'package:esp32_rgbw_app/models/color.dart';
import 'package:esp32_rgbw_app/models/effect.dart';
import 'package:esp32_rgbw_app/controllers/light_controller.dart';
import 'package:esp32_rgbw_app/controllers/device_controller.dart';
import 'dart:async';

class ControlPage extends StatefulWidget {
  final Device device;

  const ControlPage({super.key, required this.device});

  @override
  State<ControlPage> createState() => _ControlPageState();
}

class _ControlPageState extends State<ControlPage> {
  bool _showColorPicker = false;
  Timer? _debounceTimer;
  final Duration _debounceDuration = const Duration(milliseconds: 200);

  void _debounce(VoidCallback callback) {
    if (_debounceTimer != null) {
      _debounceTimer?.cancel();
    }
    _debounceTimer = Timer(_debounceDuration, callback);
  }

  @override
  void dispose() {
    _debounceTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final lightController = Provider.of<LightController>(context);
    final deviceController = Provider.of<DeviceController>(context);
    final effect = lightController.currentEffect;

    return Scaffold(
      appBar: AppBar(
        title: Text('Control ${widget.device.name}'),
        actions: [
          IconButton(
            icon: const Icon(Icons.logout),
            onPressed: () {
              deviceController.disconnectFromDevice();
              Navigator.pop(context);
            },
          ),
        ],
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            // 总开关
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Row(
                  mainAxisAlignment: MainAxisAlignment.spaceBetween,
                  children: [
                    const Text('Power', style: TextStyle(fontSize: 18)),
                    Switch(
                      value: lightController.isOn,
                      onChanged: (value) {
                        lightController.setPower(value);
                      },
                    ),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 20),

            // 效果选择
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Effect', style: TextStyle(fontSize: 18)),
                    const SizedBox(height: 10),
                    Row(
                      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
                      children: EffectType.values.map((type) {
                        return ElevatedButton(
                          onPressed: () {
                            lightController.updateEffectType(type);
                          },
                          style: ElevatedButton.styleFrom(
                            backgroundColor: effect.type == type
                                ? Theme.of(context).primaryColor
                                : null,
                          ),
                          child: Text(type.toString().split('.').last),
                        );
                      }).toList(),
                    ),
                    if (effect.type != EffectType.static)
                      Column(
                        children: [
                          const SizedBox(height: 10),
                          Row(
                            children: [
                              const Text('Speed: '),
                              Expanded(
                                child: Slider(
                                  value: effect.speed.toDouble(),
                                  min: 0,
                                  max: 100,
                                  onChanged: (value) {
                                    _debounce(() {
                                      lightController.updateSpeed(value.round());
                                    });
                                  },
                                ),
                              ),
                              Text('${effect.speed}%'),
                            ],
                          ),
                        ],
                      ),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 20),

            // 颜色选择
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Color', style: TextStyle(fontSize: 18)),
                    const SizedBox(height: 10),
                    GestureDetector(
                      onTap: () {
                        setState(() {
                          _showColorPicker = true;
                        });
                      },
                      child: Container(
                        width: double.infinity,
                        height: 100,
                        decoration: BoxDecoration(
                          color: effect.color.toFlutterColor(),
                          borderRadius: BorderRadius.circular(8),
                        ),
                        child: const Center(
                          child: Text('Tap to pick color'),
                        ),
                      ),
                    ),
                    const SizedBox(height: 10),
                    // RGBW滑块
                    Row(
                      children: [
                        Expanded(
                          child: Column(
                            children: [
                              const Text('R'),
                              Slider(
                                value: effect.color.r.toDouble(),
                                min: 0,
                                max: 255,
                                onChanged: (value) {
                                  _debounce(() {
                                    final newColor = ColorModel(
                                      r: value.round(),
                                      g: effect.color.g,
                                      b: effect.color.b,
                                      w: effect.color.w,
                                      brightness: effect.color.brightness,
                                    );
                                    lightController.updateColor(newColor);
                                  });
                                },
                                activeColor: Colors.red,
                              ),
                              Text('${effect.color.r}'),
                            ],
                          ),
                        ),
                        Expanded(
                          child: Column(
                            children: [
                              const Text('G'),
                              Slider(
                                value: effect.color.g.toDouble(),
                                min: 0,
                                max: 255,
                                onChanged: (value) {
                                  _debounce(() {
                                    final newColor = ColorModel(
                                      r: effect.color.r,
                                      g: value.round(),
                                      b: effect.color.b,
                                      w: effect.color.w,
                                      brightness: effect.color.brightness,
                                    );
                                    lightController.updateColor(newColor);
                                  });
                                },
                                activeColor: Colors.green,
                              ),
                              Text('${effect.color.g}'),
                            ],
                          ),
                        ),
                        Expanded(
                          child: Column(
                            children: [
                              const Text('B'),
                              Slider(
                                value: effect.color.b.toDouble(),
                                min: 0,
                                max: 255,
                                onChanged: (value) {
                                  _debounce(() {
                                    final newColor = ColorModel(
                                      r: effect.color.r,
                                      g: effect.color.g,
                                      b: value.round(),
                                      w: effect.color.w,
                                      brightness: effect.color.brightness,
                                    );
                                    lightController.updateColor(newColor);
                                  });
                                },
                                activeColor: Colors.blue,
                              ),
                              Text('${effect.color.b}'),
                            ],
                          ),
                        ),
                        Expanded(
                          child: Column(
                            children: [
                              const Text('W'),
                              Slider(
                                value: effect.color.w.toDouble(),
                                min: 0,
                                max: 255,
                                onChanged: (value) {
                                  _debounce(() {
                                    final newColor = ColorModel(
                                      r: effect.color.r,
                                      g: effect.color.g,
                                      b: effect.color.b,
                                      w: value.round(),
                                      brightness: effect.color.brightness,
                                    );
                                    lightController.updateColor(newColor);
                                  });
                                },
                                activeColor: Colors.white,
                              ),
                              Text('${effect.color.w}'),
                            ],
                          ),
                        ),
                      ],
                    ),
                  ],
                ),
              ),
            ),

            const SizedBox(height: 20),

            // 亮度控制
            Card(
              child: Padding(
                padding: const EdgeInsets.all(16.0),
                child: Column(
                  crossAxisAlignment: CrossAxisAlignment.start,
                  children: [
                    const Text('Brightness', style: TextStyle(fontSize: 18)),
                    const SizedBox(height: 10),
                    Row(
                      children: [
                        const Icon(Icons.brightness_low),
                        Expanded(
                          child: Slider(
                            value: effect.color.brightness.toDouble(),
                            min: 0,
                            max: 100,
                            onChanged: (value) {
                              _debounce(() {
                                final newColor = ColorModel(
                                  r: effect.color.r,
                                  g: effect.color.g,
                                  b: effect.color.b,
                                  w: effect.color.w,
                                  brightness: value.round(),
                                );
                                lightController.updateColor(newColor);
                              });
                            },
                          ),
                        ),
                        const Icon(Icons.brightness_high),
                        Text('${effect.color.brightness}%'),
                      ],
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
      // 颜色选择器对话框
      floatingActionButton: _showColorPicker
          ? null
          : FloatingActionButton(
              onPressed: () {
                setState(() {
                  _showColorPicker = true;
                });
              },
              child: const Icon(Icons.color_lens),
            ),
      // 颜色选择器
      ...(_showColorPicker
          ? [
              AlertDialog(
                title: const Text('Pick a color'),
                content: SingleChildScrollView(
                  child: ColorPicker(
                    pickerColor: effect.color.toFlutterColor(),
                    onColorChanged: (color) {
                      final newColor = ColorModel.fromFlutterColor(
                        color,
                        w: effect.color.w,
                        brightness: effect.color.brightness,
                      );
                      lightController.updateColor(newColor);
                    },
                    showLabel: true,
                    pickerAreaHeightPercent: 0.8,
                  ),
                ),
                actions: [
                  TextButton(
                    onPressed: () {
                      setState(() {
                        _showColorPicker = false;
                      });
                    },
                    child: const Text('Done'),
                  ),
                ],
              ),
            ]
          : []),
    );
  }
}
