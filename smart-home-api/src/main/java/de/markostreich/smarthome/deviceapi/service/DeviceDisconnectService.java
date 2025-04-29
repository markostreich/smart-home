package de.markostreich.smarthome.deviceapi.service;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Profile;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

@Profile("!dev")
@Service
@RequiredArgsConstructor
@Slf4j
public class DeviceDisconnectService {

    @Value("${smarthome.disconnect-threshold}")
    private long disconnectThreshold;

    private final DeviceRepository deviceRepository;

    @Scheduled(fixedDelay = 1000)
    public void disconnectAbsent() {
        val currentTime = System.currentTimeMillis();
        val deviceIterator = deviceRepository.findAll();
        deviceIterator.forEach(device -> {
            if (currentTime
                    - device.getLastLogin().getTime() > disconnectThreshold)
                deviceRepository.delete(device);
            log.debug("Deleted {}", device.getName());
        });
    }
}
