import axios from "axios";
import { App } from "vue";

export default {
  install(app: App) {
    app.config.globalProperties.$axios = axios;
  },
};
