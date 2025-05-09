#version 150

uniform sampler2D tex0;
uniform float sizeX;
uniform float sizeY;
uniform float threshold;
uniform vec2 resolution;

in vec2 texCoordVarying;
out vec4 outputColor;

void main() {
    // Calculate the pixel size based on the resolution and size parameters
    vec2 pixelSize = vec2(max(1.0, sizeX) / resolution.x, max(1.0, sizeY) / resolution.y);
    
    // Calculate the pixelated coordinates
    vec2 pixelCoord = floor(texCoordVarying / pixelSize) * pixelSize + (pixelSize * 0.5);
    
    // Sample the texture at the pixelated coordinates
    vec4 color = texture(tex0, pixelCoord);
    
    // Apply threshold if enabled
    if (threshold < 1.0) {
        float brightness = (color.r + color.g + color.b) / 3.0;
        if (brightness < threshold) {
            color = vec4(0.0, 0.0, 0.0, color.a);
        }
    }
    
    // Output the result
    outputColor = color;
}
