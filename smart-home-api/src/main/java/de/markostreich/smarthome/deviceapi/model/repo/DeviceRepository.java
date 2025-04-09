package de.markostreich.smarthome.deviceapi.model.repo;

import org.springframework.data.repository.CrudRepository;

import de.markostreich.smarthome.deviceapi.model.Device;

public interface DeviceRepository extends CrudRepository<Device, String> {

	Device findByName(String name);
}
