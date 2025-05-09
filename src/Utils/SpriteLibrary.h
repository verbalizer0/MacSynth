// File: src/Utils/SpriteLibrary.h
#pragma once

#include "ofMain.h"
#include "Sprite.h"

// Struct to hold sprite information
struct SpriteInfo {
    string id;
    string name;
    string path;
    string category;
    string thumbnailPath;
    bool analyzed;
    int frameCount;
    int width;
    int height;
    vector<float> frameDurations;
};

class SpriteLibrary {
public:
    SpriteLibrary();
    ~SpriteLibrary();
    
    void setup();
    
    // Add a sprite to the library
    bool addSprite(string path, string category = "custom", string name = "");
    
    // Add a sprite from URL
    bool addSpriteFromURL(string url, string category = "custom", string name = "");
    
    // Remove a sprite from the library
    bool removeSprite(string id);
    
    // Get a sprite by ID
    SpriteInfo* getSpriteById(string id);
    
    // Get sprites in a category
    vector<SpriteInfo*> getSpritesByCategory(string category);
    
    // Get all sprites
    vector<SpriteInfo*> getAllSprites();
    
    // Get all categories
    vector<string> getCategories();
    
    // Create a sprite instance from a sprite in the library
    GifSprite* createSpriteInstance(string id, float x = 0.5, float y = 0.5, float scale = 1.0, float rotation = 0);
    
    // Scan sprites directory
    void scanDirectory();
    
    // Create a new category
    bool createCategory(string name);
    
    // Move sprite to a different category
    bool moveSprite(string id, string newCategory);
    
    // Analyze a sprite to get its properties
    bool analyzeSprite(string id);
    
private:
    // Base directory for sprites
    string baseDirectory;
    
    // Default categories
    vector<string> defaultCategories;
    
    // Library storage
    map<string, SpriteInfo*> sprites;
    map<string, vector<SpriteInfo*>> categories;
    
    // Initialize library directory structure
    void initializeDirectories();
    
    // Generate unique ID
    string generateSpriteId(string category, string filename);
    
    // Get file extension
    string getFileExtension(string filename);
    
    // Load GIF and extract frame information
    bool loadGifInfo(string path, SpriteInfo* info);
    
    // Create thumbnail for a sprite
    string createThumbnail(string sourcePath, string category, string filename);
};