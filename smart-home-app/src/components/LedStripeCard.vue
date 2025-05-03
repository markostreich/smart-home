<template>
  <v-card outlined>
    <v-card-title class="text-h6">{{ ledStripeObject.name }}</v-card-title>
    <v-col cols="12" md="7">
      <v-select
        v-model="ledStripeObject.mode"
        :items="modes"
        outlined
        dense
        @update:modelValue="sendStripe(ledStripeObject)"
      />
    </v-col>
    <v-col cols="12" md="7">
      <v-slider
        v-model="ledStripeObject.brightness"
        label="Brightness"
        min="0"
        max="255"
        step="1"
        thumb-label="always"
        hide-details
        dense
        @click="sendStripe(ledStripeObject)"
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
        @click="sendStripe(ledStripeObject)"
      />
    </v-col>
  </v-card>
</template>

<script setup lang="ts">
import { LedStripeObject, modes } from "@/interfaces/LedStripeObject";
import { ref } from "vue";

// eslint-disable-next-line no-undef
const props = defineProps<{
  ledStripeObject: LedStripeObject;
}>();

// eslint-disable-next-line no-undef
const emit = defineEmits<{
  (e: "send", ledStripeObject: LedStripeObject): void;
}>();

const ledStripeObject = ref<LedStripeObject>(props.ledStripeObject);

const rgbToHex = ([r, g, b]: [number, number, number]): string => {
  return (
    "#" +
    [r, g, b]
      .map((n) => n.toString(16).padStart(2, "0"))
      .join("")
      .toUpperCase()
  );
};

const color = ref<string>(
  rgbToHex([
    props.ledStripeObject.red,
    props.ledStripeObject.green,
    props.ledStripeObject.blue,
  ])
);

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

const sendStripe = (ledStripeObject: LedStripeObject) => {
  const [red, green, blue] = parseRgbString(color.value);
  ledStripeObject.red = red;
  ledStripeObject.green = green;
  ledStripeObject.blue = blue;
  emit("send", ledStripeObject);
};
</script>

<style scoped></style>
