# MacSynth

## Project Structure

MacSynth is organized into several components:

- **Core Application**: `ofApp.h/cpp`, `main.cpp`
- **Layers**: Background, Sprite, FX, and Camera layers in the `Layers/` directory
- **Utilities**: Audio analysis, effects, sprites in the `Utils/` directory
- **User Interface**: GUI elements in the `UI/` directory

## Using MacSynth

### Main Interface

Upon launching MacSynth, you'll see:

1. **Output Preview** - The central area showing visual output
2. **Scene Tabs** - Eight selectable scenes at the bottom
3. **Layer Controls** - Parameters for each layer on the right side
4. **Transport Controls** - Play/Stop buttons at the top

### Setting Up Audio Input

1. Go to the **Tempo** tab in the Layer Controls
2. Select your audio input device from the dropdown
3. Adjust input gain until you see the audio visualizer responding
4. Select a clock source (Audio, MIDI, CV, or Manual)

### Creating Visuals

#### Background Layer

1. Select the **Background** tab
2. Choose a source type (Color, Video, Camera, Pattern)
3. Adjust parameters like colors, feedback amount, and zoom
4. Enable feedback to create recursive effects

#### Sprite Layer

1. Select the **Sprites** tab
2. Import GIFs using the built-in Sprite Library
3. Adjust sprite count, trail length, and motion amount
4. Select blend mode for how sprites interact with the background

#### FX Layer

1. Select the **FX** tab
2. Enable effects like feedback, pixelate, glitch, strobe
3. Adjust parameters for each effect
4. Set intensity to control the overall effect strength

#### Camera Layer

1. Select the **Camera** tab
2. Choose your camera device
3. Enable feedback to send camera input to the background layer
4. Adjust position, scale, and rotation
5. Configure chroma key for transparent backgrounds

### Audio Reactivity

1. Ensure audio input is properly set up
2. For each layer, select which parameters respond to audio
3. Choose frequency bands (bass, mid, high) for different effects
4. Adjust reactivity amounts to control intensity

### Creating Scenes

1. Configure all layers as desired
2. Save the current setup as a scene preset
3. Repeat for different visual configurations
4. Use scene tabs to quickly switch between setups during performance

### Using Camera Feedback

For the distinctive feedback loops:

1. Position your camera to see the screen or projected output
2. Enable camera feedback in the Camera tab
3. Adjust feedback amount in the Background tab
4. Experiment with zoom, rotation, and color shift
5. Move objects in front of the camera for interesting effects

## Performance Tips

- **Preload Scenes**: Set up different scenes before your performance
- **Key Shortcuts**: Use number keys 1-8 to quickly switch scenes
- **Tempo Sync**: Match BPM to your music for synchronized visuals
- **Beat Patterns**: Configure effects to trigger on specific beat patterns
- **Layer Combinations**: Experiment with different layer combinations
- **Feedback Control**: Use feedback sparingly to avoid oversaturation

## Troubleshooting

### Common Issues

- **Performance Issues**: Reduce sprite count or disable complex effects
- **Audio Not Detected**: Check input device and permissions in System Preferences
- **Camera Not Working**: Verify camera permissions and connections
- **Feedback Overload**: Reset feedback amount to zero if visuals become too chaotic

### Debug Mode

Press 'D' to enable debug overlay showing:
- FPS (frames per second)
- BPM (beats per minute)
- Current beat and phase
- Active scene and clock source

## Customizing MacSynth

### Adding Custom Effects

1. Create a new effect class that inherits from `Effect`
2. Implement the required methods (setup, update, apply)
3. Add your effect to the FXLayer's `initializeDefaultEffects()` method

### Adding Sprite Behaviors

1. Modify the `SpriteBehavior` class to add new motion patterns
2. Implement the behavior in the `apply()` method
3. Add a new case to the switch statement in `applyMotion()`

## Resources

- [openFrameworks Documentation](https://openframeworks.cc/documentation/)
- [MacSynth GitHub Repository](https://github.com/yourusername/macsynth)
- [GIF Resources](https://giphy.com/explore/transparent-background)

## Credits

MacSynth was developed using openFrameworks and the following addons:
- ofxGif by armadillu
- ofxFft by Kyle McDonald
- ofxImGui by Jason Van Cleave

## License

MacSynth is released under the MIT License. See LICENSE file for details.
