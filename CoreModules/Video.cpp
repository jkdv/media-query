//
//  Video.cpp
//  Motion Semantics
//
//  Created by Heetae Kim on 4/18/15.
//  Copyright (c) 2015 Heetae Kim. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <iomanip>

#include "Video.h"

Video::Video()
{
    m_width = -1;
    m_height = -1;
    m_frameSize = -1;
}

Video::Video(int w, int h, const string& path)
{
    m_path = path;
	m_audioPath = path + ".wav";
    m_width = w;
    m_height = h;
    m_frameSize = -1;
}

Video::Video(int w, int h, int frameSize, const string& path)
{
	m_path = path;
	m_audioPath = path + ".wav";
    m_width = w;
    m_height = h;
    m_frameSize = frameSize;
}

Video::Video(const Video& rhs)
{
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    m_path = rhs.m_path;
	m_audioPath = rhs.m_audioPath;
    m_images = rhs.m_images;
    m_frameSize = rhs.m_frameSize;
	m_sound = rhs.m_sound;
}

Video::~Video()
{
}

Video& Video::operator=(const Video& rhs)
{
    m_width = rhs.m_width;
    m_height = rhs.m_height;
	m_path = rhs.m_path;
	m_audioPath = rhs.m_audioPath;
    m_images = rhs.m_images;
    m_frameSize = rhs.m_frameSize;
	m_sound = rhs.m_sound;
    return *this;
}

bool Video::read()
{
    if (m_width < 0 || m_height < 0 || m_frameSize < 0)
        return false;

    m_images.clear();
    for (int i = 0; i < m_frameSize; i++) {
        ostringstream postfix;
        postfix << m_path << setfill('0') << setw(3) << i + 1 << ".rgb";

		FILE *file;
		if (fopen_s(&file, postfix.str().c_str(), "rb") == 0) {
			fclose(file);
		}
		else {
			postfix = ostringstream();
			postfix << m_path << "_" << setfill('0') << setw(3) << i + 1 << ".rgb";

			if (fopen_s(&file, postfix.str().c_str(), "rb") == 0) {
				fclose(file);
			}
			else {
				return false;
			}
		}

        m_images.push_back(Image(m_width, m_height, postfix.str()));

        if (!m_images.back().read()) {
			return false;
        }
    }

	m_frameSize = (int)m_images.size();

	//sound object
	m_sound.setFileName(m_path);
	m_sound.setNumFrames(m_frameSize);
	m_sound.Initialize();

    return true;
}

bool Video::readUntilExist()
{
    if (m_width < 0 || m_height < 0)
        return false;
    
    m_images.clear();
    m_frameSize = 0;
    
    while (true) {
        ostringstream postfix;
        postfix << m_path
                << setfill('0') << setw(3) << m_frameSize + 1 << ".rgb";
        
		FILE *file;
        if (fopen_s(&file, postfix.str().c_str(), "rb") == 0)
            fclose(file);
		else {
			postfix = ostringstream();
			postfix << m_path << "_" << setfill('0') << setw(3) << m_frameSize + 1 << ".rgb";

			if (fopen_s(&file, postfix.str().c_str(), "rb") == 0) {
				fclose(file);
			}
			else {
				if (m_frameSize == 0)
					return false;
				break;
			}
		}
        
        m_images.push_back(Image(m_width, m_height, postfix.str()));
        if (!m_images.back().read()) {
            return false;
        }
        ++m_frameSize;
    }

	m_frameSize = (int)m_images.size();

	//sound object
	m_sound.setFileName(m_path);
	m_sound.setNumFrames(m_frameSize);
	m_sound.Initialize();
    
    return true;
}
