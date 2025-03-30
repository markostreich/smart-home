<template>
  <v-container class="pa-4">
    <v-card>
      <v-card-text>
        <canvas
          id="drawingCanvas"
          @mousedown="startDrawing"
          @mousemove="draw"
          @mouseup="stopDrawing"
          :width="width"
          :height="height"
        ></canvas>
      </v-card-text>
    </v-card>

    <v-row class="mt-4" align="center" justify="space-between">
      <v-col cols="12" md="4">
        <v-color-picker
          v-model="color"
          hide-canvas
          mode="hexa"
          flat
          show-swatches
          swatches-max-height="100"
        />
      </v-col>

      <v-col cols="12" md="4">
        <v-switch v-model="isGrid" label="Show Grid Pattern" inset />
      </v-col>

      <v-col cols="12" md="4" class="d-flex justify-center">
        <v-btn color="red" variant="tonal" @click="clearBoard"> Clear </v-btn>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import { ref, onMounted, getCurrentInstance } from "vue";

const { proxy } = getCurrentInstance();

const isDrawing = ref(false);
const color = ref("#ff0000");
const isGrid = ref(true);
let context: CanvasRenderingContext2D | null = null;
let canvas: HTMLCanvasElement | null = null;

const CELL_AMOUNT_X = 16;
const CELL_AMOUNT_Y = 16;
const width = ref(400);
const height = ref(400);
const CELL_WIDTH = width.value / CELL_AMOUNT_X;
const CELL_HEIGHT = height.value / CELL_AMOUNT_Y;
const LINE_WIDTH = 1;

let pixelData: string[][] = Array.from({ length: CELL_AMOUNT_Y }, () =>
  Array.from({ length: CELL_AMOUNT_X }, () => "000000")
);

const startDrawing = (event: MouseEvent) => {
  if (!context) return;
  isDrawing.value = true;
  let x = event.offsetX;
  let y = event.offsetY;
  _draw(x, y);
};

const draw = (event: MouseEvent) => {
  if (!isDrawing.value || !context) return;
  let x = event.offsetX;
  let y = event.offsetY;
  _draw(x, y);
};

const stopDrawing = () => {
  isDrawing.value = false;
  postImage();
};

const _draw = (x: number, y: number) => {
  if (!context) return;
  x = getXInGrid(x);
  y = getYInGrid(y);
  let posX = x / CELL_WIDTH;
  let posY = CELL_AMOUNT_Y - 1 - y / CELL_HEIGHT;
  putPixel(posX, posY, color.value);
  context.fillStyle = color.value;
  context.fillRect(
    x + LINE_WIDTH,
    y + LINE_WIDTH,
    CELL_WIDTH - LINE_WIDTH - 1,
    CELL_HEIGHT - LINE_WIDTH - 1
  );
  context.fill();
};

const getXInGrid = (x: number): number => {
  return Math.floor(x / CELL_WIDTH) * CELL_WIDTH;
};

const getYInGrid = (y: number): number => {
  return Math.floor(y / CELL_HEIGHT) * CELL_HEIGHT;
};

const clearBoard = () => {
  context?.clearRect(0, 0, width.value, height.value);
  context?.beginPath();
  drawBoard(context, width.value, height.value, CELL_AMOUNT_X, CELL_AMOUNT_Y);
  initPixelData();
};

onMounted(() => {
  initCanvas();
  drawBoard(context, width.value, height.value, CELL_AMOUNT_X, CELL_AMOUNT_Y);
  initPixelData();
});

const initPixelData = () => {
  pixelData = Array.from({ length: CELL_AMOUNT_Y }, () =>
    Array.from({ length: CELL_AMOUNT_X }, () => "000000")
  );
};

const putPixel = (_x: number, _y: number, color: string) => {
  pixelData[_y][_x] = color.slice(1);
  logPixelData();
};

const logPixelData = () => {
  let result = "";
  pixelData.forEach((row, rowIndex) => {
    row.forEach((value, colIndex) => {
      if (value !== "000000") result += pixelToHex(colIndex, rowIndex, value);
    });
  });
  console.log(result);
};

const convertPixelDataToHex = (): string => {
  let result = "";
  pixelData.forEach((row, rowIndex) => {
    row.forEach((value, colIndex) => {
      if (value !== "000000") result += pixelToHex(colIndex, rowIndex, value);
    });
  });
  return result;
};

const initCanvas = () => {
  canvas = document.getElementById("drawingCanvas") as HTMLCanvasElement;
  context = canvas.getContext("2d");
  if (context != null) context.imageSmoothingEnabled = false;
};

const drawBoard = (
  drawingContext: CanvasRenderingContext2D | null,
  width: number,
  height: number,
  cellAmountX: number,
  cellAmountY: number
) => {
  if (!drawingContext) return;
  drawingContext.globalAlpha = 1;
  drawingContext.fillStyle = "#000";
  drawingContext.fillRect(0, 0, width - 1, height - 1);
  drawingContext.fill();
  if (isGrid.value)
    drawGrid(drawingContext, width, height, cellAmountX, cellAmountY);
};

const drawGrid = (
  drawingContext: CanvasRenderingContext2D | null,
  width: number,
  height: number,
  cellAmountX: number,
  cellAmountY: number
) => {
  for (let x = 0; x <= width; x += width / cellAmountX) {
    drawingContext?.moveTo(0.5 + x, 0);
    drawingContext?.lineTo(0.5 + x, height);
  }
  for (let y = 0; y <= height; y += height / cellAmountY) {
    drawingContext?.moveTo(0, 0.5 + y);
    drawingContext?.lineTo(width, 0.5 + y);
  }
  if (drawingContext != null) {
    drawingContext.strokeStyle = "rgba(200,200,200,1.0)";
    drawingContext.stroke();
  }
  color.value = "#ff0000";
};

const pixelToHex = (x: number, y: number, color: string): string => {
  return _padHex(x.toString(16)) + _padHex(y.toString(16)) + color;
};

const _padHex = (hex: string) => {
  return hex.padStart(2, "0");
};

const postImage = () => {
  const ledPanelObject = {
    name: "heart",
    pos_x: 0,
    pos_y: 0,
    rotationPoint_x: 8,
    rotationPoint_y: 8,
    imageData: convertPixelDataToHex(),
    deviceName: "deviceOne",
  };
  proxy?.$axios.post("/ledpanel/panelobject", ledPanelObject);
};
</script>

<style scoped>
#drawingCanvas {
  border: 2px solid #444;
  cursor: cell;
  display: block;
  margin: 0 auto;
}
</style>
