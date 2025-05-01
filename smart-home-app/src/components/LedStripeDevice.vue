<template>
  <v-container class="pa-4">
    <v-card class="mb-4">
      <v-card-title>
        {{ deviceName }}
      </v-card-title>
    </v-card>

    <div v-if="lesStripeObjects.length === 0">
      <v-alert type="info" variant="tonal" class="mb-4">
        Loading stripes...
      </v-alert>
    </div>
    <v-row v-else dense>
      <v-col
        v-for="leStripeObject in lesStripeObjects"
        :key="leStripeObject.name"
        cols="12"
        md="6"
        lg="4"
      >
        <v-card outlined>
          <v-card-title class="text-h6">{{ leStripeObject.name }}</v-card-title>
          <v-col cols="12" md="7">
            <v-select v-model="selectedMode" :items="modes" outlined dense />
          </v-col>
          <v-col cols="12" md="7">
            <v-slider
              v-model="brightness"
              label="Brightness"
              min="0"
              max="255"
              step="1"
              thumb-label="always"
              hide-details
              dense
            />
          </v-col>
          <v-col cols="12" md="7">
            <v-color-picker
              v-model="color"
              hide-canvas
              mode="rgb"
              hide-inputs
              flat
              show-swatches
              swatches-max-height="100"
            />
          </v-col>
          <v-col cols="12" md="7">
            <v-btn color="primary" @click="sendStripe(leStripeObject)">
              Send
            </v-btn>
          </v-col>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import { getCurrentInstance, onBeforeUnmount, onMounted, ref } from "vue";
import { LedStripeObject } from "@/interfaces/LedStripeObject";

// eslint-disable-next-line no-undef
const props = defineProps<{
  deviceName: string;
}>();

const { proxy } = getCurrentInstance();

const lesStripeObjects = ref<LedStripeObject[]>([]);
let intervalHandle: number | null = null;

const modes = ["OFF", "THEATER", "RAINBOW", "COLOR"] as const;
const selectedMode = ref<(typeof modes)[number] | null>("COLOR");
const color = ref("#0000FF");
const brightness = ref<number>(100);

const parseRgbString = (hex: string): [number, number, number] => {
  let cleaned = hex.replace(/^#/, "");
  if (cleaned.length === 3) {
    cleaned = cleaned
      .split("")
      .map((c) => c + c)
      .join("");
  }
  if (cleaned.length !== 6) {
    return [0, 0, 0];
  }
  const num = parseInt(cleaned, 16);
  const r = (num >> 16) & 0xff;
  const g = (num >> 8) & 0xff;
  const b = num & 0xff;
  return [r, g, b];
};

const fetchStripes = async () => {
  try {
    const response = await proxy?.$axios.get(
      "/ledstripe/update/" + props.deviceName
    );
    lesStripeObjects.value = response.data;
  } catch (error) {
    console.error("Error fetching buttons:", error);
  }
};

const sendStripe = async (stripe: LedStripeObject) => {
  const [red, green, blue] = parseRgbString(color.value);
  const payload = {
    name: stripe.name,
    mode: selectedMode.value,
    red,
    green,
    blue,
    brightness: brightness.value,
    deviceName: props.deviceName,
  };
  try {
    await proxy.$axios.post(`/ledstripe/stripeobject`, payload);
    // Optionally refetch or show feedback
  } catch (error) {
    console.error("Error sending stripe:", error);
  }
};

onMounted(() => {
  fetchStripes();
  intervalHandle = window.setInterval(fetchStripes, 1000);
});

onBeforeUnmount(() => {
  if (intervalHandle !== null) clearInterval(intervalHandle);
});
</script>

<style scoped></style>
