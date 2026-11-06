package de.markostreich.smarthome.sensordevice.model.repo;

import de.markostreich.smarthome.sensordevice.model.SensorDevice;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import java.sql.Timestamp;
import java.util.Collection;
import java.util.List;
import java.util.UUID;

public interface SensorDeviceRepository extends CrudRepository<SensorDevice, UUID> {

    SensorDevice findByName(String name);

    @Query("select d.id from SensorDevice d where d.name = :name")
    UUID findIdByName(String name);

    @Query("select d.id from SensorDevice d where d.lastLogin < :lastLogin")
    List<UUID> findIdsByLastLoginBefore(Timestamp lastLogin);

    @Modifying
    @Transactional
    @Query("update SensorDevice d set d.lastLogin = :lastLogin where d.name = :name")
    int updateLastLoginByName(String name, Timestamp lastLogin);

    @Modifying
    @Transactional
    @Query("delete from SensorDevice d where d.id in :deviceIds")
    int deleteByIdIn(Collection<UUID> deviceIds);
}
