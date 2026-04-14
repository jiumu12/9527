// 颜色选择器组件

import 'package:flutter/material.dart';
import 'package:flutter_colorpicker/flutter_colorpicker.dart';
import '../models/color.dart';
import '../utils/color_utils.dart';

class ColorPickerWidget extends StatefulWidget {
  final Color initialColor;
  final Function(Color) onColorChanged;

  const ColorPickerWidget({
    Key? key,
    required this.initialColor,
    required this.onColorChanged,
  }) : super(key: key);

  @override
  _ColorPickerWidgetState createState() => _ColorPickerWidgetState();
}

class _ColorPickerWidgetState extends State<ColorPickerWidget> {
  late Color currentColor;
  late double whiteValue;

  @override
  void initState() {
    super.initState();
    currentColor = widget.initialColor;
    whiteValue = widget.initialColor.w.toDouble();
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        const Text(
          '颜色选择',
          style: TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.w500,
          ),
        ),
        const SizedBox(height: 16),
        // RGB颜色选择器
        ColorPicker(
          pickerColor: Color.fromARGB(
            255,
            currentColor.r,
            currentColor.g,
            currentColor.b,
          ),
          onColorChanged: (color) {
            setState(() {
              currentColor = Color(
                color.red,
                color.green,
                color.blue,
                currentColor.w,
                currentColor.brightness,
              );
              widget.onColorChanged(currentColor);
            });
          },
          pickerAreaHeightPercent: 0.7,
          enableAlpha: false,
          showLabel: true,
          paletteType: PaletteType.hsv,
        ),
        const SizedBox(height: 16),
        // 白色通道调节
        Row(
          children: [
            const Text('白色通道:'),
            const SizedBox(width: 16),
            Expanded(
              child: Slider(
                value: whiteValue,
                min: 0,
                max: 255,
                onChanged: (value) {
                  setState(() {
                    whiteValue = value;
                    currentColor = Color(
                      currentColor.r,
                      currentColor.g,
                      currentColor.b,
                      value.round(),
                      currentColor.brightness,
                    );
                    widget.onColorChanged(currentColor);
                  });
                },
              ),
            ),
            const SizedBox(width: 16),
            Text(whiteValue.round().toString()),
          ],
        ),
      ],
    );
  }
}
