<template>
  <v-app>
    <v-app-bar app color="primary" dark>
      <v-toolbar-title>Smart Home</v-toolbar-title>
      <v-spacer />
      <v-btn to="/drawing" text exact> Drawing </v-btn>
    </v-app-bar>

    <v-main>
      <router-view />
    </v-main>
  </v-app>
</template>

<script setup lang="ts">
// no script needed here unless you’re adding logic
</script>

<style scoped>
.v-btn.router-link-exact-active {
  background-color: rgba(255, 255, 255, 0.2);
}
</style>
