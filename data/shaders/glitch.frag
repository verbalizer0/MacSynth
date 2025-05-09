#version 150

uniform sampler2D tex0;
uniform float amount;
uniform float time;
uniform vec2 resolution;
uniform bool colorShift;
uniform bool scanlines;
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
    
    // Apply glitch amount
    float glitchAmount = amount;
    
    // Calculate glitch based on time and position
    float glitchLine = floor(uv.y * 20.0) / 20.0;
    float randomValue = random(vec2(glitchLine, floor(time * 10.0)));
    
    // Only apply glitch to some lines based on randomness
    if (randomValue < glitchAmount * 0.5) {
        // Horizontal offset for the glitch
        float offsetX = (random(vec2(glitchLine, time)) - 0.5) * 0.1 * glitchAmount;
        uv.x += offsetX;
    }
    
    // Add some vertical glitches too
    if (randomValue > 0.8 && randomValue < 0.83) {
        uv.y = fract(uv.y + random(vec2(floor(time * 20.0))));
    }
    
    // Sample the texture with glitched coordinates
    vec4 color = texture(tex0, uv);
    
    // Apply RGB shift if enabled
    if (colorShift) {
        // Separate color channels
        float rOffset = (random(vec2(time, 0.0)) - 0.5) * 0.01 * glitchAmount;
        float gOffset = (random(vec2(time, 0.1)) - 0.5) * 0.01 * glitchAmount;
        float bOffset = (random(vec2(time, 0.2)) - 0.5) * 0.01 * glitchAmount;
        
        // Sample each color channel with different offsets
        float r = texture(tex0, vec2(uv.x + rOffset, uv.y)).r;
        float g = texture(tex0, vec2(uv.x + gOffset, uv.y)).g;
        float b = texture(tex0, vec2(uv.x + bOffset, uv.y)).b;
        
        // Combine channels
        color = vec4(r, g, b, color.a);
    }
    
    // Apply scanlines if enabled
    if (scanlines) {
        float scanlineY = fract(uv.y * resolution.y * 0.5);
        float scanlineIntensity = 0.1 + 0.05 * sin(time * 10.0);
        
        if (scanlineY < 0.5) {
            color.rgb *= (1.0 - scanlineIntensity);
        }
    }
    
    // Apply digital noise if enabled
    if (noiseAmount > 0.0) {
        float noise = random(uv * time);
        
        // Only apply noise to some pixels based on threshold
        if (noise > (1.0 - noiseAmount * glitchAmount)) {
            // Generate random RGB values
            float noiseR = random(uv + vec2(0.1, time));
            float noiseG = random(uv + vec2(0.2, time));
            float noiseB = random(uv + vec2(0.3, time));
            
            // Mix with original color based on noise amount
            color.rgb = mix(color.rgb, vec3(noiseR, noiseG, noiseB), noiseAmount * glitchAmount);
        }
    }
    
    // Output the result
    outputColor = color;
}
