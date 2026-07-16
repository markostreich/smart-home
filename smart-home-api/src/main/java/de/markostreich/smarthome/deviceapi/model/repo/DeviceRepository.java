package de.markostreich.smarthome.deviceapi.model.repo;

import java.sql.Timestamp;
import java.util.Collection;
import java.util.List;
import java.util.UUID;

import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import de.markostreich.smarthome.deviceapi.model.Device;

public interface DeviceRepository extends CrudRepository<Device, UUID> {

	Device findByName(String name);

	@Query("select d.id from Device d where d.name = :name")
	UUID findIdByName(String name);

	@Query("select d.id from Device d where d.lastLogin < :lastLogin")
	List<UUID> findIdsByLastLoginBefore(Timestamp lastLogin);

	@Modifying
	@Transactional
	@Query("update Device d set d.lastLogin = :lastLogin where d.name = :name")
	int updateLastLoginByName(String name, Timestamp lastLogin);

	@Modifying
	@Transactional
	@Query("delete from Device d where d.id in :deviceIds")
	int deleteByIdIn(Collection<UUID> deviceIds);
}
