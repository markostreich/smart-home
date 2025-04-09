package de.markostreich.smarthome.deviceapi.service;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Collectors;

import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import de.markostreich.smarthome.deviceapi.model.dto.DeviceDto;
import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.leddeviceapi.model.LedPanelObject;
import de.markostreich.smarthome.leddeviceapi.model.LedStripeObject;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedPanelObjectRepository;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedStripeObjectRepository;
import de.markostreich.smarthome.switchdeviceapi.model.SwitchObject;
import de.markostreich.smarthome.switchdeviceapi.model.repo.SwitchObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.val;
import lombok.extern.slf4j.Slf4j;

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
		log.info(deviceIterator.toString());
		val deviceDtos = new ArrayList<DeviceDto>();
		deviceIterator.forEach(device -> {
			log.info(device.toString());
			val ledPanelObjects = ledPanelObjectRepository.findByDevice(device);
			val ledStripeObjects = ledStripeObjectRepository.findByDevice(device);
			val switchObjects = switchObjectRepository.findByDevice(device);
			log.info(switchObjects.toString());
			deviceDtos.add(new DeviceDto(device.getName(),
					getLedPanelObjectName(ledPanelObjects),
					getLedStripeObjectName(ledStripeObjects),
					getSwitchObjectName(switchObjects)));
		});
		return deviceDtos;
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
