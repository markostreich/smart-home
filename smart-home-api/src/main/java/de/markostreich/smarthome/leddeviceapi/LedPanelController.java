package de.markostreich.smarthome.leddeviceapi;

import java.util.HexFormat;
import java.util.Objects;

import org.springframework.context.annotation.Profile;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.leddeviceapi.model.LedPanelObject;
import de.markostreich.smarthome.leddeviceapi.model.dto.LedPanelObjectDto;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedPanelObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.val;
import lombok.extern.slf4j.Slf4j;

@RestController
@Profile("!testmode")
@RequestMapping("/ledpanel")
@RequiredArgsConstructor
@Slf4j
public class LedPanelController {

	private final LedPanelObjectRepository ledPanelObjectRepository;
	private final DeviceRepository deviceRepository;
	private final HexFormat hexFormat = HexFormat.of();

	@GetMapping(path = "/update/{device}", produces = "application/json")
	public ResponseEntity<LedPanelObjectDto> update(
			@PathVariable(name = "device") final String deviceName) {
		log.info("Searching data for device '{}' ...", deviceName);
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.", deviceName);
			return ResponseEntity.notFound().build();
		}
		val ledPanelObjectList = ledPanelObjectRepository.findByDevice(device);
		if (ledPanelObjectList.isEmpty()) {
			log.warn("Could not find led panel data for device '{}'.",
					deviceName);
			return ResponseEntity.noContent().build();
		}
		if (ledPanelObjectList.size() != 1) {
			log.warn("Found more than one led panel data for device '{}'.",
					deviceName);
			return ResponseEntity.badRequest().build();
		}
		val ledPanelObject = ledPanelObjectList.getFirst();
		log.info("Found data for device '{}':", deviceName);
		return ResponseEntity.ok(new LedPanelObjectDto(ledPanelObject.getName(),
				ledPanelObject.getX(), ledPanelObject.getY(),
				ledPanelObject.getRotationPointX(),
				ledPanelObject.getRotationPointY(),
				hexFormat.formatHex(ledPanelObject.getData()),
				ledPanelObject.getDevice().getName()));
	}

	@PostMapping(path = "/panelobject", consumes = "application/json", produces = "application/json")
	public ResponseEntity<Object> postLedPanelObject(
			@RequestBody final LedPanelObjectDto objectDto) {
		log.info("Received {}.", objectDto.toString());
		log.info("Searching data for device '{}'.", objectDto.deviceName());
		val device = deviceRepository.findByName(objectDto.deviceName());
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.", objectDto.deviceName());
			return ResponseEntity.noContent().build();
		}
		val exisistingLedPanelObjectOptional = ledPanelObjectRepository
				.findByNameAndDevice(objectDto.name(), device);
		if (exisistingLedPanelObjectOptional.isPresent()) {
			exisistingLedPanelObjectOptional
					.ifPresent(exisistingLedPanelObject -> {
						log.info("Updating led panel device '{}'.",
								objectDto.deviceName());
						exisistingLedPanelObject.setX(objectDto.pox_x());
						exisistingLedPanelObject.setY(objectDto.pos_y());
						exisistingLedPanelObject
								.setRotationPointX(objectDto.rotationPoint_x());
						exisistingLedPanelObject
								.setRotationPointY(objectDto.rotationPoint_y());
						exisistingLedPanelObject.setData(
								hexStringToByteArray(objectDto.imageData()));
						ledPanelObjectRepository.save(exisistingLedPanelObject);
						log.info("Updated.");
					});
			return ResponseEntity.accepted().build();
		}
		log.info("Adding led panel object to device '{}'.", objectDto.name());
		var createdObject = LedPanelObject.builder().name(objectDto.name())
				.x(objectDto.pox_x()).y(objectDto.pos_y())
				.rotationPointX(objectDto.rotationPoint_x())
				.rotationPointY(objectDto.rotationPoint_y())
				.data(hexStringToByteArray(objectDto.imageData()))
				.device(device).build();
		createdObject = ledPanelObjectRepository.save(createdObject);
		log.info("Created.");
		val location = ServletUriComponentsBuilder.fromCurrentContextPath()
				.path("/panelobject").path("/{device}").path("/{object}")
				.buildAndExpand(createdObject.getDevice().getName(),
						createdObject.getName())
				.toUri();
		return ResponseEntity.created(location).build();
	}

	@GetMapping(path = "/panelobject/{device}/{object}", produces = "application/json")
	public ResponseEntity<LedPanelObjectDto> getLedPanelObject(
			@PathVariable(name = "device") final String deviceName,
			@PathVariable(name = "object") final String objectName) {
		log.info("Searching {}/{}", deviceName, objectName);
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device)) {
			log.warn("Could not find device '{}'.", deviceName);
			return ResponseEntity.notFound().build();
		}
		val optionalPanelObject = ledPanelObjectRepository
				.findByNameAndDevice(objectName, device);
		optionalPanelObject.ifPresentOrElse(
				object -> log.info("Found {}", object.getName()),
				() -> log.info("Could not find {}", objectName));
		return optionalPanelObject
				.map(ledPanelObject -> {
					val responseBody = new LedPanelObjectDto(
							ledPanelObject.getName(), ledPanelObject.getX(),
							ledPanelObject.getY(),
							ledPanelObject.getRotationPointX(),
							ledPanelObject.getRotationPointY(),
							hexFormat.formatHex(ledPanelObject.getData()),
							ledPanelObject.getDevice().getName());
					return ResponseEntity.ok(responseBody);
				}).orElse(ResponseEntity.notFound().build());
	}

	private byte[] hexStringToByteArray(final String hexString) {
		val length = hexString.length();
		val data = new byte[length / 2];
		for (var i = 0; i < length; i += 2) {
			data[i / 2] = (byte) ((Character.digit(hexString.charAt(i),
					16) << 4) + Character.digit(hexString.charAt(i + 1), 16));
		}
		return data;
	}

}
