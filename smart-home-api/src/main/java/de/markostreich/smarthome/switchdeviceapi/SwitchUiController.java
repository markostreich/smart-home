package de.markostreich.smarthome.switchdeviceapi;

import java.util.List;
import java.util.Objects;

import org.springframework.context.annotation.Profile;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.switchdeviceapi.model.dto.SwitchObjectDto;
import de.markostreich.smarthome.switchdeviceapi.model.repo.SwitchObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.val;
import lombok.extern.slf4j.Slf4j;

@RestController
@Profile("!testmode")
@RequestMapping("/ui/switch")
@RequiredArgsConstructor
@Slf4j
public class SwitchUiController {

	private final SwitchObjectRepository switchObjectRepository;
	private final DeviceRepository deviceRepository;

	@GetMapping(path = "/{device}", produces = "application/json")
	public ResponseEntity<List<SwitchObjectDto>> getSwitchObjects(
			@PathVariable(name = "device") final String deviceName) {
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.", deviceName);
			return ResponseEntity.notFound().build();
		}
		val switchObjects = switchObjectRepository.findByDevice(device).stream()
				.map(switchObject -> new SwitchObjectDto(switchObject.getName(),
						switchObject.isState(), switchObject.getDuration(),
						switchObject.getDevice().getName()))
				.toList();
		return ResponseEntity.ok(switchObjects);
	}

	@PostMapping(consumes = "application/json")
	public ResponseEntity<Void> updateSwitchObject(
			@RequestBody final SwitchObjectDto switchObjectDto) {
		val device = deviceRepository.findByName(switchObjectDto.deviceName());
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.",
					switchObjectDto.deviceName());
			return ResponseEntity.notFound().build();
		}
		val switchObject = switchObjectRepository
				.findByNameAndDevice(switchObjectDto.name(), device);
		if (switchObject.isEmpty())
			return ResponseEntity.notFound().build();
		switchObject.get().setState(switchObjectDto.state());
		switchObject.get().setDuration(switchObjectDto.duration());
		switchObjectRepository.save(switchObject.get());
		return ResponseEntity.accepted().build();
	}
}
