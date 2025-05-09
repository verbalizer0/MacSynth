// File: src/Utils/SpriteLibrary.cpp
#include "SpriteLibrary.h"

SpriteLibrary::SpriteLibrary() {
    // Set base directory for sprites
    baseDirectory = ofToDataPath("Sprites", true);
    
    // Default categories
    defaultCategories = {
        "abstract",
        "geometric",
        "people",
        "animals",
        "nature",
        "patterns",
        "effects",
        "symbols",
        "custom"
    };
}

SpriteLibrary::~SpriteLibrary() {
    // Clean up resources
    for (auto& sprite : sprites) {
        delete sprite.second;
    }
    sprites.clear();
    categories.clear();
}

void SpriteLibrary::setup() {
    // Initialize directory structure
    initializeDirectories();
    
    // Scan for sprites
    scanDirectory();
    
    ofLogNotice("SpriteLibrary") << "Loaded " << sprites.size() << " sprites in " 
                                << categories.size() << " categories";
}

void SpriteLibrary::initializeDirectories() {
    // Create base directory if it doesn't exist
    if (!ofDirectory::doesDirectoryExist(baseDirectory)) {
        ofDirectory::createDirectory(baseDirectory);
        ofLogNotice("SpriteLibrary") << "Created sprite base directory: " << baseDirectory;
    }
    
    // Create category subdirectories
    for (const auto& category : defaultCategories) {
        string categoryPath = baseDirectory + "/" + category;
        if (!ofDirectory::doesDirectoryExist(categoryPath)) {
            ofDirectory::createDirectory(categoryPath);
            ofLogNotice("SpriteLibrary") << "Created category directory: " << category;
        }
    }
    
    // Create thumbnails directory
    string thumbnailsDir = baseDirectory + "/thumbnails";
    if (!ofDirectory::doesDirectoryExist(thumbnailsDir)) {
        ofDirectory::createDirectory(thumbnailsDir);
    }
}

void SpriteLibrary::scanDirectory() {
    // Clear existing sprites
    for (auto& sprite : sprites) {
        delete sprite.second;
    }
    sprites.clear();
    categories.clear();
    
    // Initialize categories map
    for (const auto& category : defaultCategories) {
        categories[category] = vector<SpriteInfo*>();
    }
    
    // Scan each category directory
    for (const auto& category : defaultCategories) {
        string categoryPath = baseDirectory + "/" + category;
        ofDirectory dir(categoryPath);
        
        // List only GIF files
        dir.allowExt("gif");
        dir.listDir();
        
        ofLogNotice("SpriteLibrary") << "Scanning category: " << category 
                                   << " (" << dir.size() << " files)";
        
        // Process each file
        for (int i = 0; i < dir.size(); i++) {
            string path = dir.getPath(i);
            string filename = dir.getName(i);
            
            // Generate sprite ID
            string id = generateSpriteId(category, filename);
            
            // Create sprite info
            SpriteInfo* info = new SpriteInfo();
            info->id = id;
            info->name = filename.substr(0, filename.find_last_of("."));
            info->path = path;
            info->category = category;
            info->thumbnailPath = path; // Use original for now, thumbnails would be generated later
            info->analyzed = false;
            info->frameCount = 1;
            
            // Add to library
            sprites[id] = info;
            categories[category].push_back(info);
            
            // Log
            ofLogVerbose("SpriteLibrary") << "Added sprite: " << filename;
        }
    }
}

bool SpriteLibrary::addSprite(string path, string category, string name) {
    // Validate category
    if (find(defaultCategories.begin(), defaultCategories.end(), category) == defaultCategories.end()) {
        category = "custom";
    }
    
    // Get filename from path
    string filename = ofFilePath::getFileName(path);
    
    // Check if file is a GIF
    if (getFileExtension(filename) != "gif") {
        ofLogError("SpriteLibrary") << "File is not a GIF: " << filename;
        return false;
    }
    
    // Generate a custom name if not provided
    if (name.empty()) {
        name = filename.substr(0, filename.find_last_of("."));
    }
    
    // Generate unique filename to avoid collisions
    string uniqueFilename = name + "_" + ofToString(ofGetUnixTime()) + ".gif";
    
    // Create destination path
    string destPath = baseDirectory + "/" + category + "/" + uniqueFilename;
    
    // Copy file to library
    bool success = ofFile::copyFromTo(path, destPath);
    
    if (success) {
        // Generate ID
        string id = generateSpriteId(category, uniqueFilename);
        
        // Create sprite info
        SpriteInfo* info = new SpriteInfo();
        info->id = id;
        info->name = name;
        info->path = destPath;
        info->category = category;
        info->thumbnailPath = destPath; // Generate thumbnail later
        info->analyzed = false;
        info->frameCount = 1;
        
        // Add to library
        sprites[id] = info;
        categories[category].push_back(info);
        
        // Analyze sprite
        analyzeSprite(id);
        
        ofLogNotice("SpriteLibrary") << "Added sprite: " << name << " to " << category;
        return true;
    } else {
        ofLogError("SpriteLibrary") << "Failed to copy file: " << path << " to " << destPath;
        return false;
    }
}

bool SpriteLibrary::addSpriteFromURL(string url, string category, string name) {
    // In a real implementation, this would download the GIF from the URL
    // For this example, we'll simulate it
    
    ofLogNotice("SpriteLibrary") << "Downloading GIF from URL: " << url;
    
    // Extract filename from URL
    string filename = url.substr(url.find_last_of("/") + 1);
    
    // Remove any URL parameters
    if (filename.find("?") != string::npos) {
        filename = filename.substr(0, filename.find("?"));
    }
    
    // Check if file is a GIF
    if (getFileExtension(filename) != "gif") {
        ofLogError("SpriteLibrary") << "URL does not point to a GIF: " << url;
        return false;
    }
    
    // Generate a custom name if not provided
    if (name.empty()) {
        name = filename.substr(0, filename.find_last_of("."));
    }
    
    // Simulate downloading
    ofLogNotice("SpriteLibrary") << "Downloaded GIF: " << filename;
    
    // In a real implementation, we would download the file and then call addSprite()
    // For this example, we'll create a dummy sprite
    
    // Generate unique filename
    string uniqueFilename = name + "_" + ofToString(ofGetUnixTime()) + ".gif";
    
    // Create destination path
    string destPath = baseDirectory + "/" + category + "/" + uniqueFilename;
    
    // Create sprite info
    string id = generateSpriteId(category, uniqueFilename);
    
    SpriteInfo* info = new SpriteInfo();
    info->id = id;
    info->name = name;
    info->path = destPath;
    info->category = category;
    info->thumbnailPath = destPath;
    info->analyzed = false;
    info->frameCount = 4; // Simulate animated GIF
    info->width = 100;
    info->height = 100;
    
    // Add to library
    sprites[id] = info;
    categories[category].push_back(info);
    
    ofLogNotice("SpriteLibrary") << "Added sprite from URL: " << name << " to " << category;
    return true;
}

bool SpriteLibrary::removeSprite(string id) {
    // Find sprite
    auto it = sprites.find(id);
    if (it == sprites.end()) {
        ofLogError("SpriteLibrary") << "Sprite not found: " << id;
        return false;
    }
    
    SpriteInfo* info = it->second;
    
    // Remove from filesystem
    ofFile file(info->path);
    if (file.exists()) {
        file.remove();
    }
    
    // Remove thumbnail if it exists
    if (info->thumbnailPath != info->path) {
        ofFile thumbnail(info->thumbnailPath);
        if (thumbnail.exists()) {
            thumbnail.remove();
        }
    }
    
    // Remove from category
    auto& categorySprites = categories[info->category];
    categorySprites.erase(remove_if(categorySprites.begin(), categorySprites.end(),
        [id](SpriteInfo* s) { return s->id == id; }), categorySprites.end());
    
    // Remove from sprites map
    sprites.erase(it);
    
    // Delete sprite info
    delete info;
    
    ofLogNotice("SpriteLibrary") << "Removed sprite: " << id;
    return true;
}

SpriteInfo* SpriteLibrary::getSpriteById(string id) {
    auto it = sprites.find(id);
    if (it != sprites.end()) {
        return it->second;
    }
    return nullptr;
}

vector<SpriteInfo*> SpriteLibrary::getSpritesByCategory(string category) {
    auto it = categories.find(category);
    if (it != categories.end()) {
        return it->second;
    }
    return vector<SpriteInfo*>();
}

vector<SpriteInfo*> SpriteLibrary::getAllSprites() {
    vector<SpriteInfo*> result;
    for (auto& sprite : sprites) {
        result.push_back(sprite.second);
    }
    return result;
}

vector<string> SpriteLibrary::getCategories() {
    vector<string> result;
    for (auto& category : categories) {
        result.push_back(category.first);
    }
    return result;
}

GifSprite* SpriteLibrary::createSpriteInstance(string id, float x, float y, float scale, float rotation) {
    SpriteInfo* info = getSpriteById(id);
    if (!info) {
        ofLogError("SpriteLibrary") << "Sprite not found: " << id;
        return nullptr;
    }
    
    // Create GIF sprite
    GifSprite* sprite = new GifSprite();
    sprite->setup(info->path, x, y, scale, rotation);
    
    return sprite;
}

bool SpriteLibrary::createCategory(string name) {
    // Check if category already exists
    if (find(defaultCategories.begin(), defaultCategories.end(), name) != defaultCategories.end()) {
        ofLogWarning("SpriteLibrary") << "Category already exists: " << name;
        return false;
    }
    
    // Create directory
    string categoryPath = baseDirectory + "/" + name;
    bool success = ofDirectory::createDirectory(categoryPath);
    
    if (success) {
        // Add to categories
        defaultCategories.push_back(name);
        categories[name] = vector<SpriteInfo*>();
        
        ofLogNotice("SpriteLibrary") << "Created category: " << name;
        return true;
    } else {
        ofLogError("SpriteLibrary") << "Failed to create category directory: " << name;
        return false;
    }
}

// Fixed moveSprite method for SpriteLibrary.cpp
bool SpriteLibrary::moveSprite(string id, string newCategory) {
    // Find sprite
    SpriteInfo* info = getSpriteById(id);
    if (!info) {
        ofLogError("SpriteLibrary") << "Sprite not found: " << id;
        return false;
    }
    
    // Check if category exists
    if (find(defaultCategories.begin(), defaultCategories.end(), newCategory) == defaultCategories.end()) {
        ofLogError("SpriteLibrary") << "Category not found: " << newCategory;
        return false;
    }
    
    // Get old category
    string oldCategory = info->category;
    
    // Create new path
    string filename = ofFilePath::getFileName(info->path);
    string newPath = baseDirectory + "/" + newCategory + "/" + filename;
    
    // Move file
    bool success = ofFile::moveFromTo(info->path, newPath);
    
    if (success) {
        // Update sprite info
        info->path = newPath;
        info->category = newCategory;
        
        // Move thumbnail if it exists
        if (info->thumbnailPath != info->path) {
            string thumbnailFilename = ofFilePath::getFileName(info->thumbnailPath);
            string newThumbnailPath = baseDirectory + "/thumbnails/" + newCategory + "/" + thumbnailFilename;
            
            // Create a file object to check existence
            ofFile thumbnailFile(info->thumbnailPath);
            if (thumbnailFile.exists()) {
                ofFile::moveFromTo(info->thumbnailPath, newThumbnailPath);
                info->thumbnailPath = newThumbnailPath;
            }
        }
        
        // Remove from old category
        auto& oldCategorySprites = categories[oldCategory];
        oldCategorySprites.erase(remove_if(oldCategorySprites.begin(), oldCategorySprites.end(),
            [id](SpriteInfo* s) { return s->id == id; }), oldCategorySprites.end());
        
        // Add to new category
        categories[newCategory].push_back(info);
        
        ofLogNotice("SpriteLibrary") << "Moved sprite " << info->name 
                                   << " from " << oldCategory 
                                   << " to " << newCategory;
        return true;
    } else {
        ofLogError("SpriteLibrary") << "Failed to move sprite file from " 
                                  << info->path << " to " << newPath;
        return false;
    }
}

bool SpriteLibrary::analyzeSprite(string id) {
    SpriteInfo* info = getSpriteById(id);
    if (!info) {
        ofLogError("SpriteLibrary") << "Sprite not found: " << id;
        return false;
    }
    
    // Load GIF and extract frame information
    bool success = loadGifInfo(info->path, info);
    
    if (success) {
        info->analyzed = true;
        ofLogNotice("SpriteLibrary") << "Analyzed sprite: " << info->name 
                                   << " (" << info->frameCount << " frames)";
        return true;
    } else {
        ofLogError("SpriteLibrary") << "Failed to analyze sprite: " << info->name;
        return false;
    }
}

string SpriteLibrary::generateSpriteId(string category, string filename) {
    string baseName = filename.substr(0, filename.find_last_of("."));
    return category + "_" + baseName;
}

string SpriteLibrary::getFileExtension(string filename) {
    size_t pos = filename.find_last_of(".");
    if (pos != string::npos) {
        return ofToLower(filename.substr(pos + 1));
    }
    return "";
}

bool SpriteLibrary::loadGifInfo(string path, SpriteInfo* info) {
    // In a real implementation, this would use ofxGif or similar addon
    // to load the GIF and extract frame information
    
    // For this example, we'll simulate some basic information
    
    // Check if file exists
    if (!ofFile::doesFileExist(path)) {
        ofLogError("SpriteLibrary") << "File not found: " << path;
        return false;
    }
    
    // Load the image to get dimensions
    ofImage img;
    bool loaded = img.load(path);
    
    if (loaded) {
        // Set dimensions
        info->width = img.getWidth();
        info->height = img.getHeight();
        
        // Simulate frame detection for animated GIFs
        // In a real implementation, this would extract actual frame information
        info->frameCount = (ofRandom(0, 1) > 0.5) ? 1 : (int)ofRandom(2, 10);
        
        // Generate frame durations (100ms per frame typical for GIFs)
        info->frameDurations.clear();
        for (int i = 0; i < info->frameCount; i++) {
            info->frameDurations.push_back(0.1);
        }
        
        return true;
    } else {
        ofLogError("SpriteLibrary") << "Failed to load image: " << path;
        return false;
    }
}

string SpriteLibrary::createThumbnail(string sourcePath, string category, string filename) {
    // In a real implementation, this would create a thumbnail version of the sprite
    
    // Create thumbnails directory for category if it doesn't exist
    string thumbnailsDir = baseDirectory + "/thumbnails/" + category;
    if (!ofDirectory::doesDirectoryExist(thumbnailsDir)) {
        ofDirectory::createDirectory(thumbnailsDir);
    }
    
    // Create thumbnail filename
    string thumbnailPath = thumbnailsDir + "/" + filename;
    
    // Load source image
    ofImage sourceImg;
    if (sourceImg.load(sourcePath)) {
        // Resize to thumbnail size (e.g., 100x100)
        ofImage thumbnailImg = sourceImg;
        thumbnailImg.resize(100, 100);
        
        // Save thumbnail
        thumbnailImg.save(thumbnailPath);
        
        return thumbnailPath;
    } else {
        ofLogError("SpriteLibrary") << "Failed to create thumbnail for: " << sourcePath;
        return sourcePath; // Return original path as fallback
    }
}