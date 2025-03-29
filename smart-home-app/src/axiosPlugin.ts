import axios from "axios";
import { App } from "vue";

const apiHost = (window as any).APP_CONFIG?.apiHost || "http://localhost:8080";

const axiosInstance = axios.create({
  baseURL: apiHost,
});

export default {
  install(app: App) {
    app.config.globalProperties.$axios = axiosInstance;
  },
};
