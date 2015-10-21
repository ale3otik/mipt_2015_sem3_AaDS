#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;
// using z fucntion
void kmp_zf(string str , int tmp_len)
{
	int len = str.length();
	int r = 0,l = 0;
	vector<int> zf(tmp_len);
	zf.resize(tmp_len);
	zf[0] = 0;
	int i;

	for(i = 1; i < tmp_len; ++ i)
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

	int zf_cur;
	++i;
	for(;i<len; ++ i)
	{
		zf_cur = 0;
		if(i <= r)
		{
			zf_cur = min(zf[i - l], r - i + 1);
		}

		while(i + zf_cur < len && str[i + zf_cur] == str[zf_cur])
		{
			++zf_cur;
		}

		if(i + zf_cur - 1 > r)
		{
			r = i + zf_cur - 1;
			l = i;
		}	
		if(zf_cur == tmp_len) cout << i - tmp_len - 1 << " ";
	}
}

void kmp_prf(string str,int tmp_len)
{
	int len = str.length();
	vector<int> pr(tmp_len);

	pr[0] = 0;
	int i;
	for(i = 1; i < tmp_len; ++ i)
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

	int pr_last = 0;
	for(++i; i < len; ++i)
	{
		int pr_cur = 0;
		int j = pr_last;
		while(j > 0 && str[i] != str[j])
		{
			j = pr[j-1];
		}

		if(str[i] == str[j])
		{
			pr_cur = j + 1;
		}
		else
		{
			pr_cur = 0;
		}

		pr_last = pr_cur;
		if(pr_cur == tmp_len)
		{
			cout << i - 2*tmp_len<<" ";
		}

	}

}
int main()
{
	// freopen("in.txt", "r", stdin);
	string src, tmp, all;
	cin >> tmp >> src;

	reverse(src.begin(),src.end());
	reverse(tmp.begin(),tmp.end());
	src += "#" + tmp;
	reverse(src.begin(),src.end());
	// cout << src << endl;

	kmp_prf(src,tmp.length());
	return 0;
}