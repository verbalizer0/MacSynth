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

// Fixed savePreset method for SpriteLayer.cpp
void SpriteLayer::savePreset(ofXml& xml) {
    // Save layer parameters
    xml.appendChild("density").set(ofToString(density));
    xml.appendChild("maxTrailLength").set(ofToString(maxTrailLength));
    xml.appendChild("spriteScale").set(ofToString(spriteScale));
    xml.appendChild("motionAmount").set(ofToString(motionAmount));
    xml.appendChild("blendMode").set(blendMode);
    xml.appendChild("audioReactivity").set(ofToString(audioReactivity));
    
    // Save sprites
    ofXml spritesXml;
    for (int i = 0; i < sprites.size(); i++) {
        ofXml spriteXml;
        spriteXml.appendChild("type").set(sprites[i]->getType());
        spriteXml.appendChild("x").set(ofToString(sprites[i]->getX()));
        spriteXml.appendChild("y").set(ofToString(sprites[i]->getY()));
        spriteXml.appendChild("scale").set(ofToString(sprites[i]->getScale()));
        spriteXml.appendChild("rotation").set(ofToString(sprites[i]->getRotation()));
        
        if (sprites[i]->getType() == "gif") {
            spriteXml.appendChild("path").set(((GifSprite*)sprites[i])->getPath());
        } else if (sprites[i]->getType() == "basic") {
            BasicSprite* basicSprite = (BasicSprite*)sprites[i];
            ofXml colorXml;
            colorXml.appendChild("r").set(ofToString(basicSprite->getColor().r));
            colorXml.appendChild("g").set(ofToString(basicSprite->getColor().g));
            colorXml.appendChild("b").set(ofToString(basicSprite->getColor().b));
            spriteXml.appendChild("color").appendChild(colorXml);
        }
        
        string spriteNodeName = "sprite" + ofToString(i);
        spritesXml.appendChild(spriteNodeName).appendChild(spriteXml);
    }
    
    xml.appendChild("sprites").appendChild(spritesXml);
}

// Fixed loadPreset method for SpriteLayer.cpp
void SpriteLayer::loadPreset(ofXml& xml) {
    // Clear existing sprites
    clearSprites();
    
    // Load layer parameters
    auto densityNode = xml.find("density");
    if (densityNode.size() > 0) {
        setDensity(ofToInt(xml.getChild("density").getValue()));
    }
    
    auto maxTrailLengthNode = xml.find("maxTrailLength");
    if (maxTrailLengthNode.size() > 0) {
        setMaxTrailLength(ofToInt(xml.getChild("maxTrailLength").getValue()));
    }
    
    auto spriteScaleNode = xml.find("spriteScale");
    if (spriteScaleNode.size() > 0) {
        spriteScale = ofToFloat(xml.getChild("spriteScale").getValue());
    }
    
    auto motionAmountNode = xml.find("motionAmount");
    if (motionAmountNode.size() > 0) {
        motionAmount = ofToFloat(xml.getChild("motionAmount").getValue());
    }
    
    auto blendModeNode = xml.find("blendMode");
    if (blendModeNode.size() > 0) {
        setBlendMode(xml.getChild("blendMode").getValue());
    }
    
    auto audioReactivityNode = xml.find("audioReactivity");
    if (audioReactivityNode.size() > 0) {
        setAudioReactivity(ofToFloat(xml.getChild("audioReactivity").getValue()));
    }
    
    // Load sprites
    auto spritesNode = xml.find("sprites");
    if (spritesNode.size() > 0) {
        ofXml spritesXml = xml.getChild("sprites");
        auto spriteNodes = spritesXml.getChildren();
        
        for (auto& spriteNodeParent : spriteNodes) {
            auto spriteNode = spriteNodeParent.getFirstChild();
            
            // Get sprite properties
            auto typeNode = spriteNode.getChild("type");
            if (!typeNode) continue;
            
            string type = typeNode.getValue();
            
            auto xNode = spriteNode.getChild("x");
            auto yNode = spriteNode.getChild("y");
            auto scaleNode = spriteNode.getChild("scale");
            auto rotationNode = spriteNode.getChild("rotation");
            
            if (!xNode || !yNode || !scaleNode || !rotationNode) continue;
            
            float x = ofToFloat(xNode.getValue());
            float y = ofToFloat(yNode.getValue());
            float scale = ofToFloat(scaleNode.getValue());
            float rotation = ofToFloat(rotationNode.getValue());
            
            if (type == "gif") {
                // Load GIF sprite
                auto pathNode = spriteNode.getChild("path");
                if (!pathNode) continue;
                
                string path = pathNode.getValue();
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
                auto colorNode = spriteNode.getChild("color");
                if (colorNode) {
                    auto rNode = colorNode.getChild("r");
                    auto gNode = colorNode.getChild("g");
                    auto bNode = colorNode.getChild("b");
                    
                    if (rNode && gNode && bNode) {
                        int r = ofToInt(rNode.getValue());
                        int g = ofToInt(gNode.getValue());
                        int b = ofToInt(bNode.getValue());
                        color = ofColor(r, g, b);
                    }
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