package de.markostreich.smarthome.leddeviceapi.model.repo;

import java.util.List;
import java.util.Optional;
import java.util.UUID;

import org.springframework.data.repository.CrudRepository;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.leddeviceapi.model.LedStripeObject;

public interface LedStripeObjectRepository extends CrudRepository<LedStripeObject, UUID>{

	List<LedStripeObject> findByDevice(Device device);
	
	Optional<LedStripeObject>findByNameAndDevice(String name, Device device);

}
