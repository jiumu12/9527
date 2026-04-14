import 'package:flutter/material.dart';
import 'package:esp32_rgbw_app/models/device.dart';
import 'package:http/http.dart' as http;
import 'dart:convert';

class ConfigPage extends StatefulWidget {
  final Device device;

  const ConfigPage({super.key, required this.device});

  @override
  State<ConfigPage> createState() => _ConfigPageState();
}

class _ConfigPageState extends State<ConfigPage> {
  final _formKey = GlobalKey<FormState>();
  int _wifiMode = 0;
  String _apSSID = 'ESP32-RGBW';
  String _apPassword = '12345678';
  String _stationSSID = '';
  String _stationPassword = '';
  int _ledPin = 7;
  int _ledCount = 30;
  int _ledBrightness = 255;
  bool _isLoading = false;
  String? _statusMessage;
  bool _statusSuccess = false;

  @override
  void initState() {
    super.initState();
    _loadConfig();
  }

  Future<void> _loadConfig() async {
    setState(() {
      _isLoading = true;
      _statusMessage = '加载配置中...';
    });

    try {
      final response = await http.get(
        Uri.parse('http://${widget.device.ipAddress}/get-config'),
        headers: {'Content-Type': 'application/json'},
      );

      if (response.statusCode == 200) {
        final data = json.decode(response.body);
        if (data['status'] == 'ok') {
          final config = data['config'];
          setState(() {
            _wifiMode = config['wifiMode'];
            _apSSID = config['apSSID'];
            _apPassword = config['apPassword'];
            _stationSSID = config['stationSSID'];
            _stationPassword = config['stationPassword'];
            _ledPin = config['ledPin'];
            _ledCount = config['ledCount'];
            _ledBrightness = config['ledBrightness'];
            _isLoading = false;
          });
        } else {
          throw Exception('获取配置失败');
        }
      } else {
        throw Exception('网络请求失败: ${response.statusCode}');
      }
    } catch (e) {
      setState(() {
        _isLoading = false;
        _statusMessage = '加载配置失败: $e';
        _statusSuccess = false;
      });
    }
  }

  Future<void> _saveConfig() async {
    if (_formKey.currentState!.validate()) {
      _formKey.currentState!.save();

      setState(() {
        _isLoading = true;
        _statusMessage = '保存配置中...';
      });

      try {
        final response = await http.post(
          Uri.parse('http://${widget.device.ipAddress}/save-config'),
          headers: {'Content-Type': 'application/json'},
          body: json.encode({
            'wifiMode': _wifiMode,
            'apSSID': _apSSID,
            'apPassword': _apPassword,
            'stationSSID': _stationSSID,
            'stationPassword': _stationPassword,
            'ledPin': _ledPin,
            'ledCount': _ledCount,
            'ledBrightness': _ledBrightness,
          }),
        );

        if (response.statusCode == 200) {
          final data = json.decode(response.body);
          if (data['status'] == 'ok') {
            setState(() {
              _isLoading = false;
              _statusMessage = '配置保存成功，设备正在重启...';
              _statusSuccess = true;
            });

            // 3秒后返回
            Future.delayed(const Duration(seconds: 3), () {
              Navigator.pop(context);
            });
          } else {
            throw Exception('保存配置失败: ${data['message']}');
          }
        } else {
          throw Exception('网络请求失败: ${response.statusCode}');
        }
      } catch (e) {
        setState(() {
          _isLoading = false;
          _statusMessage = '保存配置失败: $e';
          _statusSuccess = false;
        });
      }
    }
  }

  Future<void> _resetConfig() async {
    if (await showDialog(
          context: context,
          builder: (context) => AlertDialog(
            title: const Text('恢复默认配置'),
            content: const Text('确定要恢复默认配置吗？'),
            actions: [
              TextButton(
                onPressed: () => Navigator.pop(context, false),
                child: const Text('取消'),
              ),
              TextButton(
                onPressed: () => Navigator.pop(context, true),
                child: const Text('确定'),
              ),
            ],
          ),
        ) ==
        true) {
      setState(() {
        _isLoading = true;
        _statusMessage = '恢复默认配置中...';
      });

      try {
        final response = await http.post(
          Uri.parse('http://${widget.device.ipAddress}/reset-config'),
          headers: {'Content-Type': 'application/json'},
        );

        if (response.statusCode == 200) {
          final data = json.decode(response.body);
          if (data['status'] == 'ok') {
            setState(() {
              _isLoading = false;
              _statusMessage = '已恢复默认配置，设备正在重启...';
              _statusSuccess = true;
            });

            // 3秒后返回
            Future.delayed(const Duration(seconds: 3), () {
              Navigator.pop(context);
            });
          } else {
            throw Exception('恢复默认配置失败: ${data['message']}');
          }
        } else {
          throw Exception('网络请求失败: ${response.statusCode}');
        }
      } catch (e) {
        setState(() {
          _isLoading = false;
          _statusMessage = '恢复默认配置失败: $e';
          _statusSuccess = false;
        });
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('设备配置'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Form(
          key: _formKey,
          child: ListView(
            children: [
              // WiFi模式
              DropdownButtonFormField<int>(
                value: _wifiMode,
                decoration: const InputDecoration(labelText: 'WiFi 模式'),
                items: const [
                  DropdownMenuItem(value: 0, child: Text('AP模式 (设备作为热点)')),
                  DropdownMenuItem(value: 1, child: Text('Station模式 (连接到路由器)')),
                ],
                onChanged: (value) => setState(() => _wifiMode = value!),
                validator: (value) => value == null ? '请选择WiFi模式' : null,
              ),

              // AP模式配置
              const SizedBox(height: 20),
              const Text('AP模式配置', style: TextStyle(fontWeight: FontWeight.bold)),
              TextFormField(
                initialValue: _apSSID,
                decoration: const InputDecoration(labelText: 'AP模式 SSID'),
                validator: (value) => value!.isEmpty ? '请输入AP SSID' : null,
                onSaved: (value) => _apSSID = value!,
              ),
              TextFormField(
                initialValue: _apPassword,
                decoration: const InputDecoration(labelText: 'AP模式 密码'),
                validator: (value) => value!.length < 8 ? '密码长度至少8位' : null,
                onSaved: (value) => _apPassword = value!,
              ),

              // Station模式配置
              const SizedBox(height: 20),
              const Text('Station模式配置', style: TextStyle(fontWeight: FontWeight.bold)),
              TextFormField(
                initialValue: _stationSSID,
                decoration: const InputDecoration(labelText: 'Station模式 SSID'),
                onSaved: (value) => _stationSSID = value!,
              ),
              TextFormField(
                initialValue: _stationPassword,
                decoration: const InputDecoration(labelText: 'Station模式 密码'),
                onSaved: (value) => _stationPassword = value!,
              ),

              // LED配置
              const SizedBox(height: 20),
              const Text('LED配置', style: TextStyle(fontWeight: FontWeight.bold)),
              TextFormField(
                initialValue: _ledPin.toString(),
                decoration: const InputDecoration(labelText: 'LED 引脚'),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value!.isEmpty) return '请输入LED引脚';
                  int pin = int.tryParse(value) ?? -1;
                  if (pin < 0 || pin > 39) return '引脚范围0-39';
                  return null;
                },
                onSaved: (value) => _ledPin = int.parse(value!),
              ),
              TextFormField(
                initialValue: _ledCount.toString(),
                decoration: const InputDecoration(labelText: 'LED 数量'),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value!.isEmpty) return '请输入LED数量';
                  int count = int.tryParse(value) ?? 0;
                  if (count < 1 || count > 100) return '数量范围1-100';
                  return null;
                },
                onSaved: (value) => _ledCount = int.parse(value!),
              ),
              TextFormField(
                initialValue: _ledBrightness.toString(),
                decoration: const InputDecoration(labelText: 'LED 最大亮度'),
                keyboardType: TextInputType.number,
                validator: (value) {
                  if (value!.isEmpty) return '请输入亮度';
                  int brightness = int.tryParse(value) ?? -1;
                  if (brightness < 0 || brightness > 255) return '亮度范围0-255';
                  return null;
                },
                onSaved: (value) => _ledBrightness = int.parse(value!),
              ),

              // 按钮
              const SizedBox(height: 30),
              ElevatedButton(
                onPressed: _isLoading ? null : _saveConfig,
                style: ElevatedButton.styleFrom(
                  minimumSize: const Size(double.infinity, 50),
                ),
                child: _isLoading
                    ? const CircularProgressIndicator(color: Colors.white)
                    : const Text('保存配置'),
              ),
              const SizedBox(height: 10),
              OutlinedButton(
                onPressed: _isLoading ? null : _resetConfig,
                style: OutlinedButton.styleFrom(
                  minimumSize: const Size(double.infinity, 50),
                ),
                child: const Text('恢复默认'),
              ),

              // 状态消息
              if (_statusMessage != null)
                Container(
                  margin: const EdgeInsets.only(top: 20),
                  padding: const EdgeInsets.all(10),
                  decoration: BoxDecoration(
                    color: _statusSuccess ? Colors.green[100] : Colors.red[100],
                    borderRadius: BorderRadius.circular(4),
                  ),
                  child: Text(
                    _statusMessage!,
                    style: TextStyle(
                      color: _statusSuccess ? Colors.green[800] : Colors.red[800],
                    ),
                  ),
                ),
            ],
          ),
        ),
      ),
    );
  }
}
