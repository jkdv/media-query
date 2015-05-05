//
//  Database.h
//  Motion Semantics
//
//  Created by Heetae Kim on 4/18/15.
//  Copyright (c) 2015 Heetae Kim. All rights reserved.
//

#ifndef __fuzzy_tribble__Database__
#define __fuzzy_tribble__Database__

#include "Video.h"
#include "Sound.h"
#include "Tree.h"
#include <map>

class Database
{
public:
    static const string m_PREFIX;
    static const string FLOWERS;
    static const string INTERVIEW;
    static const string MOVIE;
    static const string MUSICVIDEO;
    static const string SPORTS;
    static const string STARCRAFT;
    static const string TRAFFIC;
    static const int WIDTH = 352;
    static const int HEIGHT = 288;
    static const int FRAME_SIZE = 600;
   
private:
    vector<string> m_kinds;
    map<string, Video> m_videos;
    map<string, vector<Point2d>> m_motions;
	Tree m_tree;		// Tree storing all the calculated values
	map<string, double*> m_valueSum;
	map<string, string> m_nicknames;
	//map<string, double*> image_DB;
    
public:
	 
    Database();
    ~Database();
    Database(const Database& rhs);
    
    Database& operator=(const Database& rhs);
	const Tree& getTree() const { return m_tree; }
	string convertToNickname(const string& path) const {
		return m_nicknames.at(path); 
	}
	string convertToFullname(const string& nickname) const {
		return m_PREFIX + "/" + nickname + "/" + nickname;
	}
    
    bool read();
    bool indexMotion();
	static vector<Point2d> calcMotionVector(Video& video);
    bool empty() const { return m_videos.empty(); }
    Video& videoAt(const string& kind) { return m_videos[kind]; }
    Video& videoAt(int n) { return m_videos[m_kinds[n]]; }
	
	/* methods for comparing 2 properties */
	/* compare two histogram values --> return double 2-D array */
	map<string, double*> compareColorWithInput(Video video);
	map<string, double*> compareColorWithInput2(Video video);
	/* compare motion vectors */
	map<string, double*> compareMoVectorWithInput(Video video);
	map<string, double*> compareMoVectorWithInput2(Video video);

	map<string, double*> compareSoundWithInput(Sound sound);

	double compareSound(const cv::Vec2f& sound1, const cv::Vec2f& sound2) const{
		return abs(sound2[0] - sound1[0]) + abs(sound2[1] - sound1[1]);
	}
	
	/* Put values in a tree*/
	void makeTree(Video video);
	void makeTree2(Video video);

	map<string, double*> getHistogramValues(){ return m_valueSum; }

};

#endif /* defined(__fuzzy_tribble__Database__) */
