//
//  Source.cpp
//  Motion Semantics
//
//  Created by Heetae Kim on 4/14/15.
//  Copyright (c) 2015 Heetae Kim. All rights reserved.
//
#include <iostream>
#include <chrono>

#include "Database.h"

using namespace std;

void test_db()
{
	Database db;

	cout << "Start reading..." << endl;

	auto start_time = chrono::high_resolution_clock::now();
	bool b = db.read();
	auto end_time = chrono::high_resolution_clock::now();

	if (b) {
		cout << "Reading has been done in "
			<< chrono::duration_cast<chrono::seconds>(end_time - start_time).count()
			<< "."
			<< chrono::duration_cast<chrono::microseconds>(end_time - start_time).count()
			<< " sec." << endl;
	}
	else {
		printf("Failed to read.");
	}

	cout << "Start indexing..." << endl;

	start_time = chrono::high_resolution_clock::now();
	b = db.indexMotion();
	end_time = chrono::high_resolution_clock::now();

	if (b) {
		cout << "Indexing has been done in "
			<< chrono::duration_cast<chrono::seconds>(end_time - start_time).count()
			<< "."
			<< chrono::duration_cast<chrono::microseconds>(end_time - start_time).count()
			<< " sec." << endl;
	}
	else {
		printf("Failed to index.");
	}

	Mat mat = db.videoAt(0).frameAt(3).data();
//	Mat mat = db.videoAt(1).frameAt(456).data();

	Video v(352, 288, 600, Database::MOVIE);
	v.read();
	v.readUntilExist();
	db.makeTree(v);
	printf("dfddfddfd");
	namedWindow("Unprocessed Image", WINDOW_AUTOSIZE);
	imshow("Unprocessed Image", mat);
	waitKey();
}

void test_video()
{
	Video v(352, 288,Database::STARCRAFT);
	v.readUntilExist();

	cout << v.size();

	Mat m = v.frameAt(599).data();
	namedWindow("Unprocessed Image", WINDOW_AUTOSIZE);
	imshow("Unprocessed Image", m);
	waitKey();
}

void test_image()
{
	Image img(352, 288, "C:/database/starcraft/starcraft600.rgb");
	img.read();
	img.readGrayscale();

	Mat m = img.grayscale();
	printf("%s", img.getHistogram(65));
	namedWindow("Unprocessed Image", WINDOW_AUTOSIZE);
	imshow("Unprocessed Image", img.data());
	
	waitKey();
}

int main(int argc, const char * argv[])
{
	//test_image();
	test_db();
	return 0;
}
