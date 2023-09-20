#include <bits/stdc++.h>
using namespace std;
class Solution
{
public:
    int minCost(vector<int> &q,int l,int r,int ql,int qr)
    {
        int n=q.size();
        //0表示上次删前面，1表示上次删后面
        vector<vector<vector<int>>> dp(n,vector<vector<int>>(n,vector<int>(3,-1)));
        function<int(int, int, int)> helper = [&](int left, int right, int pre) -> int
        {
            if(left==right)
            {
                return min(l,r)*q[left];
            }
            if(dp[left][right][pre]!=-1)
            {
                return dp[left][right][pre];
            }
            int front = helper(left + 1, right, 0) + l * q[left] + (pre == 0 ? ql : 0);
            int back = helper(left, right - 1, 1) + r * q[right] + (pre == 1 ? qr : 0);
            return dp[left][right][pre] = min(front, back);
        };
        return helper(0,n-1,2);
    }
};

int main()
{
    int n, l, r, ql, qr;
    cin >> n >> l >> r >> ql >> qr;
    vector<int> q;
    int tmp = 0;
    while(cin>>tmp)
    {
        q.push_back(tmp);
    }
    Solution s;
    cout << s.minCost(q, l, r, ql, qr) << endl;
}