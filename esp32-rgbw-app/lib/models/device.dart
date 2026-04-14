class Device {
  final String name;
  final String ipAddress;
  final int port;
  bool isConnected;

  Device({
    required this.name,
    required this.ipAddress,
    this.port = 8080,
    this.isConnected = false,
  });

  factory Device.fromMap(Map<String, dynamic> map) {
    return Device(
      name: map['name'] ?? '',
      ipAddress: map['ipAddress'] ?? '',
      port: map['port'] ?? 8080,
      isConnected: map['isConnected'] ?? false,
    );
  }

  Map<String, dynamic> toMap() {
    return {
      'name': name,
      'ipAddress': ipAddress,
      'port': port,
      'isConnected': isConnected,
    };
  }

  @override
  String toString() {
    return 'Device(name: $name, ipAddress: $ipAddress, port: $port, isConnected: $isConnected)';
  }

  @override
  bool operator ==(Object other) {
    if (identical(this, other)) return true;
    return other is Device &&
        other.name == name &&
        other.ipAddress == ipAddress &&
        other.port == port;
  }

  @override
  int get hashCode {
    return name.hashCode ^ ipAddress.hashCode ^ port.hashCode;
  }
}
