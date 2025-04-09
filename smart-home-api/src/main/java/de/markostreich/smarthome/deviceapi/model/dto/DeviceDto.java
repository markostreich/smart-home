package de.markostreich.smarthome.deviceapi.model.dto;

import java.util.Set;

public record DeviceDto(
		String name,
		Set<String> ledPanelObjects,
		String ledStripeObject,
		Set<String> switchObjects) {
}
