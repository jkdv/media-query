#include "Tree.h"
#include <ctime>

Tree::Tree()
{
}

Tree::~Tree()
{
}

Tree::Tree(const Tree& rhs)
{
	m_root = rhs.m_root;
	m_children = rhs.m_children;
}

Tree& Tree::operator=(const Tree& rhs)
{
	m_root = rhs.m_root;
	m_children = rhs.m_children;
	return *this;
}

void Tree::insert(const string& videoName, int nFrame, double dValue)
{
	m_children[videoName][nFrame] = dValue;
	if (m_root[videoName] < dValue)
		m_root[videoName] = dValue;
}

double Tree::getRootAvg(const string& videoName) const
{
	size_t nSize = getChildrenCount(videoName);
	double dSum = 0.0;
	for (int i = 0; i < nSize; ++i) {
		dSum += getChild(videoName, i);
	}
	return dSum / nSize;
}

int Tree::locateMaxChild(const string& videoName)
{
	pair<int, double> p = { -1, 0.0 };
	for (auto child : m_children[videoName]) {
		if (child.second > p.second)
			p = child;
	}
	return p.first;
}

map<string, double> Tree::getRanksByAvg()
{
	map<string, double> ranks;
	for (auto maps : m_root) {
		ranks[maps.first] = getRootAvg(maps.first);
	}
	return ranks;
}

void Tree::clear()
{
	m_root.clear();
	m_children.clear();
}

/*
void Tree::test()
{
	double d;
	for (int i = 0; i < 450; ++i) {
		srand(time(nullptr) * i);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/flowers/flowers", i, d);

		srand(time(nullptr) * i * 2);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/interview/interview", i, d);

		srand(time(nullptr) * i * 3);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/movie/movie", i, d);

		srand(time(nullptr) * i * 4);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/musicvideo/musicvideo", i, d);

		srand(time(nullptr) * i * 5);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/sports/sports", i, d);

		srand(time(nullptr) * i * 6);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/starcraft/starcraft", i, d);

		srand(time(nullptr) * i * 7);
		d = rand() % 10000 / 10000.0;
		insert("C:/database/traffic/traffic", i, d);
	}
}
*/