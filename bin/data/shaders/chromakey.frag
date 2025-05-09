#version 150

uniform sampler2D tex0;
uniform vec3 chromaColor;
uniform float tolerance;

in vec2 texCoordVarying;
out vec4 outputColor;

void main() {
    // Sample the texture
    vec4 color = texture(tex0, texCoordVarying);
    
    // Calculate color distance from chroma key color
    float distance = length(color.rgb - chromaColor);
    
    // Create a smooth mask based on distance
    float mask = smoothstep(tolerance, tolerance + 0.1, distance);
    
    // Apply mask to alpha channel
    color.a *= mask;
    
    // Output the result
    outputColor = color;
}
