package de.markostreich.smarthome.deviceapi.service;

import de.markostreich.smarthome.deviceapi.model.dto.DeviceDto;
import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.leddeviceapi.model.LedPanelObject;
import de.markostreich.smarthome.leddeviceapi.model.LedStripeObject;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedPanelObjectRepository;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedStripeObjectRepository;
import de.markostreich.smarthome.switchdeviceapi.model.SwitchObject;
import de.markostreich.smarthome.switchdeviceapi.model.repo.SwitchObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.*;
import java.util.stream.Collectors;

@Service
@RequiredArgsConstructor
@Slf4j
public class DeviceService {

	private final DeviceRepository deviceRepository;
	private final LedPanelObjectRepository ledPanelObjectRepository;
	private final LedStripeObjectRepository ledStripeObjectRepository;
	private final SwitchObjectRepository switchObjectRepository;

	@Transactional(readOnly = true)
	public List<DeviceDto> getAllDevices() {
		val deviceIterator = deviceRepository.findAll();
		log.debug(deviceIterator.toString());
		val deviceDtos = new ArrayList<DeviceDto>();
		deviceIterator.forEach(device -> {
			val ledPanelObjects = ledPanelObjectRepository.findByDevice(device);
			val ledStripeObjects = ledStripeObjectRepository
					.findByDevice(device);
			val switchObjects = switchObjectRepository.findByDevice(device);
			deviceDtos.add(new DeviceDto(device.getName(),
					getLedPanelObjectName(ledPanelObjects),
					getLedStripeObjectName(ledStripeObjects),
					getSwitchObjectName(switchObjects)));
		});
		return deviceDtos;
	}

	public boolean deleteDevice(final String deviceName) {
		val device = deviceRepository.findByName(deviceName);
		if (Objects.isNull(device))
			return false;
		deviceRepository.delete(device);
		return true;
	}

	private Set<String> getLedPanelObjectName(
			final List<LedPanelObject> ledPanelObjects) {
		if (Objects.isNull(ledPanelObjects))
			return new HashSet<>();
		return ledPanelObjects.parallelStream().map(LedPanelObject::getName)
				.collect(Collectors.toSet());
	}

	private Set<String> getLedStripeObjectName(
			final List<LedStripeObject> ledStripeObjects) {
		if (Objects.isNull(ledStripeObjects))
			return new HashSet<>();
		return ledStripeObjects.parallelStream().map(LedStripeObject::getName)
				.collect(Collectors.toSet());
	}

	private Set<String> getSwitchObjectName(
			final List<SwitchObject> switchObjects) {
		if (Objects.isNull(switchObjects))
			return new HashSet<>();
		return switchObjects.parallelStream().map(SwitchObject::getName)
				.collect(Collectors.toSet());
	}

}
