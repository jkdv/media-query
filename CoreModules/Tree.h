#ifndef __fuzzy_tribble__Tree__
#define __fuzzy_tribble__Tree__

#include <map>
#include <vector>
#include <string>

using namespace std;

class Tree
{
private:
	map<string, double> m_root;
	map<string, map<int, double>> m_children;

public:
	Tree();
	Tree(const Tree& rhs);
	~Tree();
	Tree& operator=(const Tree& rhs);

	// inline functions
	double getRootMax(const string& videoName) const { return m_root.at(videoName); }

	double getChild(const string& videoName, int childIdx) const { return m_children.at(videoName).at(childIdx); }
	size_t getChildrenCount(const string& videoName) const { return m_children.at(videoName).size(); }
	map<string, double> getRanksByMax() { return m_root; }

	// non-inline functions
	double getRootAvg(const string& videoName) const;
	int locateMaxChild(const string& videoName);
	void insert(const string& videoName, int nFrame, double dValue);
	map<string, double> getRanksByAvg();
	

	void clear();
	//void test();
};

#endif /* defined(__fuzzy_tribble__Tree__) */
