//
//  Database.cpp
//  Motion Semantics
//
//  Created by Heetae Kim on 4/18/15.
//  Copyright (c) 2015 Heetae Kim. All rights reserved.
//

#include "Database.h"
#include <iostream>
#include <thread>
#include <omp.h>

//#include <codecogs/statistics/moments/correlation.h>
const string Database::m_PREFIX     = "C:/database";
const string Database::FLOWERS      = m_PREFIX + "/flowers/flowers";
const string Database::INTERVIEW    = m_PREFIX + "/interview/interview";
const string Database::MOVIE        = m_PREFIX + "/movie/movie";
const string Database::MUSICVIDEO   = m_PREFIX + "/musicvideo/musicvideo";
const string Database::SPORTS       = m_PREFIX + "/sports/sports";
const string Database::STARCRAFT    = m_PREFIX + "/starcraft/starcraft";
const string Database::TRAFFIC      = m_PREFIX + "/traffic/traffic";

Database::Database()
{
    m_kinds = {
        FLOWERS, INTERVIEW, MOVIE, MUSICVIDEO,
        SPORTS, STARCRAFT, TRAFFIC
    };
	m_nicknames[FLOWERS] = "flowers";
	m_nicknames[INTERVIEW] = "interview";
	m_nicknames[MOVIE] = "movie";
	m_nicknames[MUSICVIDEO] = "musicvideo";
	m_nicknames[SPORTS] = "sports";
	m_nicknames[STARCRAFT] = "starcraft";
	m_nicknames[TRAFFIC] = "traffic";
}

Database::~Database()
{
}

Database::Database(const Database& rhs)
{
    m_kinds = rhs.m_kinds;
    m_videos = rhs.m_videos;
}

Database& Database::operator=(const Database& rhs)
{
    if (this != &rhs) {
        m_kinds = rhs.m_kinds;
        m_videos = rhs.m_videos;
    }
    return *this;
}

bool Database::read()
{
	// Initialize video vector space
    m_videos.clear();
	for (string& kind : m_kinds) {
		m_videos.insert({
			kind, Video(WIDTH, HEIGHT, FRAME_SIZE, kind)
		});
	}

	// Read video files using OpenMP
	#pragma omp parallel default(none)
	{
		#pragma omp for
		for (int i = 0; i < m_kinds.size(); ++i) {
			String kind = m_kinds[i];
			Video* pVideo = &m_videos[kind];
			pVideo->read();
		}
	}

	// See if it run correctly
    for (string& s : m_kinds) {
        if (m_videos[s].empty())
            return false;
    }
    
    return true;
}

bool Database::indexMotion()
{
	// Initialize motion vector space
	m_motions.clear();
	for (string& kind : m_kinds) {
		Video* pVideo = &m_videos[kind];
		m_motions[kind] = calcMotionVector(*pVideo);
	}

	// See if it run correctly
    for (string& s : m_kinds) {
        if (m_motions[s].empty()) {
            cerr << "Fail to index motion vectors" << endl;
            return false;
		}
    }
    
    return true;
}

vector<Point2d> Database::calcMotionVector(Video& video)
{
	Video* pVideo = &video;
	Image* prev = nullptr;
	Image* curr = nullptr;
	Point2d p;

	int i, id, num_threads, first_row, last_row, N = pVideo->size();
	vector<Point2d> motion(N - 1);

	#pragma omp parallel default(none) shared(i, pVideo, N)
	{
		#pragma omp for
		for (i = 0; i < N; ++i) {
			pVideo->frameAt(i).readGrayscale();
		}
	}

	#pragma omp parallel default(none) \
	shared(pVideo, prev, curr, motion, N) \
	private(i, p, id, num_threads, first_row, last_row)
	{
		id = omp_get_thread_num();
		num_threads = omp_get_num_threads();
		first_row = id * (N / num_threads);
		last_row = first_row + (N / num_threads);
		if (id == num_threads - 1)
			last_row = N;
		
		prev = &pVideo->frameAt(first_row);

		for (i = first_row + 1; i < last_row; ++i) {
			curr = &pVideo->frameAt(i);

			p = phaseCorrelate(
				prev->grayscale(),
				curr->grayscale());
			memcpy(&motion[i - 1], &p, sizeof(Point2d));

			prev = curr;
		}
	}

	#pragma omp parallel default(none) shared(i, pVideo, N)
	{
	#pragma omp for
		for (i = 0; i < N; ++i) {
			pVideo->frameAt(i).releaseGrayscale();
		}
	}

	return motion;
}
map<string, double*> Database::compareColorWithInput(Video video){
	int inputSize = 10;//video.size();	//Frame Size of the video

	/* all of the histogram value sets of all database videos */
	map<string, double*> databaseFrame;
	//double *image_hist;
	//int kindIdx = 0;
	for (string& kind : m_kinds) {
		// for each database image
		int cnt = 0;
		databaseFrame[kind] = new double[FRAME_SIZE];
		memset(databaseFrame[kind], 0, sizeof(double) * FRAME_SIZE);
		while ((cnt + 150) <= FRAME_SIZE){
			printf("%d ", cnt);
			double *each_histo = new double[inputSize];
			int idx;
//			int i;
			for (idx=0; idx < inputSize; idx++){
				//for each image groups in a video
				//for each frame in the input video
				
				Image input = video.frameAt(idx);
				Image db_img = m_videos[kind].frameAt(idx+cnt);	// each frame in database
				//printf("%d-1 ", idx);
				MatND hist1 = db_img.getHistogram(2);
				MatND hist2 = input.getHistogram(2);
				each_histo[idx] = cv::compareHist(db_img.getHistogram(2), input.getHistogram(2), CV_COMP_CORREL);
				//printf("%d-2 \n", idx);
			}
			/* Get average for the image group */
			double sum = 0.0;
			for (int i = 0; i < inputSize; i++){
				sum += each_histo[i];
			}
			//image_hist[cnt] = sum / inputSize;
			databaseFrame[kind][cnt] = sum / inputSize;
			cnt+=1;
			delete[] each_histo;
		}
		//databaseFrame[kind] = image_hist;
		//kindIdx++;
	}
	//delete[] image_hist;

	return databaseFrame;
}

map<string, double*> Database::compareColorWithInput2(Video video){
	int inputSize = 10;//video.size();	//Frame Size of the video

	/* all of the histogram value sets of all database videos */
	map<string, double*> databaseFrame;
	//double *image_hist;
	//int kindIdx = 0;
	for (string& kind : m_kinds) {
		// for each database image
		int cnt = 0;
		databaseFrame[kind] = new double[FRAME_SIZE];
		memset(databaseFrame[kind], 0, sizeof(double) * FRAME_SIZE);
		while ((cnt + 150) <= FRAME_SIZE){
			printf("%d ", cnt);
			double *each_histo = new double[inputSize];
			int idx;
			//			int i;

			auto* pVideos = &m_videos;

#pragma omp parallel default(none) shared(idx, inputSize, each_histo, pVideos, video, kind, cnt)
			{
#pragma omp for
				for (idx = 0; idx < inputSize; idx++){
					//for each image groups in a video
					//for each frame in the input video

					Image input = video.frameAt(idx);
					Image db_img = (*pVideos)[kind].frameAt(idx + cnt);	// each frame in database
					//printf("%d-1 ", idx);
					MatND hist1 = db_img.getHistogram(2);
					MatND hist2 = input.getHistogram(2);
					each_histo[idx] = cv::compareHist(db_img.getHistogram(2), input.getHistogram(2), CV_COMP_CORREL);
					//printf("%d-2 \n", idx);
				}
			}



			/* Get average for the image group */
			double sum = 0.0;
			for (int i = 0; i < inputSize; i++){
				sum += each_histo[i];
			}

			//image_hist[cnt] = sum / inputSize;
			databaseFrame[kind][cnt] = sum / inputSize;
			cnt += 1;
			delete[] each_histo;
		}
		//databaseFrame[kind] = image_hist;
		//kindIdx++;
	}
	//delete[] image_hist;

	return databaseFrame;
}

map<string, double*> Database::compareMoVectorWithInput(Video video){
	int inputSize = 10;//video.size();
	int interval = 30;
//	double *each_vector;
	//double *video_vect;
	map<string, double*> databaseFrame;
	vector<Point2d> motion1group = (this->calcMotionVector(video));	// calculate motion vector for the input video
	int index = 0;
	Point2d motion1;
	Point2d motion2;
	for (string& kind : m_kinds) {
		// for each database image
		int cnt = 0;
		databaseFrame[kind] = new double[FRAME_SIZE];
		memset(databaseFrame[kind], 0, sizeof(double) * FRAME_SIZE);
		while ((cnt + 150) <= FRAME_SIZE-1){
			//printf("%d ", cnt);
			double *each_histo = new double[inputSize];
			int idx=0;
//			int i;
			for (idx = 0; idx < inputSize; idx++){
				//for each image groups in a video
				//for each frame in the input video
				motion1 = motion1group[idx];
				motion2 = m_motions[kind][idx + cnt];

				//Image input = video.frameAt(idx);
				//Image db_img = m_videos[kind].frameAt(idx + cnt);	// each frame in database
				//printf("%d-1 ", idx);
				//MatND hist1 = db_img.getHistogram(2);
				//MatND hist2 = input.getHistogram(2);
				//each_histo[idx] = cv::compareHist(db_img.getHistogram(2), input.getHistogram(2), CV_COMP_CORREL);
				each_histo[idx] = sqrt(pow(motion2.x - motion1.x, 2) + pow(motion2.y - motion1.y, 2));
				//&motion1 = NULL;
				motion1.x = 0.0;
				motion1.y = 0.0;
				motion2.x = 0.0;
				motion2.y = 0.0;
				//motion2 = 0.0;
				//printf("%d-2 \n", idx);
			}
			/* Get average for the image group */
			double sum = 0.0;
			for (int i = 0; i < inputSize; i++){
				sum += each_histo[i];
			}
			//image_hist[cnt] = sum / inputSize;
			databaseFrame[kind][cnt] = sum / (double) inputSize;
			printf("%f\n", databaseFrame[kind][cnt]);
			cnt+=1;
			delete[] each_histo;
		}
		//databaseFrame[kind] = image_hist;
		//kindIdx++;
	}
	/* Normalize each distance */

	for (string& kind : m_kinds) {
		double sum = 0.0;
		double vsum = 0.0;
		for (int idx = 0; idx < 450 - 1; idx++){
			//printf("%f\n", databaseFrame[kind][idx]);
			//sum += databaseFrame[kind][idx];
			vsum += (databaseFrame[kind][idx] * databaseFrame[kind][idx]); //pow(databaseFrame[kind][idx], 2);
		}
		//double avg = sum / (inputSize - 1);
		//double v_avg = vsum / (inputSize - 1);
		double div = sqrt(vsum);
		//put normalized values
		for (int idx = 0; idx < 450; idx++){
			databaseFrame[kind][idx] /= div;
			printf("value: %f, ", databaseFrame[kind][idx]);
		}
	}
	//delete[] video_vect;
	return databaseFrame;
}


map<string, double*> Database::compareMoVectorWithInput2(Video video){
	int inputSize = 10;//video.size();
	int interval = 30;
	//	double *each_vector;
	//double *video_vect;
	map<string, double*> databaseFrame;
	vector<Point2d> motion1group = (this->calcMotionVector(video));	// calculate motion vector for the input video
	int index = 0;
	for (string& kind : m_kinds) {
		// for each database image
		int cnt = 0;
		databaseFrame[kind] = new double[FRAME_SIZE];
		memset(databaseFrame[kind], 0, sizeof(double) * FRAME_SIZE);
		while ((cnt + 150) <= FRAME_SIZE - 1){
			//printf("%d ", cnt);
			double *each_histo = new double[inputSize];
			int idx = 0;
			//			int i;



			auto* pMotions = &m_motions;

#pragma omp parallel default(none) shared(idx, inputSize, each_histo, pMotions, video, kind, cnt, motion1group)
			{
#pragma omp for
				for (idx = 0; idx < inputSize; idx++){
					//for each image groups in a video
					//for each frame in the input video
					Point2d motion1 = motion1group[idx];
					Point2d motion2 = (*pMotions)[kind][idx + cnt];

					each_histo[idx] = sqrt(pow(motion2.x - motion1.x, 2) + pow(motion2.y - motion1.y, 2));
				}
			}


			/* Get average for the image group */
			double sum = 0.0;
			for (int i = 0; i < inputSize; i++){
				sum += each_histo[i];
			}
			//image_hist[cnt] = sum / inputSize;
			databaseFrame[kind][cnt] = sum / (double)inputSize;
			cnt += 1;
			delete[] each_histo;
		}
		//databaseFrame[kind] = image_hist;
		//kindIdx++;
	}
	/* Normalize each distance */

	for (string& kind : m_kinds) {
		double sum = 0.0;
		double vsum = 0.0;
		for (int idx = 0; idx < 450 - 1; idx++){
			//printf("%f\n", databaseFrame[kind][idx]);
			//sum += databaseFrame[kind][idx];
			vsum += (databaseFrame[kind][idx] * databaseFrame[kind][idx]); //pow(databaseFrame[kind][idx], 2);
		}
		//double avg = sum / (inputSize - 1);
		//double v_avg = vsum / (inputSize - 1);
		double div = sqrt(vsum);
		//put normalized values
		for (int idx = 0; idx < 450; idx++){
			databaseFrame[kind][idx] /= div;
		}
	}
	//delete[] video_vect;
	return databaseFrame;
}

/* draw histogram image*/
/*
Mat Database::getHistogramImage(Mat & data){
	int histSize = sizeof(data);
	Mat histImg;
	double maxVal = 0;
	double minVal = 0;
	cv::minMaxLoc(data, &minVal, &maxVal, 0, 0);
	//nbins*90 ==> max point
	int hpt = static_cast<int>(0.9*histSize);
	for (int h = 0; h < histSize; h++){
		float binVal = data.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		cv::line(histImg, cv::Point(h, histSize),
			cv::Point(h, histSize - intensity),
			cv::Scalar::all(0));
	}
	return histImg;
	//====> how to call?? imshow("histogram", h.getHistogramImage(image));
}*/

map<string, double*> Database::compareSoundWithInput(Sound sound){
	int inputSize = sound.getNumFrames();
	map<string, double*> databaseFrame;
	unsigned int* all_sound;
	int *each_sound = new int[inputSize];
	double* diff = new double[FRAME_SIZE - inputSize + 1];
	unsigned int* input_sound = sound.data();

	for (string& kind : m_kinds) {
		databaseFrame[kind] = new double[FRAME_SIZE];

		// for each database sound
		int cnt = 0;
		all_sound = m_videos[kind].getSound().data();

		while ((cnt + inputSize) <= FRAME_SIZE)
		{
			int i;
			int idx;

			for (i = cnt, idx = 0; (i < cnt + inputSize); i++, idx++)
			{
				each_sound[idx] = all_sound[i] - input_sound[idx];
			}

			/* Get average for the each group */
			unsigned int sum = 0;

			for (i = 0; i < inputSize; i++)
			{
				sum += each_sound[i];
			}
			databaseFrame[kind][cnt] = sum / inputSize;

			if (databaseFrame[kind][cnt] < 0)
				databaseFrame[kind][cnt] *= -1;

			printf("sound%d : %f\n", cnt, databaseFrame[kind][cnt]);
			cnt++;
			//delete[] each_sound;
			//for (i = 0; i < inputSize; i++)
			//delete[] each_sound; each_sound[i] = 0;
		}
		//databaseFrame[kind] = diff;
	}
	//delete[] diff;
	/* Normalize each distance */

	for (string& kind : m_kinds) {
		double sum = 0.0;
		double vsum = 0.0;

		for (int idx = 0; idx < 450; idx++){
			//printf("%f\n", databaseFrame[kind][idx]);
			//sum += databaseFrame[kind][idx];
			vsum += (databaseFrame[kind][idx] * databaseFrame[kind][idx]); //pow(databaseFrame[kind][idx], 2);
		}

		//double avg = sum / (inputSize - 1);
		//double v_avg = vsum / (inputSize - 1);
		double div = sqrt(vsum);
		//put normalized values

		for (int idx = 0; idx <= 450; idx++){
			databaseFrame[kind][idx] /= div;
			//printf("sound: %f, ", databaseFrame[kind][idx]);
		}
	}

	delete[] each_sound;

	return databaseFrame;
}


void Database::makeTree(Video video){	// input video
	m_tree.clear();
	//if (&(video.getSound())!=NULL){
	Sound sound = video.getSound();
	//}
	//Sound
	map<string, double*> image_DB = compareColorWithInput(video);
	map<string, double*> motion_DB = compareMoVectorWithInput(video);
	map<string, double*> sound_DB = compareSoundWithInput(sound);
	map<string, double*> tmpSum;

	//#pragma omp parallel default(none) \
	//shared(image_DB, motion_DB, sound_DB, tmpSum)
	//{
		double color_ratio = 0.7;
		double motion_ratio = 0.2;
		double sound_ratio = 0.1;
		
		//#pragma omp for
		for (int idx = 0; idx < m_kinds.size(); ++idx) {
			String kind = m_kinds[idx];
			//int arrSize = (image_DB[kind])
			tmpSum[kind] = new double[600];

			for (int i = 0; i < 450; i++){
				//printf("kind%d : %f\n",idx, motion_DB[kind][i]);
				tmpSum[kind][i] = color_ratio * (1.0 - image_DB[kind][i]) + motion_ratio * motion_DB[kind][i]+ sound_ratio * image_DB[kind][i];
				tmpSum[kind][i] = 1.0 - tmpSum[kind][i];
				m_tree.insert(kind, i, tmpSum[kind][i]);

			//}
			}
		}

	//m_valueSum = tmpSum;
}

void Database::makeTree2(Video video){	// input video
	m_tree.clear();
	Sound sound = video.getSound();

	map<string, double*> image_DB;
	map<string, double*> motion_DB;
	map<string, double*> sound_DB;

	thread thImage([](void* object, void* pResult, void* pData) {
		Database* pDb = (Database*)object;
		map<string, double*>* pMap = (map<string, double*>*)pResult;
		*pMap = pDb->compareColorWithInput2(*(Video*)pData);
	}, this, &image_DB, &video);

	thread thMotion([](void* object, void* pResult, void* pData) {
		Database* pDb = (Database*)object;
		map<string, double*>* pMap = (map<string, double*>*)pResult;
		*pMap = pDb->compareMoVectorWithInput2(*(Video*)pData);
	}, this, &motion_DB, &video);

	thread thSound([](void* object, void* pResult, void* pData) {
		Database* pDb = (Database*)object;
		map<string, double*>* pMap = (map<string, double*>*)pResult;
		*pMap = pDb->compareSoundWithInput(*(Sound*)pData);
	}, this, &sound_DB, &sound);

	thImage.join();
	thMotion.join();
	thSound.join();

	map<string, double*> tmpSum;

	double color_ratio = 0.8;
	double motion_ratio = 0.15;
	double sound_ratio = 0.05;

	for (int idx = 0; idx < m_kinds.size(); ++idx) {
		String kind = m_kinds[idx];
		tmpSum[kind] = new double[600];

		Tree* pTree = &m_tree;

#pragma omp parallel default(none) shared(tmpSum, kind, color_ratio, image_DB, motion_ratio, motion_DB, sound_ratio, pTree)
		{
#pragma omp for
			for (int i = 0; i < 450; i++){
				tmpSum[kind][i] = color_ratio * (1.0 - image_DB[kind][i]) + motion_ratio * motion_DB[kind][i] + sound_ratio * image_DB[kind][i];
				tmpSum[kind][i] = 1.0 - tmpSum[kind][i];
#pragma omp critical
				pTree->insert(kind, i, tmpSum[kind][i]);
			}
		}


	}

}

