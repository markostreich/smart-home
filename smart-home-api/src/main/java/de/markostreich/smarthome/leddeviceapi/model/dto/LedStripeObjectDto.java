package de.markostreich.smarthome.leddeviceapi.model.dto;

public record LedStripeObjectDto(
		String name,
		String mode,
		int red,
		int green,
		int blue,
		int brightness,
		String deviceName) {}
