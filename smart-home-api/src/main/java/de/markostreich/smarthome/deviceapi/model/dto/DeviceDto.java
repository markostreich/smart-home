package de.markostreich.smarthome.deviceapi.model.dto;

import java.util.Set;

public record DeviceDto(
		String name,
		Set<String> ledPanelObjects,
		Set<String> ledStripeObjects,
		Set<String> switchObjects) {
}
