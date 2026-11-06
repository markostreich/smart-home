package de.markostreich.smarthome.sensordevice.model;

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
import org.hibernate.annotations.OnDelete;
import org.hibernate.annotations.OnDeleteAction;

import java.time.LocalDateTime;
import java.util.UUID;

@Entity
@Data
@NoArgsConstructor
@AllArgsConstructor
@Builder
public class HumidityObject {

    @Id
    @GeneratedValue
    @Column(unique = true, nullable = false)
    private UUID id;

    @Column(nullable = false)
    private LocalDateTime timestamp;

    @Column(nullable = false)
    private Float temperature;

    @Column(nullable = false)
    private Float humidity;

    @ManyToOne
    @JoinColumn(name = "device_id", nullable = false)
    @OnDelete(action = OnDeleteAction.NO_ACTION)
    private SensorDevice device;
}
