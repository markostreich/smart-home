<template>
  <div class="switch-button">
    <button :class="{ pressed: isPressed }" @click="togglePress">
      {{ isPressed ? "Pressed" : "Press" }}
    </button>
    <div class="duration-field">
      Duration:
      <span>{{ duration ? duration + "s" : "not set" }}</span>
    </div>
    <input
      type="number"
      v-model.number="duration"
      placeholder="Set duration (seconds)"
      min="0"
    />
  </div>
</template>

<script setup>
import { ref, onUnmounted, defineEmits } from "vue";

const emit = defineEmits(["pressed", "unpressed"]);

const isPressed = ref(false);
const duration = ref(null);
let timeoutHandle = null;

const togglePress = () => {
  if (isPressed.value) {
    unpressButton();
  } else {
    pressButton();
  }
};

const pressButton = () => {
  isPressed.value = true;
  emit("pressed");
  if (duration.value && duration.value > 0) {
    clearTimeout(timeoutHandle);
    timeoutHandle = setTimeout(unpressButton, duration.value * 1000);
  }
};

const unpressButton = () => {
  isPressed.value = false;
  emit("unpressed");
  clearTimeout(timeoutHandle);
};

onUnmounted(() => {
  clearTimeout(timeoutHandle);
});
</script>

<style scoped>
.pressed {
  background-color: green;
  color: white;
}
</style>
