<template>
  <div class="drawing">
    <DrawingField />
  </div>
</template>

<script lang="ts">
import { defineComponent } from "vue";
import DrawingField from "@/components/DrawingField.vue";

export default defineComponent({
  name: "DrawingView",
  components: {
    DrawingField,
  },
});
</script>
