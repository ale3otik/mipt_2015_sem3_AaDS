#include <iostream>
#include <string>
#include <vector>


using namespace std;

void z_function(vector<int> & zf, string str)
{
	int len = str.length();
	int r = 0,l = 0;
	zf.resize(len);
	zf[0] = 0;
	for(int i = 1; i < len; ++ i)
	{
		if(i <= r)
		{
			zf[i] = min(zf[i - l], r - i + 1);
		}

		while(i + zf[i] < len && str[i + zf[i]] == str[zf[i]])
		{
			++zf[i];
		}

		if(i + zf[i] - 1 > r)
		{
			r = i + zf[i] - 1;
			l = i;
		}
	}
}

void prefix_function(vector<int> & pr, string str)
{
	int len = str.length();
	pr.resize(len);
	pr[0] = 0;
	for(int i = 1; i < len; ++ i)
	{
		int j = pr[i-1];
		while(j > 0 && str[i] != str[j])
		{
			j = pr[j-1];
		}

		if(str[i] == str[j])
		{
			pr[i] = j + 1;
		}
		else
		{
			pr[i] = 0;
		}
	}
}

int main()
{
	return  0;
}