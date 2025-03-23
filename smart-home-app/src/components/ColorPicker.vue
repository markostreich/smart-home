<template>
  <div>
    <input type="color" :value="pickedColor" @input="updateColor" />
  </div>
</template>

<script setup lang="ts">
import { ref, defineEmits } from "vue";

// Define the type for the emit function
interface Emits {
  (event: "update:modelValue", color: string): void;
}

const emits = defineEmits<Emits>();
const pickedColor = ref("#ff0000"); // Default color set to red

// Function to be called when the input value changes
function updateColor(color: string) {
  pickedColor.value = color;
  emits("update:modelValue", color);
}
</script>

<style>
/* Style your color picker if needed */
</style>
