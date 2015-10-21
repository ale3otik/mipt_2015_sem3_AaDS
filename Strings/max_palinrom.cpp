#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
void find_even_pallindroms(const string & str , vector<int> & d2)
{
	int l = 0, r = 0;

	d2[0] = 0;
	l = 0, r = -1;
	for(int i = 0; i < str.length(); ++i)
	{
		if(i <= r)
		{
			d2[i] = min(d2[r - i + l], r - i);
		}
		else d2[i] = 0;

		while(i + d2[i] < str.length() && i - d2[i] - 1 >= 0 && str[i + d2[i]] == str[i - d2[i] - 1])
		{
			++d2[i];
		}

		if(i + d2[i] - 1> r)
		{
			l = i - d2[i];
			r = i + d2[i] - 1;
		}
	}
}

void find_odd_pallindroms(const string & str , vector<int> & d1)
{
	int l = 0, r = 0;
	d1[0] = 0;
	for(int i = 1; i < str.length(); ++i)
	{
		if(i <= r)
		{
			d1[i] = min(d1[r - i + l], r - i);
		}
		else d1[i] = 0;

		while(i + d1[i] < str.length() && i - d1[i] >= 0 && str[i + d1[i]] == str[i - d1[i]])
		{
			++d1[i];
		}

		--d1[i];

		if(i + d1[i] > r)
		{
			l = i - d1[i];
			r = i + d1[i];
		}
	}
}

int main()
{
	string str;
	cin >> str;

	vector<int> d(str.length());
	
	int max_l = 1;
	int center = 0;
	
	for(int i = 0; i < 2; ++i)
	{
		if(i == 1) find_even_pallindroms(str,d);
		else find_odd_pallindroms(str,d);
		
		for(int j = 0; j < d.size(); ++j)
		{
			if(max_l < 2 * d[j] + 1 * (i+1)%2)
			{
				max_l = 2 * d[j] + 1 * (i+1)%2;
				center = j;
			}
		}
	}
	// cout << center <<" "<< max_l << endl;
	int start = center - max_l/2;
	cout << str.substr(start, max_l) << endl;
	return 0;
}