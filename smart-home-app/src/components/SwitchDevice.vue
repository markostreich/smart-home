<template>
  <div>
    <h2>Press Buttons from API</h2>
    <div v-if="buttons.length === 0">Loading buttons...</div>
    <div v-for="button in buttons" :key="button.id" class="button-entry">
      <div>{{ button.name }}</div>
      <SwitchButton
        @pressed="handlePress(button.id, true)"
        @unpressed="handlePress(button.id, false)"
      />
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import SwitchButton from "./SwitchButton.vue";
import axios from "axios";

const buttons = ref([]);

const fetchButtons = async () => {
  try {
    const response = await axios.get("/api/buttons");
    buttons.value = response.data;
  } catch (error) {
    console.error("Error fetching buttons:", error);
  }
};

const handlePress = async (id, pressed) => {
  try {
    await axios.post("/api/button-press", {
      id,
      pressed,
    });
    console.log(`Button ${id} ${pressed ? "pressed" : "unpressed"}`);
  } catch (error) {
    console.error("Error sending press state:", error);
  }
};

onMounted(fetchButtons);
</script>

<style scoped>
.button-entry {
  border: 1px solid #ccc;
  padding: 1rem;
  margin-bottom: 1rem;
}
</style>
