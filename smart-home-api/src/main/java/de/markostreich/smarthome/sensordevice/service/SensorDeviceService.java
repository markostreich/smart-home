package de.markostreich.smarthome.sensordevice.service;

import de.markostreich.smarthome.sensordevice.model.HumidityObject;
import de.markostreich.smarthome.sensordevice.model.SensorDevice;
import de.markostreich.smarthome.sensordevice.model.dto.SensorDeviceDto;
import de.markostreich.smarthome.sensordevice.model.repo.HumidityObjectRepository;
import de.markostreich.smarthome.sensordevice.model.repo.SensorDeviceRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.sql.Timestamp;
import java.time.Instant;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

import static java.util.Objects.isNull;

@Service
@RequiredArgsConstructor
@Slf4j
public class SensorDeviceService {

    private final SensorDeviceRepository deviceRepository;
    private final HumidityObjectRepository humidityObjectRepository;

    public boolean connectDevice(final String deviceName) {
        val now = Timestamp.from(Instant.now());
        if (deviceRepository.updateLastLoginByName(deviceName, now) > 0) {
            log.debug("Device '{}' connected.", deviceName);
            return false;
        }

        try {
            log.info("Adding new device '{}'.", deviceName);
            val createdDevice = SensorDevice.builder().name(deviceName).lastLogin(now)
                    .build();
            deviceRepository.save(createdDevice);
            log.info("Added new device '{}'.", deviceName);
            return true;
        } catch (final DataIntegrityViolationException exception) {
            log.debug("Device '{}' was created concurrently. Updating login.",
                    deviceName);
            deviceRepository.updateLastLoginByName(deviceName,
                    Timestamp.from(Instant.now()));
            return false;
        }
    }

    @Transactional(readOnly = true)
    public List<SensorDeviceDto> getAllDevices() {
        val deviceIterator = deviceRepository.findAll();
        val deviceDtos = new ArrayList<SensorDeviceDto>();
        deviceIterator.forEach(device -> {
            val humidityObjects = humidityObjectRepository.findByDevice(device);
            deviceDtos.add(new SensorDeviceDto(device.getName(),
                    getHumidityObjectValues(humidityObjects)));
        });
        return deviceDtos;
    }

    @Transactional
    public boolean deleteDevice(final String deviceName) {
        val deviceId = deviceRepository.findIdByName(deviceName);
        if (isNull(deviceId))
            return false;
        humidityObjectRepository.deleteByDeviceIdIn(List.of(deviceId));
        return deviceRepository.deleteByIdIn(List.of(deviceId)) > 0;
    }

    private Set<String> getHumidityObjectValues(
            final List<HumidityObject> humidityObjects) {
        if (isNull(humidityObjects))
            return new HashSet<>();
        return humidityObjects.parallelStream()
                .map(humidityObject -> String.format("%.1f degree, %.1f percent, %s",
                        humidityObject.getTemperature(),
                        humidityObject.getHumidity(),
                        humidityObject.getTimestamp().toString()))
                .collect(Collectors.toSet());
    }
}
