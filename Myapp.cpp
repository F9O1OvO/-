#include <iostream>      
#include <vector>        
#include <string>        
#include <map>           
#include <random>        
#include <chrono>        
#include <algorithm>     
#include <fstream>       
#include <stack>         
#include <cstdlib>       
#include <cctype>        
using namespace std;
#define int long long
const int inf = 0x3f3f3f3f;
static mt19937_64 sj(chrono::steady_clock::now().time_since_epoch().count());
int num = 10000, maxm = 100;
vector<string> op = { "+", "-", "*", "/" };
map<char, char> mp1, mp2;
uniform_int_distribution<int> promaxm(0, maxm - 1);//roll数大小
uniform_int_distribution<int> prolen(0, 3);//3为符号数量的上限,可自行调控
uniform_int_distribution<int> proop(0, op.size() - 1);//roll运算符
uniform_int_distribution<int> plusop(0, 100);//roll括号
string s1 = "-n", s2 = "-r", s3 = "-e", s4 = "-a";
string inputfile = "Exercises.txt", ansfile = "Answer.txt";
string outfile1 = "Exercises.txt", outfile2 = "Answer.txt", outfile3 = "Grade.txt";
map<string, int>mp;
int gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}
struct number {
    int son, mum; // 构造函数
    number(int n = 0, int d = 1) : son(n), mum(d) {
        simplify();
    }
    void simplify() {
        int g = gcd(abs(son), abs(mum));
        son /= g, mum /= g;
        if (mum < 0) {
            son = -son;
            mum = -mum;
        }
    }
    number operator+(const number& other) const { // 分数加法
        return number(son * other.mum + other.son * mum, mum * other.mum);
    }
    number operator-(const number& other) const { // 分数减法
        return number(son * other.mum - other.son * mum, mum * other.mum);
    }
    number operator*(const number& other) const { // 分数乘法
        return number(son * other.son, mum * other.mum);
    }
    number operator/(const number& other) const { // 分数除法
        return number(son * other.mum, mum * other.son);
    }
    number& operator=(const number& other) {
        if (this != &other) { // 防止自我赋值
            son = other.son;
            mum = other.mum;
            simplify(); // 确保赋值后仍然是简化的形式
        }
        return *this;
    }
};

number check(string s) {
    // 获取操作符优先级
    auto get_op = [&](char op1) {
        if (op1 == '+' || op1 == '-') return 1;
        if (op1 == '*' || op1 == '/') return 2;
        return 0;
        };
    // 计算两个数的运算结果，同时判断是否为负数
    auto cul = [&](number a, number b, char op1) {
        number result(0, 1);
        switch (op1) {
        case '+':
            result = a + b;
            break;
        case '-':
            result = a - b;
            break;
        case '*':
            result = a * b;
            break;
        case '/':
            // 检查是否除以 0
            if (b.son == 0) {
                // cout << "Error: Division by zero!" << endl;
                return number(0, 1); // 返回默认分数 0/1
            }
            result = a / b;
            break;
        default:
            return number(0, 1); // 不应该到达这里，返回默认分数 0/1
        }
        if (result.son < 0) {
            result.son = -1, result.mum = -1;
            return result; // 如果结果为负，返回 (-inf, -inf)
        }
        return result;
        };

    stack<number> values; // 用于存储分数的栈
    stack<char> ops;      // 用于存储操作符的栈

    for (int i = 0; i < s.size(); ++i) {
        if (isdigit(s[i])) {
            int num = 0;
            while (i < s.size() && isdigit(s[i])) {
                num = num * 10 + (s[i] - '0');
                i++;
            }
            values.push(number(num, 1)); // 将数字作为分数推入栈，分母设置为 1
            i--;
        }
        // 处理左括号
        else if (s[i] == '(') {
            ops.push('(');
        }
        else if (s[i] == ')') {
            while (!ops.empty() && ops.top() != '(') {
                number val2 = values.top();
                values.pop();
                number val1 = values.top();
                values.pop();
                char op1 = ops.top();
                ops.pop();
                number result = cul(val1, val2, op1);

                if (result.son < 0 || result.mum < 0) {
                    // cout << "a" << endl;
                    return result;
                }
                values.push(result); // 计算两个数并推入栈
            }
            ops.pop(); // 弹出左括号
        }
        // 处理操作符
        else if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            while (!ops.empty() && get_op(ops.top()) >= get_op(s[i])) {
                number val2 = values.top();
                values.pop();
                number val1 = values.top();
                values.pop();
                char op1 = ops.top();
                ops.pop();
                number result = cul(val1, val2, op1);

                if (result.son < 0 || result.mum < 0) {
                    // cout << "b" << endl;
                    return result;
                }
                values.push(result);
            }
            ops.push(s[i]); // 将当前操作符压入栈
        }
    }

    // 处理剩余操作符
    while (!ops.empty()) {
        number val2 = values.top();
        values.pop();
        number val1 = values.top();
        values.pop();
        char op1 = ops.top();
        ops.pop();
        number result = cul(val1, val2, op1);
        if (result.son < 0 || result.mum < 0) {
            // cout << "c" << endl;
            return result;
        }
        values.push(result);
    }
    number final_result = values.top();
    return final_result; // 返回最终的计算结果
}

string pplus(string s) {
    string now = "";
    int cnt1 = 0;
    for (int i = 0, flag = 1; i < s.size(); i++) {
        if (isdigit(s[i])) {
            if (flag) {
                flag = 0;
                if (plusop(sj) % 10 == 0) {
                    now += '(';
                    cnt1++;
                }
            }
        }
        else {
            if (!flag && cnt1 && plusop(sj) % 12 == 0) {
                now += ')';
                cnt1--;
            }
            flag = 1;
        }
        now += s[i];
    }
    for (int i = 0; i < cnt1; i++) {
        now += ')';
    }
    // cout << now << endl;
    return now;
}

string getstring() { // 生成每一个字符串
    string now = "";
    int cnt = prolen(sj) + 1;
    for (int i = 0; i < cnt; i++) {
        int num1 = promaxm(sj);
        string op1 = op[proop(sj)];
        now += to_string(num1);
        if (i != cnt - 1) now += op1;
    }
    return now;
}

void create() { // 生成问题
    vector<string> pro;
    for (int i = 0; i < num; i++) {
        string now = getstring();
        now = pplus(now);
        number tt = check(now);
        while (tt.son < 0 || tt.mum < 0) {
            now = getstring();
            now = pplus(now);
            tt = check(now);
        }
        pro.push_back(now);
    }
    std::ofstream file(outfile1);
    if (!file.is_open()) {
        cout << "Can't open file:" << outfile1 << endl;
        exit(0);
    }
    for (int i = 0; i < pro.size(); i++) file << i + 1 << "." << pro[i] << endl;
}

vector<string> read(string s) { // 读入文件信息
    vector<string> v;
    ifstream ifs(s);
    if (!ifs.is_open()) {
        cout << "Can't open file:" << s << endl;
        exit(0);
    }
    string line, now;
    while (getline(ifs, line)) {
        now = "";
        int flag = 0;
        for (int i = 0; i < line.length(); i++) {
            if (flag == 1) now += line[i];
            if (line[i] == '.') flag = 1;
        }
        v.push_back(now);
        line = "";
    }
    ifs.close();
    return v;
}

vector<string> work(vector<string> pro) { // 计算答案
    vector<string> ans;
    for (int i = 0; i < pro.size(); i++) {
        number now = check(pro[i]);
        // cout << check(pro[i]).son << " " << check(pro[i]).mum << endl;
        if (now.son < 0 || now.mum < 0) {
            string s = "Negative numbers appear in this equation";
            ans.push_back(s);
        }
        else {
            string s;
            if (now.mum == 1) {
                ans.push_back(to_string(now.son));
                continue;
            }
            int cnt = now.son / now.mum;
            if (cnt) now.son %= now.mum, s += to_string(cnt), s += '`';
            s += to_string(now.son), s += "/", s += to_string(now.mum);
            ans.push_back(s);
        }
    }
    return ans;
}

void write1(vector<string> ans) { // 输出计算答案
    std::ofstream file(outfile2);
    if (!file.is_open()) {
        cout << "Can't open file:" << outfile1 << endl;
        exit(0);
    }
    for (int i = 0; i < ans.size(); i++) file << i + 1 << "." << ans[i] << endl;
    file.close();
}

void write2(vector<int> right, vector<int> wrong) { // 输出比较答案
    std::ofstream file(outfile3);
    if (!file.is_open()) {
        cout << "Can't open file:" << outfile3 << endl;
        exit(0);
    }
    file << "Correct:" << right.size();
    file << "(";
    for (int i = 0; i < right.size(); i++) {
        file << right[i] << ",)"[i == right.size() - 1];
    }
    file << endl;
    file << "Wrong:" << wrong.size();
    file << "(";
    if (wrong.size() == 0) {
        file << ")" << endl;
    }
    for (int i = 0; i < wrong.size(); i++) {
        file << wrong[i] << ",)"[i == wrong.size() - 1];
    }
}

void compare(vector<string> ans, vector<string> ans1) { //
    int cnt1 = 0, cnt2 = 0;
    vector<int> right, wrong;
    for (int i = 0; i < ans.size(); i++) {
        if (ans[i] != ans1[i]) {
            wrong.push_back(i + 1);
            cnt2++;
        }
        else {
            right.push_back(i + 1);
            cnt1++;
        }
    }
    write2(right, wrong);
}

void solve(int flag) { // 解决问题
    if (flag == 1) {
        create();
        vector<string> pro = read(outfile1);
        vector<string> ans = work(pro);
        write1(ans);
    }
    else {
        vector<string> pro = read(inputfile);
        vector<string> ans = work(pro);
        vector<string> ans1 = read(ansfile);
        compare(ans, ans1);
    }
}


signed main(int argc, char* argv[]) {
    // cout<<argc<<endl;
    if (argc != 5) {
       //cout << argc << endl;
       cout << "the input is wrong,please restart the program" << endl;
       return 0;
    }
    if (argv[1] == s1) {
       num = atoi(argv[2]), maxm = atoi(argv[4]);
       // cout<<num<<" "<<maxm<<endl;
       solve(1);
    }
    else if (argv[1] == s3) {
       inputfile = argv[2], ansfile = argv[4];
       solve(2);
    }
    return 0;
}
