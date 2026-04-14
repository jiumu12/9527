// 效果选择器组件

import 'package:flutter/material.dart';
import '../models/effect.dart';

class EffectSelectorWidget extends StatefulWidget {
  final EffectType initialEffect;
  final int initialSpeed;
  final Function(EffectType, int) onEffectChanged;

  const EffectSelectorWidget({
    Key? key,
    required this.initialEffect,
    required this.initialSpeed,
    required this.onEffectChanged,
  }) : super(key: key);

  @override
  _EffectSelectorWidgetState createState() => _EffectSelectorWidgetState();
}

class _EffectSelectorWidgetState extends State<EffectSelectorWidget> {
  late EffectType currentEffect;
  late int currentSpeed;

  @override
  void initState() {
    super.initState();
    currentEffect = widget.initialEffect;
    currentSpeed = widget.initialSpeed;
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text(
          '效果选择',
          style: TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.w500,
          ),
        ),
        const SizedBox(height: 16),
        // 效果类型选择
        DropdownButtonFormField<EffectType>(
          value: currentEffect,
          onChanged: (value) {
            if (value != null) {
              setState(() {
                currentEffect = value;
                widget.onEffectChanged(currentEffect, currentSpeed);
              });
            }
          },
          items: EffectType.values.map((effect) {
            return DropdownMenuItem<EffectType>(
              value: effect,
              child: Text(effectToString(effect)),
            );
          }).toList(),
          decoration: const InputDecoration(
            labelText: '效果类型',
            border: OutlineInputBorder(),
          ),
        ),
        const SizedBox(height: 16),
        // 速度调节
        Row(
          children: [
            const Text('速度:'),
            const SizedBox(width: 16),
            Expanded(
              child: Slider(
                value: currentSpeed.toDouble(),
                min: 0,
                max: 100,
                onChanged: (value) {
                  setState(() {
                    currentSpeed = value.round();
                    widget.onEffectChanged(currentEffect, currentSpeed);
                  });
                },
              ),
            ),
            const SizedBox(width: 16),
            Text(currentSpeed.toString()),
          ],
        ),
      ],
    );
  }

  // 效果类型转字符串
  String effectToString(EffectType effect) {
    switch (effect) {
      case EffectType.staticColor:
        return '静态颜色';
      case EffectType.breathing:
        return '呼吸效果';
      case EffectType.rainbow:
        return '彩虹效果';
      case EffectType.flashing:
        return '闪烁效果';
      default:
        return '未知效果';
    }
  }
}
