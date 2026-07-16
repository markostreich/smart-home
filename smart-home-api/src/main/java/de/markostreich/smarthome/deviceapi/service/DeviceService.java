package de.markostreich.smarthome.deviceapi.service;

import de.markostreich.smarthome.deviceapi.model.Device;
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
import org.springframework.dao.DataIntegrityViolationException;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.sql.Timestamp;
import java.time.Instant;
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

	public boolean connectDevice(final String deviceName) {
		val now = Timestamp.from(Instant.now());
		if (deviceRepository.updateLastLoginByName(deviceName, now) > 0) {
			log.debug("Device '{}' connected.", deviceName);
			return false;
		}

		try {
			log.info("Adding new device '{}'.", deviceName);
			val createdDevice = Device.builder().name(deviceName).lastLogin(now)
					.build();
			deviceRepository.save(createdDevice);
			log.info("Added new device '{}'.", deviceName);
			return true;
		} catch (final DataIntegrityViolationException exception) {
			log.debug("Device '{}' was created concurrently. Updating login.",
					deviceName);
			deviceRepository.updateLastLoginByName(deviceName,
					Timestamp.from(Instant.now()));
			return false;
		}
	}

	@Transactional(readOnly = true)
	public List<DeviceDto> getAllDevices() {
		val deviceIterator = deviceRepository.findAll();
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

	@Transactional
	public boolean deleteDevice(final String deviceName) {
		val deviceId = deviceRepository.findIdByName(deviceName);
		if (Objects.isNull(deviceId))
			return false;
		deleteDevices(List.of(deviceId));
		return true;
	}

	@Transactional
	public int deleteDisconnectedDevices(final Timestamp threshold) {
		val deviceIds = deviceRepository.findIdsByLastLoginBefore(threshold);
		if (deviceIds.isEmpty())
			return 0;
		return deleteDevices(deviceIds);
	}

	private int deleteDevices(final Collection<UUID> deviceIds) {
		switchObjectRepository.deleteByDeviceIdIn(deviceIds);
		ledStripeObjectRepository.deleteByDeviceIdIn(deviceIds);
		ledPanelObjectRepository.deleteByDeviceIdIn(deviceIds);
		return deviceRepository.deleteByIdIn(deviceIds);
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
