#version 150

uniform sampler2D tex0;
uniform float time;
uniform float amplitude;
uniform float frequency;
uniform float speed;
uniform vec2 resolution;
uniform string direction; // "horizontal", "vertical", "both"
uniform float noiseAmount;

in vec2 texCoordVarying;
out vec4 outputColor;

// Random function
float random(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    // Get the original texture coordinates
    vec2 uv = texCoordVarying;
    
    // Apply wave distortion
    if (direction == "horizontal" || direction == "both") {
        // Horizontal wave
        float xWave = amplitude * 0.01 * sin(uv.y * frequency * 10.0 + time * speed);
        // Add noise if enabled
        if (noiseAmount > 0.0) {
            xWave += (random(vec2(uv.y, time)) - 0.5) * noiseAmount * 0.02;
        }
        uv.x += xWave;
    }
    
    if (direction == "vertical" || direction == "both") {
        // Vertical wave
        float yWave = amplitude * 0.01 * sin(uv.x * frequency * 10.0 + time * speed);
        // Add noise if enabled
        if (noiseAmount > 0.0) {
            yWave += (random(vec2(uv.x, time)) - 0.5) * noiseAmount * 0.02;
        }
        uv.y += yWave;
    }
    
    // Sample the texture with distorted coordinates
    // Clamp to valid texture coordinates
    uv = clamp(uv, vec2(0.0), vec2(1.0));
    vec4 color = texture(tex0, uv);
    
    // Output the result
    outputColor = color;
}
