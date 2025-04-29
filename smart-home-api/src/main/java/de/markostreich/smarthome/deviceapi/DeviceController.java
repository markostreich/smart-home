package de.markostreich.smarthome.deviceapi;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.deviceapi.model.dto.DeviceDto;
import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.deviceapi.service.DeviceService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import java.sql.Timestamp;
import java.time.Instant;
import java.util.List;
import java.util.Objects;

@RestController
@RequestMapping("/device")
@RequiredArgsConstructor
@Slf4j
public class DeviceController {

	private final DeviceRepository deviceRepository;
	private final DeviceService deviceService;

	@PostMapping(path = "/connect", consumes = "application/json", produces = "application/json")
	public ResponseEntity<Object> connectDevice(
			@RequestBody final DeviceDto device) {
		/* Device exists */
		val existingDevice = deviceRepository.findByName(device.name());
		if (Objects.nonNull(existingDevice)) {
			existingDevice.setLastLogin(Timestamp.from(Instant.now()));
			deviceRepository.save(existingDevice);
			log.debug("Device '{}' connected.", existingDevice.getName());
			return ResponseEntity.accepted().build();
		}

		/* add new device */
		log.info("Adding new device '{}'.", device.name());
		var createdDevice = Device.builder().name(device.name())
				.lastLogin(Timestamp.from(Instant.now())).build();
		createdDevice = deviceRepository.save(createdDevice);
		log.info("Added new device '{}'.", createdDevice.getName());
		val location = ServletUriComponentsBuilder.fromCurrentContextPath()
				.path("/update").path("/{device}")
				.buildAndExpand(createdDevice.getName()).toUri();
		return ResponseEntity.created(location).build();
	}

	@DeleteMapping(path = "/{device}")
	public ResponseEntity<String> deleteDevice(
			@PathVariable(name = "device") final String deviceName) {
		if (deviceService.deleteDevice(deviceName))
			return ResponseEntity.ok(
					"Device '" + deviceName + "' was deleted successfully.");
		else
			return ResponseEntity.notFound().build();
	}

	@GetMapping(path = "/list", produces = "application/json")
	public ResponseEntity<List<DeviceDto>> getDevices() {
		return ResponseEntity.ok(deviceService.getAllDevices());
	}

}
