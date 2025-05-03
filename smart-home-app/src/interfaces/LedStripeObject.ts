export interface LedStripeObject {
  name: string;
  mode: Modes;
  red: number;
  green: number;
  blue: number;
  brightness: number;
  deviceName: string;
}

export const modes = ["OFF", "THEATER", "RAINBOW", "COLOR"] as const;
export type Modes = (typeof modes)[number];
