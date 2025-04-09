<template>
  <v-card class="pa-4">
    <v-btn
      :color="isPressed ? 'green' : 'primary'"
      variant="flat"
      @click="togglePress"
      class="mb-3"
    >
      {{ isPressed ? "Pressed" : "Press" }}
    </v-btn>

    <div class="mb-2">
      <strong>Duration:</strong>
      <span>{{ localDuration ? localDuration + "s" : "not set" }}</span>
    </div>

    <v-text-field
      v-model.number="localDuration"
      label="Set duration (seconds)"
      type="number"
      min="0"
      density="compact"
      hide-details
    />
  </v-card>
</template>

<script setup lang="ts">
import { defineProps, defineEmits, ref, watch } from "vue";

const props = defineProps<{
  isPressed: boolean;
  duration: number | null;
}>();

const emit = defineEmits<{
  (e: "pressed", duration: number | null): void;
  (e: "unpressed", duration: number | null): void;
}>();

const localDuration = ref<number | null>(props.duration);

// Sync if parent changes duration
watch(
  () => props.duration,
  (newVal) => {
    localDuration.value = newVal;
  }
);

const togglePress = () => {
  props.isPressed
    ? emit("unpressed", localDuration.value)
    : emit("pressed", localDuration.value);
};
</script>
