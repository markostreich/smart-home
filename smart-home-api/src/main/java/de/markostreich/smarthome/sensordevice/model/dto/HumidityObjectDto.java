package de.markostreich.smarthome.sensordevice.model.dto;

import java.time.LocalDateTime;

public record HumidityObjectDto(LocalDateTime timestamp, float temperature, float humidity, String deviceName) {
}
