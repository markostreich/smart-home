package de.markostreich.smarthome.sensordevice.model.dto;

import java.util.Set;

public record SensorDeviceDto(
        String name,
        Set<String> humidityObjects) {
}
