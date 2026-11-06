package de.markostreich.smarthome.sensordevice;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.sensordevice.model.HumidityObject;
import de.markostreich.smarthome.sensordevice.model.dto.HumidityObjectDto;
import de.markostreich.smarthome.sensordevice.model.repo.HumidityObjectRepository;
import de.markostreich.smarthome.sensordevice.model.repo.SensorDeviceRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import java.sql.Timestamp;
import java.time.Instant;

import static java.util.Objects.isNull;

@RestController
@RequestMapping("/sensor/humidity")
@RequiredArgsConstructor
@Slf4j
public class HumidityController {

    private final HumidityObjectRepository humidityObjectRepository;
    private final SensorDeviceRepository deviceRepository;

    @PostMapping(path = "/object", consumes = "application/json", produces = "application/json")
    private ResponseEntity<HumidityObjectDto> addHumidityObject(
            @RequestBody final HumidityObjectDto humidityObjectDto) {
        log.debug("Attempting to add Humidity Object for sensor device '{}'.", humidityObjectDto.deviceName());
        log.info("Received humidity object: {}", humidityObjectDto);
        val device = deviceRepository.findByName(humidityObjectDto.deviceName());
        if (isNull(device)) {
            log.warn("Could not find device '{}'.", humidityObjectDto.deviceName());
            return ResponseEntity.notFound().build();
        }
        deviceRepository.updateLastLoginByName(humidityObjectDto.deviceName(), Timestamp.from(Instant.now()));
        var createdHumidityObject = HumidityObject.builder()
                .humidity(humidityObjectDto.humidity())
                .temperature(humidityObjectDto.temperature())
                .timestamp(humidityObjectDto.timestamp())
                .device(device)
                .build();
        createdHumidityObject =  humidityObjectRepository.save(createdHumidityObject);
        log.debug("Successfully added humidity object: {}.", createdHumidityObject);
        val location = ServletUriComponentsBuilder.fromCurrentRequest()
                .path("/{device}").path("/{uuid}")
                .buildAndExpand(createdHumidityObject.getDevice().getName(),
                        createdHumidityObject.getId().toString())
                .toUri();
        return ResponseEntity.created(location).build();
    }
}
