<template>
  <v-container class="pa-4">
    <v-card>
      <v-card-title>Drawing</v-card-title>
      <v-card-text>
        <DrawingField />
      </v-card-text>
    </v-card>
  </v-container>
</template>

<script setup lang="ts">
import DrawingField from "@/components/DrawingField.vue";
</script>
