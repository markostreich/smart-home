# Use an official OpenJDK runtime as a base image
FROM openjdk:21-jdk-slim

# Set the working directory
WORKDIR /app

# Copy the JAR file to the container
COPY target/smart-home-api*.jar smart-home-api.jar

# Expose the port (change if needed)
EXPOSE 8080

# Command to run the application
ENTRYPOINT ["java", "-jar", "smart-home-api.jar"]
