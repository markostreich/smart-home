package de.markostreich.smarthome.sensordevice.model.repo;

import de.markostreich.smarthome.sensordevice.model.HumidityObject;
import de.markostreich.smarthome.sensordevice.model.SensorDevice;
import org.springframework.data.jpa.repository.Modifying;
import org.springframework.data.jpa.repository.Query;
import org.springframework.data.repository.CrudRepository;
import org.springframework.transaction.annotation.Transactional;

import java.util.Collection;
import java.util.List;
import java.util.UUID;

public interface HumidityObjectRepository extends CrudRepository<HumidityObject, UUID> {

    List<HumidityObject> findByDevice(SensorDevice device);

    @Modifying
    @Transactional
    @Query("delete from HumidityObject s where s.device.id in :deviceIds")
    int deleteByDeviceIdIn(Collection<UUID> deviceIds);
}
