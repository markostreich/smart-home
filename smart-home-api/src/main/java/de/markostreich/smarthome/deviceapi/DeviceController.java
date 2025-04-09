package de.markostreich.smarthome.deviceapi;

import java.sql.Timestamp;
import java.time.Instant;
import java.util.List;
import java.util.Objects;

import org.springframework.context.annotation.Profile;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.deviceapi.model.dto.DeviceDto;
import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.deviceapi.service.DeviceService;
import lombok.RequiredArgsConstructor;
import lombok.val;
import lombok.extern.slf4j.Slf4j;

@RestController
@Profile("!testmode")
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
			log.info("Device '{}' connected.", existingDevice.getName());
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
	
	@GetMapping(path = "/list", produces = "application/json")
	public ResponseEntity<List<DeviceDto>> getDevices() {
        return ResponseEntity.ok(deviceService.getAllDevices());
	}

}
