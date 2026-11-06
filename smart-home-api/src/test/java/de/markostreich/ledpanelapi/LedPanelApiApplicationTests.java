package de.markostreich.ledpanelapi;

import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;

import de.markostreich.smarthome.SmartHomeApiApplication;

@SpringBootTest(classes = SmartHomeApiApplication.class, properties = {
		"spring.datasource.url=jdbc:h2:mem:context-test",
		"spring.jpa.hibernate.ddl-auto=create-drop"
})
class LedPanelApiApplicationTests {

	@Test
	void contextLoads() {
	}

}
