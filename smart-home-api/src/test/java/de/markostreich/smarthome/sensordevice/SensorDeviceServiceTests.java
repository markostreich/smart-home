package de.markostreich.smarthome.sensordevice;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.sensordevice.model.HumidityObject;
import de.markostreich.smarthome.sensordevice.model.repo.HumidityObjectRepository;
import de.markostreich.smarthome.sensordevice.model.repo.SensorDeviceRepository;
import de.markostreich.smarthome.sensordevice.service.SensorDeviceService;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.transaction.annotation.Transactional;

import java.sql.Timestamp;
import java.time.Instant;
import java.time.LocalDateTime;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

@SpringBootTest(properties = {
        "spring.datasource.url=jdbc:h2:mem:sensor-test",
        "spring.jpa.hibernate.ddl-auto=create-drop",
        "smarthome.disconnect-threshold=3600000"
})
@Transactional
class SensorDeviceServiceTests {
    @Autowired SensorDeviceService service;
    @Autowired SensorDeviceRepository sensors;
    @Autowired HumidityObjectRepository readings;
    @Autowired DeviceRepository devices;

    @Test
    void sensorLifecycleDoesNotModifyRegularDeviceWithSameName() {
        var originalLogin = Timestamp.from(Instant.now().minusSeconds(60));
        var regularDevice = devices.save(Device.builder().name("shared-name")
                .lastLogin(originalLogin).build());

        assertTrue(service.connectDevice("shared-name"));
        assertFalse(service.connectDevice("shared-name"));
        var sensor = sensors.findByName("shared-name");
        assertNotNull(sensor);
        assertEquals(sensor.getId(), sensors.findIdByName("shared-name"));
        assertEquals(List.of(sensor.getId()), sensors.findIdsByLastLoginBefore(
                Timestamp.from(Instant.now().plusSeconds(60))));
        assertTrue(sensor.getLastLogin().after(originalLogin));

        var reading = readings.save(HumidityObject.builder().device(sensor)
                .timestamp(LocalDateTime.now()).temperature(21.5f).humidity(45f).build());
        assertEquals(1, service.getAllDevices().size());
        assertTrue(service.deleteDevice("shared-name"));
        assertFalse(readings.existsById(reading.getId()));
        assertFalse(sensors.existsById(sensor.getId()));
        assertFalse(service.deleteDevice("shared-name"));
        assertEquals(originalLogin, devices.findById(regularDevice.getId()).orElseThrow().getLastLogin());
    }
}
