package de.markostreich.smarthome.sensordevice;

import de.markostreich.smarthome.sensordevice.model.dto.SensorDeviceDto;
import de.markostreich.smarthome.sensordevice.service.SensorDeviceService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import java.util.List;

@RestController
@RequestMapping("/sensordevice")
@RequiredArgsConstructor
@Slf4j
public class SensorDeviceController {

    private final SensorDeviceService sensorDeviceService;

    @PostMapping(path = "/connect", consumes = "application/json", produces = "application/json")
    public ResponseEntity<Object> connectDevice(
            @RequestBody final SensorDeviceDto device) {
        if (!sensorDeviceService.connectDevice(device.name()))
            return ResponseEntity.accepted().build();

        val location = ServletUriComponentsBuilder.fromCurrentContextPath()
                .path("/update").path("/{device}")
                .buildAndExpand(device.name()).toUri();
        return ResponseEntity.created(location).build();
    }

    @DeleteMapping(path = "/{device}")
    public ResponseEntity<String> deleteDevice(
            @PathVariable(name = "device") final String deviceName) {
        if (sensorDeviceService.deleteDevice(deviceName))
            return ResponseEntity.ok(
                    "Device '" + deviceName + "' was deleted successfully.");
        else
            return ResponseEntity.notFound().build();
    }

    @GetMapping(path = "/list", produces = "application/json")
    public ResponseEntity<List<SensorDeviceDto>> getDevices() {
        return ResponseEntity.ok(sensorDeviceService.getAllDevices());
    }
}
