#!/bin/bash

URL="http://localhost:8080/device/connect"
JSON_PAYLOAD='{"name":"device_one"}'
DELAY=1  # seconds between requests (optional)

while true; do
  echo "Sending POST request..."
  curl -X POST $URL \
       -H "Content-Type: application/json" \
       -d "$JSON_PAYLOAD"
  echo -e "\n---"
  sleep $DELAY
done

