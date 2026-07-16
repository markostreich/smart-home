package de.markostreich.smarthome.switchdeviceapi;

import java.util.List;
import java.util.Objects;

import org.springframework.context.annotation.Profile;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.switchdeviceapi.model.SwitchObject;
import de.markostreich.smarthome.switchdeviceapi.model.dto.SwitchObjectDto;
import de.markostreich.smarthome.switchdeviceapi.model.repo.SwitchObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.val;
import lombok.extern.slf4j.Slf4j;

@RestController
@Profile("!testmode")
@RequestMapping("/switch")
@RequiredArgsConstructor
@Slf4j
public class SwitchController {

	private final SwitchObjectRepository switchObjectRepository;
	private final DeviceRepository deviceRepository;

	@GetMapping(path = "/update/{device}", produces = "application/json")
	public ResponseEntity<List<SwitchObjectDto>> update(
			@PathVariable(name = "device") final String deviceName) {
		log.debug("Searching data for device '{}' ...", deviceName);
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.", deviceName);
			return ResponseEntity.notFound().build();
		}
		val switchObjectList = switchObjectRepository.findByDevice(device);
		if (switchObjectList.isEmpty()) {
			log.warn("Could not find switch data for device '{}'.", deviceName);
			return ResponseEntity.noContent().build();
		}
		log.debug("Found {} switch data(s) for device '{}'.",
				switchObjectList.size(), deviceName);
		val switchObjectDtos = switchObjectList.stream()
				.map(switchObject -> new SwitchObjectDto(switchObject.getName(),
						switchObject.isState(), switchObject.getDuration(),
						switchObject.getDevice().getName()))
				.toList();
		return ResponseEntity.ok(switchObjectDtos);
	}

	@PostMapping(path = "/object", consumes = "application/json", produces = "application/json")
	public ResponseEntity<SwitchObjectDto> addSwitchObject(
			@RequestBody SwitchObjectDto switchObjectDto) {
		log.info("Attempting to add switch object for device '{}'.",
				switchObjectDto.deviceName());
		log.info(switchObjectDto.toString());
		val device = deviceRepository.findByName(switchObjectDto.deviceName());
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.",
					switchObjectDto.deviceName());
			return ResponseEntity.notFound().build();
		}
		val existingSwitchObjectOptional = switchObjectRepository
				.findByNameAndDevice(switchObjectDto.name(), device);
		if (existingSwitchObjectOptional.isPresent()) {
			existingSwitchObjectOptional.ifPresent(existingSwitchObject -> {
				log.info("Updating switch device '{}'",
						switchObjectDto.deviceName());
				existingSwitchObject.setState(switchObjectDto.state());
				existingSwitchObject.setDuration(switchObjectDto.duration());
				switchObjectRepository.save(existingSwitchObject);
			});
			return ResponseEntity.accepted().build();
		}
		var createdSwitchObject = SwitchObject.builder()
				.name(switchObjectDto.name()).state(switchObjectDto.state())
				.duration(switchObjectDto.duration()).device(device).build();
		createdSwitchObject = switchObjectRepository.save(createdSwitchObject);
		log.info("Successfully added switch object '{}'.",
				createdSwitchObject.getName());
		val location = ServletUriComponentsBuilder.fromCurrentRequest()
				.path("/{device}").path("/{object}")
				.buildAndExpand(createdSwitchObject.getDevice().getName(),
						createdSwitchObject.getName())
				.toUri();
		return ResponseEntity.created(location).build();
	}

	@GetMapping(path = "/object/{device}/{object}", produces = "application/json")
	public ResponseEntity<SwitchObjectDto> getSwitchObject(
			@PathVariable(name = "device") final String deviceName,
			@PathVariable(name = "object") final String objectName) {
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Get SwitchObject: Could not find device '{}'.",
					deviceName);
			return ResponseEntity.notFound().build();
		}
		val optionalSwitchObject = switchObjectRepository
				.findByNameAndDevice(objectName, device);
		optionalSwitchObject.ifPresentOrElse(
				object -> log.info("Found {}", object.getName()),
				() -> log.info("Could not find {}", objectName));
		return switchObjectRepository.findByNameAndDevice(objectName, device)
				.map(switchObject -> {
					val responseBody = new SwitchObjectDto(
							switchObject.getName(), switchObject.isState(),
							switchObject.getDuration(),
							switchObject.getDevice().getName());
					return ResponseEntity.ok(responseBody);
				}).orElse(ResponseEntity.notFound().build());
	}

	@DeleteMapping(path = "/object/{device}/{object}")
	public ResponseEntity<String> deleteSwitchObject(
			@PathVariable(name = "device") final String deviceName,
			@PathVariable(name = "object") final String objectName) {
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Delete SwitchObject: Could not find device '{}'.",
					deviceName);
			return ResponseEntity.notFound().build();
		}
		return switchObjectRepository.findByNameAndDevice(objectName, device)
				.map(switchObject -> {
					switchObjectRepository.delete(switchObject);
					return ResponseEntity.ok("SwitchObject '" + objectName
							+ "' was deleted successfully.");
				}).orElse(ResponseEntity.notFound().build());
	}

}
