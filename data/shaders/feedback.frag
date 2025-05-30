#version 150

uniform sampler2D tex0;
uniform float colorShift;

in vec2 texCoordVarying;
out vec4 outputColor;

vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    // Get the original color
    vec4 color = texture(tex0, texCoordVarying);
    
    // Convert to HSV
    vec3 hsv = rgb2hsv(color.rgb);
    
    // Apply hue shift
    hsv.x = fract(hsv.x + colorShift);
    
    // Convert back to RGB
    color.rgb = hsv2rgb(hsv);
    
    // Output the result
    outputColor = color;
}
