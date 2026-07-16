package de.markostreich.smarthome.switchdeviceapi.model.repo;

import java.util.List;
import java.util.Optional;
import java.util.Collection;
import java.util.UUID;

import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import de.markostreich.smarthome.deviceapi.model.Device;
import de.markostreich.smarthome.switchdeviceapi.model.SwitchObject;

public interface SwitchObjectRepository extends CrudRepository<SwitchObject, UUID>{
	
	List<SwitchObject> findByDevice(Device device);
	
	Optional<SwitchObject> findByNameAndDevice(String name, Device device);

	@Modifying
	@Transactional
	@Query("delete from SwitchObject s where s.device.id in :deviceIds")
	int deleteByDeviceIdIn(Collection<UUID> deviceIds);

}
