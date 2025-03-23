/* eslint-disable prettier/prettier */
import { createRouter, createWebHashHistory, RouteRecordRaw } from "vue-router";

const routes: Array<RouteRecordRaw> = [
  {
    path: "/drawing",
    name: "drawing",
    component: () =>
      import( "../views/DrawingView.vue"),
  },
];

const router = createRouter({
  history: createWebHashHistory(),
  routes,
});

export default router;
