//
//  Video.h
//  Motion Semantics
//
//  Created by Heetae Kim on 4/18/15.
//  Copyright (c) 2015 Heetae Kim. All rights reserved.
//

#ifndef __fuzzy_tribble__Video__
#define __fuzzy_tribble__Video__

#include "Image.h"
#include <vector>
#include "Sound.h"

using namespace std;

class Video {
private:
    int m_frameSize;    // Number of frames
    int	m_width;		// Width of Image
    int	m_height;		// Height of Image
    string m_path;
	string m_audioPath;
    vector<Image> m_images;
	Sound m_sound;
    
public:
    Video();
    Video(int w, int h, const string& path);
    Video(int w, int h, int frameSize, const string& path);
    Video(const Video& rhs);
    ~Video();
    
    Video& operator=(const Video& rhs);
    
    bool empty() const { return m_images.empty(); }
    int size() const { return m_frameSize; }
	string getAutioFilePath() const { return m_audioPath; }
    Image& frameAt(int n) { return m_images[n]; }
	Sound& getSound() { printf("getsound!!"); return m_sound; }
    
    bool read();
    bool readUntilExist();
};

#endif /* defined(__fuzzy_tribble__Video__) */
