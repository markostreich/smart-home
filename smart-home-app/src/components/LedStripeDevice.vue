<template>
  <v-container class="pa-4">
    <v-card class="mb-4">
      <v-card-title>
        {{ deviceName }}
      </v-card-title>
    </v-card>

    <div v-if="lesStripeObjects.length === 0">
      <v-alert type="info" variant="tonal" class="mb-4">
        Loading buttons...
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
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import {
  defineProps,
  getCurrentInstance,
  onBeforeUnmount,
  onMounted,
  ref,
} from "vue";

const props = defineProps<{
  deviceName: string;
}>();

const { proxy } = getCurrentInstance();

const lesStripeObjects = ref<LedStripeObject[]>([]);
let intervalHandle: number | null = null;

interface LedStripeObject {
  name: string;
  mode: string;
  red: number | null;
  green: number | null;
  blue: number | null;
  brightness: number;
  deviceName: string;
}

const fetchButtons = async () => {
  try {
    const response = await proxy?.$axios.get(
      "/ledstripe/update/" + props.deviceName
    );
    lesStripeObjects.value = response.data;
  } catch (error) {
    console.error("Error fetching buttons:", error);
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

<style scoped></style>
