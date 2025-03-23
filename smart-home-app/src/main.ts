import { createApp } from "vue";
import App from "./App.vue";
import axiosPlugin from "./axiosPlugin";
import router from "./router";

const app = createApp(App);
app.use(axiosPlugin);
app.use(router);
app.mount("#app");
