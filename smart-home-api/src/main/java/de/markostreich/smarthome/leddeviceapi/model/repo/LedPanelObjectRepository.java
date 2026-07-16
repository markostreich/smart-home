package de.markostreich.smarthome.leddeviceapi.model.repo;

import java.util.List;
import java.util.Optional;
import java.util.Collection;
import java.util.UUID;

import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.leddeviceapi.model.LedPanelObject;

public interface LedPanelObjectRepository extends CrudRepository<LedPanelObject, UUID> {

	List<LedPanelObject> findByDevice(Device device);

	Optional<LedPanelObject> findByNameAndDevice(String name, Device device);

	@Modifying
	@Transactional
	@Query("delete from LedPanelObject l where l.device.id in :deviceIds")
	int deleteByDeviceIdIn(Collection<UUID> deviceIds);
}
