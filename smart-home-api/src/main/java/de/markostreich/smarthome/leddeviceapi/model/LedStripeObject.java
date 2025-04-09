package de.markostreich.smarthome.leddeviceapi.model;

import java.util.UUID;

import org.hibernate.annotations.OnDelete;
import org.hibernate.annotations.OnDeleteAction;

import de.markostreich.smarthome.deviceapi.model.Device;
import jakarta.persistence.Column;
import jakarta.persistence.Entity;
import jakarta.persistence.GeneratedValue;
import jakarta.persistence.Id;
import jakarta.persistence.JoinColumn;
import jakarta.persistence.ManyToOne;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Entity
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class LedStripeObject {

	@Id
	@GeneratedValue
	@Column(unique = true, nullable = false)
	private UUID id;

	@Column(nullable = false)
	private String name;

	private LedStripeMode mode;

	private byte red;

	private byte green;

	private byte blue;

	private byte brightness;

	@ManyToOne
	@JoinColumn(name = "device_id", nullable = false)
	@OnDelete(action = OnDeleteAction.CASCADE)
	private Device device;
}
