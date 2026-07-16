package de.markostreich.smarthome.deviceapi;

import de.markostreich.smarthome.deviceapi.model.dto.DeviceDto;
import de.markostreich.smarthome.deviceapi.service.DeviceService;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import java.util.List;

@RestController
@RequestMapping("/device")
@RequiredArgsConstructor
@Slf4j
public class DeviceController {

	private final DeviceService deviceService;

	@PostMapping(path = "/connect", consumes = "application/json", produces = "application/json")
	public ResponseEntity<Object> connectDevice(
			@RequestBody final DeviceDto device) {
		if (!deviceService.connectDevice(device.name()))
			return ResponseEntity.accepted().build();

		val location = ServletUriComponentsBuilder.fromCurrentContextPath()
				.path("/update").path("/{device}")
				.buildAndExpand(device.name()).toUri();
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
