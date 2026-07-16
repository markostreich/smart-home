package de.markostreich.smarthome.switchdeviceapi.model.repo;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

import org.springframework.data.repository.CrudRepository;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.switchdeviceapi.model.SwitchObject;

public interface SwitchObjectRepository extends CrudRepository<SwitchObject, UUID>{
	
	List<SwitchObject> findByDevice(Device device);
	
	Optional<SwitchObject> findByNameAndDevice(String name, Device device);

}
