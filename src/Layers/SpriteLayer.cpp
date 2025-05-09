// File: src/Layers/SpriteLayer.cpp
#include "SpriteLayer.h"

SpriteLayer::SpriteLayer() {
    width = 1280;
    height = 720;
    
    // Default parameters
    density = 5;
    maxTrailLength = 10;
    spriteScale = 1.0;
    motionAmount = 1.0;
    blendMode = "screen";
    audioReactivity = 0.5;
}

SpriteLayer::~SpriteLayer() {
    // Clean up resources
    clearSprites();
}

void SpriteLayer::setup(int width, int height) {
    this->width = width;
    this->height = height;
    
    // Set up FBO
    outputFbo.allocate(width, height, GL_RGBA);
    
    // Clear FBO
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    outputFbo.end();
    
    // Initialize sprites list
    clearSprites();
}

void SpriteLayer::update(float deltaTime, float* audioData, int numBands) {
    // Update each sprite
    for (auto& sprite : sprites) {
        sprite->update(deltaTime, audioData, numBands);
    }
    
    // Maintain sprite density
    maintainDensity();
}

void SpriteLayer::draw() {
    outputFbo.begin();
    ofClear(0, 0, 0, 0);
    
    // Apply blend mode
    if (blendMode == "add" || blendMode == "additive") {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
    } else if (blendMode == "multiply") {
        ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    } else if (blendMode == "screen") {
        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    } else if (blendMode == "subtract") {
        ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
    } else if (blendMode == "alpha") {
        ofEnableAlphaBlending();
    } else {
        ofEnableAlphaBlending();
    }
    
    // Draw each sprite
    for (auto& sprite : sprites) {
        sprite->draw(width, height);
    }
    
    // Reset blend mode
    ofEnableAlphaBlending();
    
    outputFbo.end();
}

void SpriteLayer::addSprite(Sprite* sprite) {
    // Generate a unique ID if needed
    if (sprite->getId() == "") {
        sprite->setId(generateSpriteId());
    }
    
    // Add to used IDs
    usedIds.insert(sprite->getId());
    
    // Set sprite parameters
    sprite->setScale(sprite->getScale() * spriteScale);
    sprite->setMotionSpeed(sprite->getMotionSpeed() * motionAmount);
    sprite->setMaxTrailLength(maxTrailLength);
    sprite->setAudioReactivity(audioReactivity);
    
    // Add to sprites list
    sprites.push_back(sprite);
}

void SpriteLayer::removeSprite(string id) {
    for (auto it = sprites.begin(); it != sprites.end(); it++) {
        if ((*it)->getId() == id) {
            // Remove from used IDs
            usedIds.erase(id);
            
            // Delete sprite and remove from list
            delete *it;
            sprites.erase(it);
            break;
        }
    }
}

void SpriteLayer::clearSprites() {
    // Delete all sprites
    for (auto& sprite : sprites) {
        delete sprite;
    }
    
    // Clear lists
    sprites.clear();
    usedIds.clear();
}

void SpriteLayer::setMaxTrailLength(int length) {
    maxTrailLength = length;
    
    // Update all sprites
    for (auto& sprite : sprites) {
        sprite->setMaxTrailLength(length);
    }
}

void SpriteLayer::setSpriteScale(float scale) {
    // Calculate relative change
    float scaleFactor = scale / spriteScale;
    
    // Update parameter
    spriteScale = scale;
    
    // Update all sprites
    for (auto& sprite : sprites) {
        sprite->setScale(sprite->getScale() * scaleFactor);
    }
}

void SpriteLayer::setMotionAmount(float amount) {
    // Calculate relative change
    float motionFactor = amount / motionAmount;
    
    // Update parameter
    motionAmount = amount;
    
    // Update all sprites
    for (auto& sprite : sprites) {
        sprite->setMotionSpeed(sprite->getMotionSpeed() * motionFactor);
    }
}

void SpriteLayer::maintainDensity() {
    // If we need more sprites, add them
    while (sprites.size() < density) {
        // In a real implementation, this would add sprites from a library
        // For now, create a basic sprite
        float x = ofRandom(0, 1);
        float y = ofRandom(0, 1);
        float scale = ofRandom(0.5, 1.5) * spriteScale;
        float rotation = ofRandom(0, TWO_PI);
        ofColor color = ofColor(ofRandom(100, 255), ofRandom(100, 255), ofRandom(100, 255));
        
        BasicSprite* sprite = new BasicSprite();
        sprite->setup(x, y, scale, rotation, color);
        sprite->setMaxTrailLength(maxTrailLength);
        sprite->setMotionSpeed(ofRandom(0.1, 0.3) * motionAmount);
        sprite->setAudioReactivity(audioReactivity);
        
        // Set random motion behavior
        float motionType = ofRandom(0, 3);
        if (motionType < 1) {
            sprite->setMotionType(MOTION_CIRCULAR);
        } else if (motionType < 2) {
            sprite->setMotionType(MOTION_BOUNCE);
        } else {
            sprite->setMotionType(MOTION_WAVE);
        }
        
        addSprite(sprite);
    }
    
    // If we have too many sprites, remove some
    while (sprites.size() > density) {
        // Remove the last sprite
        removeSprite(sprites.back()->getId());
    }
}

string SpriteLayer::generateSpriteId() {
    // Generate a unique ID
    string id;
    do {
        id = "sprite_" + ofToString(ofRandom(100000));
    } while (usedIds.find(id) != usedIds.end());
    
    return id;
}

void SpriteLayer::savePreset(ofXml& xml) {
    // Save layer parameters
    xml.addChild("density").set(density);
    xml.addChild("maxTrailLength").set(maxTrailLength);
    xml.addChild("spriteScale").set(spriteScale);
    xml.addChild("motionAmount").set(motionAmount);
    xml.addChild("blendMode").set(blendMode);
    xml.addChild("audioReactivity").set(audioReactivity);
    
    // Save sprites
    ofXml spritesXml;
    for (int i = 0; i < sprites.size(); i++) {
        ofXml spriteXml;
        spriteXml.addChild("type").set(sprites[i]->getType());
        spriteXml.addChild("x").set(sprites[i]->getX());
        spriteXml.addChild("y").set(sprites[i]->getY());
        spriteXml.addChild("scale").set(sprites[i]->getScale());
        spriteXml.addChild("rotation").set(sprites[i]->getRotation());
        
        if (sprites[i]->getType() == "gif") {
            spriteXml.addChild("path").set(((GifSprite*)sprites[i])->getPath());
        } else if (sprites[i]->getType() == "basic") {
            BasicSprite* basicSprite = (BasicSprite*)sprites[i];
            ofXml colorXml;
            colorXml.addChild("r").set(basicSprite->getColor().r);
            colorXml.addChild("g").set(basicSprite->getColor().g);
            colorXml.addChild("b").set(basicSprite->getColor().b);
            spriteXml.addChild("color").appendChild(colorXml);
        }
        
        spritesXml.addChild("sprite").appendChild(spriteXml);
    }
    
    xml.addChild("sprites").appendChild(spritesXml);
}

void SpriteLayer::loadPreset(ofXml& xml) {
    // Clear existing sprites
    clearSprites();
    
    // Load layer parameters
    if (xml.find("density").size() > 0) {
        setDensity(xml.getChild("density").getIntValue());
    }
    
    if (xml.find("maxTrailLength").size() > 0) {
        setMaxTrailLength(xml.getChild("maxTrailLength").getIntValue());
    }
    
    if (xml.find("spriteScale").size() > 0) {
        spriteScale = xml.getChild("spriteScale").getFloatValue();
    }
    
    if (xml.find("motionAmount").size() > 0) {
        motionAmount = xml.getChild("motionAmount").getFloatValue();
    }
    
    if (xml.find("blendMode").size() > 0) {
        setBlendMode(xml.getChild("blendMode").getValue());
    }
    
    if (xml.find("audioReactivity").size() > 0) {
        setAudioReactivity(xml.getChild("audioReactivity").getFloatValue());
    }
    
    // Load sprites
    if (xml.find("sprites").size() > 0) {
        ofXml spritesXml = xml.getChild("sprites");
        
        vector<ofXml> spriteXmlList = spritesXml.getChildren("sprite");
        for (auto& spriteXml : spriteXmlList) {
            string type = spriteXml.getChild("type").getValue();
            float x = spriteXml.getChild("x").getFloatValue();
            float y = spriteXml.getChild("y").getFloatValue();
            float scale = spriteXml.getChild("scale").getFloatValue();
            float rotation = spriteXml.getChild("rotation").getFloatValue();
            
            if (type == "gif") {
                // Load GIF sprite
                string path = spriteXml.getChild("path").getValue();
                if (ofFile::doesFileExist(path)) {
                    GifSprite* sprite = new GifSprite();
                    sprite->setup(path, x, y, scale, rotation);
                    sprite->setMaxTrailLength(maxTrailLength);
                    sprite->setMotionSpeed(motionAmount);
                    sprite->setAudioReactivity(audioReactivity);
                    addSprite(sprite);
                }
            } else if (type == "basic") {
                // Load basic sprite
                ofColor color = ofColor::white;
                if (spriteXml.find("color").size() > 0) {
                    ofXml colorXml = spriteXml.getChild("color");
                    int r = colorXml.getChild("r").getIntValue();
                    int g = colorXml.getChild("g").getIntValue();
                    int b = colorXml.getChild("b").getIntValue();
                    color = ofColor(r, g, b);
                }
                
                BasicSprite* sprite = new BasicSprite();
                sprite->setup(x, y, scale, rotation, color);
                sprite->setMaxTrailLength(maxTrailLength);
                sprite->setMotionSpeed(motionAmount);
                sprite->setAudioReactivity(audioReactivity);
                addSprite(sprite);
            }
        }
    }
    
    // Ensure we maintain density if needed
    maintainDensity();
}
