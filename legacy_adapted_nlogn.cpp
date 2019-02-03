#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <bitset>
#include <complex>
#include <random>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
using namespace std;

#ifdef LOCAL
#define eprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define eprintf(...) (void)0
#endif

namespace NLogN
{
	const int N = 3500500;
	const int A = 26;
	const int INF = (int)1e9;

	struct Eertree
	{
		int sz, last;
		int go[N][A];
		int len[N], diff[N];
		int link[N], series_link[N];
		int dp[2][N], last_dp[2][N];

		Eertree() : len(), diff(), link(), series_link(), dp(), last_dp()
		{
			sz = 2;
			last = 0;
			memset(go, -1, sizeof(go));
			len[0] = -1;
            for (int c : {0, 1})
                fill(dp[c], dp[c] + N, INF);
			dp[0][0] = 0;
		}

		int go_until_pal(const string &s, int i, int v)
		{
			while (s[i] != s[i - len[v] - 1])
				v = link[v];
			return v;
		}

		void add_char(const string &s, int i)
		{
			last = go_until_pal(s, i, last);
			int &v = go[last][s[i] - 'a'];
			if (v == -1)
			{
				v = sz++;
				len[v] = len[last] + 2;
				if (last == 0)
					link[v] = 1;
				else
				{
					last = go_until_pal(s, i, link[last]);
					link[v] = go[last][s[i] - 'a'];
				}
				diff[v] = len[v] - len[link[v]];
				if (diff[v] == diff[link[v]])
					series_link[v] = series_link[link[v]];
				else
					series_link[v] = link[v];
			}
			last = v;
		}

		void calc_dp(int v, int i)
		{
            for (int c : {0, 1})
            {
                int baby_len = len[series_link[v]] + diff[v];
                int res = dp[1 - c][i - baby_len] + 1;
                if (link[v] != series_link[v])
                    res = min(res, last_dp[1 - c][link[v]]);
                last_dp[1 - c][v] = res;
                dp[c][i] = min(dp[c][i], res);
            }
		}
	};

	Eertree eertree;

	vector<int> solve(string s)
	{
		int n = (int)s.length();
		s = "#" + s;
		vector<int> res[2];

		for (int i = 1; i <= n; i++)
		{
			eertree.add_char(s, i);
			for (int v = eertree.last; v != 1; v = eertree.series_link[v])
				eertree.calc_dp(v, i);
            for (int c : {0, 1}) {
                res[c].push_back(eertree.dp[c][i]);
            }
		}

        vector<int> pal_len(res[0].size(), INF);
        for (int c : {0, 1})
            for (int i = 0; i < (int)pal_len.size(); ++i)
            {
                pal_len[i] = min(pal_len[i], res[c][i]);
                bool ok = false;
                ok |= i < 2;
                ok |= res[c][i] == INF;
                ok |= abs(res[c][i - 1] - res[c][i]) <= 2;
                ok |= abs(res[c][i - 2] - res[c][i]) <= 2;
                if (!ok)
                    throw;
            }
        return pal_len;
	}
}

char buf[3500500];

void solve()
{
	scanf("%s", buf);
	string s(buf);

	auto res = NLogN::solve(s);
	for (auto e = res.begin(); e != res.end(); e++)
		printf("%d ", *e);
	printf("\n");
}

int main()
{
#ifdef LOCAL
	freopen("input.txt", "r", stdin);
	//freopen("output.txt", "w", stdout);
#else
	//freopen(".in", "r", stdin);
	//freopen(".out", "w", stdout);
#endif

	solve();

	eprintf("\n\ntime: %.3lf\n", (double)clock() / CLOCKS_PER_SEC);
	return 0;
}
