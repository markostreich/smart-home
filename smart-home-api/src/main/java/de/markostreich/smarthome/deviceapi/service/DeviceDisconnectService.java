package de.markostreich.smarthome.deviceapi.service;

import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Profile;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import java.sql.Timestamp;
import java.time.Instant;

@Profile("!dev")
@Service
@RequiredArgsConstructor
@Slf4j
public class DeviceDisconnectService {

    @Value("${smarthome.disconnect-threshold}")
    private long disconnectThreshold;

    private final DeviceService deviceService;

    @Scheduled(fixedDelay = 1000)
    public void disconnectAbsent() {
        val threshold = Timestamp.from(
                Instant.now().minusMillis(disconnectThreshold));
        val deletedDevices = deviceService.deleteDisconnectedDevices(threshold);
        if (deletedDevices > 0)
            log.debug("Deleted {} disconnected device(s).", deletedDevices);
    }
}
