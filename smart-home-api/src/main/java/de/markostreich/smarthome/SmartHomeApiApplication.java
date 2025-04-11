package de.markostreich.smarthome;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@EnableScheduling
public class SmartHomeApiApplication {

	public static void main(final String[] args) {
		SpringApplication.run(SmartHomeApiApplication.class, args);
	}

}
