<template>
  <v-container class="pa-4">
    <v-card class="mb-4">
      <v-card-title>
        {{ deviceName }}
      </v-card-title>
    </v-card>

    <div v-if="ledStripeObjects.length === 0">
      <v-alert type="info" variant="tonal" class="mb-4">
        Loading stripes...
      </v-alert>
    </div>
    <v-row v-else dense>
      <v-col
        v-for="ledStripeObject in ledStripeObjects"
        :key="ledStripeObject.name"
        cols="12"
        md="6"
        lg="4"
      >
        <led-stripe-card
          :ledStripeObject="ledStripeObject"
          @send="(ledStripeObjectToSend: LedStripeObject) => sendStripe(ledStripeObjectToSend)"
        />
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import { getCurrentInstance, onMounted, ref } from "vue";
import { LedStripeObject } from "@/interfaces/LedStripeObject";
import LedStripeCard from "@/components/LedStripeCard.vue";

// eslint-disable-next-line no-undef
const props = defineProps<{
  deviceName: string;
}>();

const { proxy } = getCurrentInstance();

const ledStripeObjects = ref<LedStripeObject[]>([]);

const fetchStripes = async () => {
  try {
    const response = await proxy?.$axios.get(
      "/ledstripe/update/" + props.deviceName
    );
    ledStripeObjects.value = response.data;
  } catch (error) {
    console.error("Error fetching buttons:", error);
  }
};

const sendStripe = async (stripe: LedStripeObject) => {
  try {
    await proxy.$axios.post(`/ledstripe/stripeobject`, stripe);
    // Optionally refetch or show feedback
  } catch (error) {
    console.error("Error sending stripe:", error);
  }
};

onMounted(() => {
  fetchStripes();
});
</script>

<style scoped></style>
