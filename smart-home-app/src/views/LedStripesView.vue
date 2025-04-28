<template>
  <v-container class="pa-4">
    <v-card class="mb-4" v-for="device in devices" :key="device.name">
      <LedStripeDevice :device-name="device.name" />
    </v-card>
  </v-container>
</template>

<script setup lang="ts">
import { Device } from "@/interfaces/Device";
import { getCurrentInstance, onBeforeUnmount, onMounted, ref } from "vue";
import LedStripeDevice from "@/components/LedStripeDevice.vue";

const { proxy } = getCurrentInstance();

const devices = ref<Device[]>([]);
let intervalHandle: number | null = null;

const isValidLedStripeDevice = (device: Device): boolean => {
  return (
    Array.isArray(device.ledStripeObjects) && device.ledStripeObjects.length > 0
  );
};

const fetchDevices = async () => {
  try {
    const response = await proxy?.$axios.get("/device/list");
    devices.value = response.data.filter(isValidLedStripeDevice);
  } catch (error) {
    console.error("Error fetching devices:", error);
  }
};

onMounted(() => {
  fetchDevices();
  intervalHandle = window.setInterval(fetchDevices, 1000);
});

onBeforeUnmount(() => {
  if (intervalHandle !== null) clearInterval(intervalHandle);
});
</script>
