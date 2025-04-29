package de.markostreich.smarthome.leddeviceapi;

import de.markostreich.smarthome.deviceapi.model.repo.DeviceRepository;
import de.markostreich.smarthome.leddeviceapi.model.LedStripeMode;
import de.markostreich.smarthome.leddeviceapi.model.LedStripeObject;
import de.markostreich.smarthome.leddeviceapi.model.dto.LedStripeObjectDto;
import de.markostreich.smarthome.leddeviceapi.model.repo.LedStripeObjectRepository;
import lombok.RequiredArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.context.annotation.Profile;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.servlet.support.ServletUriComponentsBuilder;

import java.util.List;
import java.util.Objects;

@RestController
@Profile("!testmode")
@RequestMapping("/ledstripe")
@RequiredArgsConstructor
@Slf4j
public class LedStripeController {

    private final LedStripeObjectRepository ledStripeObjectRepository;
    private final DeviceRepository ledDeviceRepository;

    @GetMapping(path = "/update/{device}", produces = "application/json")
    public ResponseEntity<List<LedStripeObjectDto>> update(
            @PathVariable(name = "device") final String deviceName) {
        log.debug("Searching data for '{}'...", deviceName);
        val ledDevice = ledDeviceRepository.findByName(deviceName);
        if (Objects.isNull(ledDevice)) {
            log.warn("Could not find device '{}'", deviceName);
            return ResponseEntity.notFound().build();
        }
        val ledStripeObjectList = ledStripeObjectRepository.findByDevice(ledDevice);
        if (ledStripeObjectList.isEmpty()) {
            log.warn("Could not find led stripe data for device '{}'.",
                    deviceName);
            return ResponseEntity.noContent().build();
        }
        log.debug("Found data for device '{}':", deviceName);
        val ledStripeObjects = ledStripeObjectList.stream()
                .map(ledStripeObject -> new LedStripeObjectDto(
                        ledStripeObject.getName(),
                        ledStripeObject.getMode().toString(),
                        ledStripeObject.getRed(),
                        ledStripeObject.getGreen(),
                        ledStripeObject.getBlue(),
                        ledStripeObject.getBrightness(),
                        ledStripeObject.getDevice().getName()
                ))
                .toList();
        return ResponseEntity.ok(ledStripeObjects);
    }

    @PostMapping(path = "/stripeobject", consumes = "application/json", produces = "application/json")
    public ResponseEntity<Object> postLedStripeObject(
            @RequestBody final LedStripeObjectDto objectDto) {
        log.info(objectDto.toString());
        val device = ledDeviceRepository.findByName(objectDto.deviceName());
        if (Objects.isNull(device)) {
            log.warn("No device found with name '{}'.", objectDto.deviceName());
            return ResponseEntity.noContent().build();
        }
        val existingLedStripeObjectOptional = ledStripeObjectRepository.findByNameAndDevice(objectDto.name(), device);
        if (existingLedStripeObjectOptional.isPresent()) {
            val existingLedStripeObject = existingLedStripeObjectOptional.get();
            existingLedStripeObject
                    .setMode(LedStripeMode.valueOf(objectDto.mode()));
            existingLedStripeObject.setRed(objectDto.red());
            existingLedStripeObject.setGreen(objectDto.green());
            existingLedStripeObject.setBlue(objectDto.blue());
            existingLedStripeObject.setBrightness(objectDto.brightness());
            ledStripeObjectRepository.save(existingLedStripeObject);
            return ResponseEntity.accepted().build();
        }
        var createdObject = LedStripeObject.builder()
                .name(objectDto.name())
                .mode(LedStripeMode.valueOf(objectDto.mode()))
                .red(objectDto.red()).green(objectDto.green())
                .blue(objectDto.blue()).brightness(objectDto.brightness())
                .device(device).build();
        createdObject = ledStripeObjectRepository.save(createdObject);
        val location = ServletUriComponentsBuilder.fromCurrentContextPath()
                .path("/{device}").path("/{object}")
                .buildAndExpand(createdObject.getDevice().getName(),
                        createdObject.getName()).toUri();
        return ResponseEntity.created(location).build();
    }

    @GetMapping(path = "/stripeobject/{device}/{object}")
    public ResponseEntity<LedStripeObjectDto> getLedStripeObject(
            @PathVariable(name = "device") final String deviceName,
            @PathVariable(name = "object") final String objectName) {
        val device = ledDeviceRepository.findByName(deviceName);
        if (Objects.isNull(device)) {
            log.warn("Get LedStripeObject: Could not find device '{}'.", deviceName);
            return ResponseEntity.notFound().build();
        }
        return ledStripeObjectRepository.findByNameAndDevice(objectName, device)
                .map(ledStripeObject -> {
                    val responseObject = new LedStripeObjectDto(
                            ledStripeObject.getName(),
                            ledStripeObject.getMode().toString(),
                            ledStripeObject.getRed(),
                            ledStripeObject.getGreen(),
                            ledStripeObject.getBlue(),
                            ledStripeObject.getBrightness(),
                            ledStripeObject.getDevice().getName()
                    );
                    return ResponseEntity.ok(responseObject);
                }).orElse(ResponseEntity.notFound().build());
    }

    @DeleteMapping(path = "/stripeobject/{device}/{object}")
    public ResponseEntity<String> deleteLedStripeObject(
            @PathVariable(name = "device") final String deviceName,
            @PathVariable(name = "object") final String objectName) {
        val device = ledDeviceRepository.findByName(deviceName);
        if (Objects.isNull(device)) {
            log.warn("Delete LedStripeObject: Could not find device '{}'.", deviceName);
            return ResponseEntity.notFound().build();
        }
        return ledStripeObjectRepository.findByNameAndDevice(objectName, device)
                .map(ledStripeObject -> {
                    ledStripeObjectRepository.delete(ledStripeObject);
                    return ResponseEntity.ok("LedStripeObject '" + objectName
                            + "' was deleted successfully.");
                }).orElse(ResponseEntity.notFound().build());
    }
}
