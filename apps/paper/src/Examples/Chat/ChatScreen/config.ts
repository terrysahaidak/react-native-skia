// renders each message offscreen into textures and renders SkImages for each message
export const RASTERIZE_MESSAGES = true;
// offscreen uses regular skia offscreen surface + skia dom canvas to render the texture
// onscreen uses skia context with webgpu canvas
export const OFFSCREEN_RENDERING = true;
// renders the background of the chat offscreen into texture
// renders texture in other places as Home and in the chat itself
export const RENDER_BACKGROUND = true;
// used to debug the rendering of the chat when opening the chat from notification
export const SHOW_NOTIFICATION = false;