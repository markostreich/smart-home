<template>
  <v-container class="pa-4">
    <v-card class="mb-4" v-for="device in devices" :key="device.name">
      <SwitchDevice :device-name="device.name" />
    </v-card>
  </v-container>
</template>

<script setup lang="ts">
import SwitchDevice from "@/components/SwitchDevice.vue";
import axios from "axios";
import { onMounted, onBeforeUnmount, ref, getCurrentInstance } from "vue";

interface Device {
  name: string;
  ledPanelObjects: string[];
  ledStripeObjects: string[];
  switchObjects: string[];
}

const { proxy } = getCurrentInstance();

const devices = ref<Device[]>([]);
let intervalHandle: number | null = null;

const isValidSwitchDevice = (device: Device): boolean => {
  return Array.isArray(device.switchObjects) && device.switchObjects.length > 0;
};

const fetchDevices = async () => {
  try {
    const response = await proxy?.$axios.get("/device/list");
    devices.value = response.data.filter(isValidSwitchDevice);
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
