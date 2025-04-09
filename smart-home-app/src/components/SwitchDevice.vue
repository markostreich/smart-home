<template>
  <v-container class="pa-4">
    <v-card class="mb-4">
      <v-card-title>
        {{ deviceName }}
      </v-card-title>
    </v-card>

    <div v-if="switchObjects.length === 0">
      <v-alert type="info" variant="tonal" class="mb-4">
        Loading buttons...
      </v-alert>
    </div>

    <v-row v-else dense>
      <v-col
        v-for="switchObject in switchObjects"
        :key="switchObject.name"
        cols="12"
        md="6"
        lg="4"
      >
        <v-card outlined>
          <v-card-title class="text-h6">{{ switchObject.name }}</v-card-title>
          <v-card-text class="d-flex justify-center">
            <SwitchButton
              :isPressed="switchObject.state"
              :duration="switchObject.duration"
              @pressed="(duration: number | null | undefined) => handlePress(switchObject.name, true, duration, props.deviceName)"
              @unpressed="(duration: number | null | undefined) => handlePress(switchObject.name, false, duration, props.deviceName)"
            />
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import {
  defineProps,
  ref,
  onMounted,
  onBeforeUnmount,
  getCurrentInstance,
} from "vue";
import SwitchButton from "./SwitchButton.vue";
import axios from "axios";

interface SwitchObject {
  name: string;
  state: boolean;
  duration: number | null;
  deviceName: string;
}

const props = defineProps<{
  deviceName: string;
}>();

const { proxy } = getCurrentInstance();

const switchObjects = ref<SwitchObject[]>([]);
let intervalHandle: number | null = null;

const fetchButtons = async () => {
  try {
    const response = await proxy?.$axios.get(
      "/switch/update/" + props.deviceName
    );
    switchObjects.value = response.data;
  } catch (error) {
    console.error("Error fetching buttons:", error);
  }
};

const handlePress = async (
  name: string,
  state: boolean,
  duration: number | null | undefined,
  deviceName: string
) => {
  try {
    await proxy?.$axios.post("/switch/object", {
      name,
      state,
      duration,
      deviceName,
    });

    const btn = switchObjects.value.find((b) => b.name === name);
    if (btn) {
      btn.state = state;
      if (state && duration !== undefined) btn.duration = duration;
    }
  } catch (error) {
    console.error("Error sending press state:", error);
  }
};

onMounted(() => {
  fetchButtons();
  intervalHandle = window.setInterval(fetchButtons, 1000);
});

onBeforeUnmount(() => {
  if (intervalHandle !== null) clearInterval(intervalHandle);
});
</script>
