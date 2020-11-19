#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
using namespace std;
typedef struct{
    string lef;
    string rig;
}proc;//产生式
typedef struct{
    string selSet;
}sel;//select集
typedef struct{
    string firSet;
}fir;//fir集
typedef struct{
    string folSet;
}fol;//fol集
proc  procSet[200];//原始文法的产生式集
fir  firSets[200];
fol  folSets[200];
sel  selSets[200];
string terSign = "n+-*/()$";
string nonTerSign = "SEXTYF";//出现顺序与文法一致
string null = "#";//#是ε
int analysTable[200][200];
stack<char> nonS;//非终结符栈
stack<char> inputS;//输入串栈
void init();
void headTerSign();
void first();
void follow();
void select();
void predAnaTable();
void initInputS();
void analys();
string firstSet(string& s, int p);
string findSFirSet(string& s);
void removDup(string& s);
string produceNull(string& s);
int judge(char iTop);
//(((4+9)*6-1)/2+4)*6$
//(((4+9)6-1)/2+4)*6$
int main() {
    init();         //载入产生式
    headTerSign();  //终结符放入FIRST集
    first();        //所有非终结符的FIRST集
    follow();       //FOLLOW集
    select();       //FIRST集+FOLLOW集
    predAnaTable(); //预测分析表
    initInputS();   //初始化栈
    analys();       //打印结果
    return 0;
}

/*
 * 载入产生式
 */

void init(){
    string FilePath = "D:\\Program_Files\\Clion_space\\yffx\\wenfa.txt";
    fstream fin(FilePath);
    int i = 1, n = 0;
    while(!fin.eof()){
        if(i/2*2!=i){
            fin >> procSet[n].lef;
        }else{
            fin >> procSet[n++].rig;
        }
        i++;
    }
    cout << "文法" << endl;
    for(int i = 0; !procSet[i].lef.empty(); i++){
        cout << procSet[i].lef << "->" << procSet[i].rig << endl;
    }
}

/*
 * 第一步：终结符放入SELECT集
 */

void headTerSign(){
    int i = 0;
    while(!procSet[i].rig.empty()){
        if(terSign.find(procSet[i].rig[0]) != string::npos){
            firSets[i].firSet += procSet[i].rig[0];
            cout << "firSets[" << i << "].firSet " << firSets[i].firSet << endl;
        }
        i++;
    }
    cout << "第一步结束" << endl;
}

/*
 * 第二步：所有非终结符的FIRST集放入SELECT集
 */

void first(){
    int i = 0;
    while(!procSet[i].rig.empty()){//首非终结符X
        if(nonTerSign.find(procSet[i].rig[0]) != string::npos){//取首非终结符X
            string s = procSet[i].lef.substr(0,1);
            firstSet(s, i);//求FIRST(X)
        }
        i++;
    }
    i = 0;
    cout << "FIRST集" << endl;
    while(!procSet[i].rig.empty()){
        cout << "firSets[" << i << "].firSet " << firSets[i].firSet << endl;
        i++;
    }
    cout << "第二步结束" << endl;
}

/*
 * 求某个s的FIRST集
 */

string firstSet(string& s, int p){//s指当前左部的非终结符
    int i = p;//递归进去后会直接找到这条产生式
    string terSigns = "";//保存s的终结符串
    while(!procSet[i].rig.empty()){
        if(s.find(procSet[i].lef) != string::npos){//找到X的产生式
            if(procSet[i].rig.find(null) != string::npos){//如果导出空
                continue;
            }
            if(terSign.find(procSet[i].rig[0]) != string::npos){//首终结符
                terSigns += procSet[i].rig[0];//添加到当前左部非终结符FIRST集
                if(!procSet[i+1].rig.empty()){//是否访问结束
                    i++;
                    continue;
                }
                return terSigns;
            }
            string r = procSet[i].rig.substr(0,1);//首非终结符
            string params = firstSet(r, i);
            if(firSets[i].firSet.find(params)){
                firSets[i].firSet += params;//求FIRST(X)
            }
            return params;
        }
        i++;
    }
    return "";
}

/*
 * 求某个s的FOLLOW集
 */

string followSet(string& s){
    if(s[0] == 'S'){
        return "";
    }
    int i = 0;
    string res = "";//终结符集
    string sNext = "";//s下一个字符
    while(!procSet[i].rig.empty()) {//AXB
        int sp = procSet[i].rig.find(s);//s的位置
        int si = procSet[i].rig.size()-1;//右部最大下标
        if (sp != string::npos) {//找到能导出s的产生式
            if (sp < si) {//s后还有字符
                sNext += procSet[i].rig[sp + 1];//s下一个字符
                if (terSign.find(sNext) != string::npos) {//AXb后面紧跟终结符
                    res += sNext;//添加到终结符集
                    i++;
                    continue;
                }//AXB后面紧跟非终结符
                res += findSFirSet(sNext);
                if (produceNull(sNext).find(null) != string::npos) {//此非终结符可导出ε
                    if(folSets[i].folSet.size() != 0){
                        res += folSets[i].folSet;
                    }else{
                        res += followSet(procSet[i].lef);
                    }
                }
            }
        }
        sNext = "";
        i++;
    }
    i = 0;
    while(!procSet[i].rig.empty()) {//AX
        int sp = procSet[i].rig.find(s);//s的位置
        int si = procSet[i].rig.size()-1;//右部最大下标
        if (sp != string::npos) {//找到能导出s的产生式
            if (sp == si) {//s后无字符
                if (procSet[i].lef == s) {//和左部相等就跳过
                    i++;
                    continue;
                }
                string f = "$";
                if (folSets[i].folSet.find(f) == string::npos) {//未添加过$
                    res += f;
                }
                string x = procSet[i].lef;
                res += followSet(procSet[i].lef);
            }
        }
        i++;
    }
    return res;
}

/*
 * 第三步：所有非终结符的FOLLOW集放入SELECT集
 */

void follow(){
    int i = 0;
    while(!procSet[i].rig.empty()){
        if(null.find(procSet[i].rig[0]) != string::npos){//首空#
            folSets[i].folSet += followSet(procSet[i].lef);
        }
        i++;
    }
    i = 0;
    cout << "FOLLOW集" << endl;
    while(!procSet[i].rig.empty()){
        if(folSets[i].folSet.size() != 0){
            removDup(folSets[i].folSet);
            cout << "folSets[" << i << "].folSet " << folSets[i].folSet << endl;
        }
        i++;
    }
    cout << "第三步结束" << endl;
}

/*
 * 第四步：FIRST集+FOLLOW集
 */

void select(){
    int i = 0;
    while (!procSet[i].rig.empty()) {
        if(firSets[i].firSet.size() != 0){
            selSets[i].selSet += firSets[i].firSet;
        }else{
            selSets[i].selSet += folSets[i].folSet;
        }
        i++;
    }
    i = 0;
    cout << "SELECT集" << endl;
    while(!procSet[i].rig.empty()){
        cout << "selSets[" << i << "].selSet " << selSets[i].selSet << endl;
        i++;
    }
    cout << "第四步结束" << endl;
}

/*
 * 返回s的FIRST集
 */

string findSFirSet(string& s) {
    int i = 0;
    string res = "";
    while (!procSet[i].rig.empty()) {
        int sp = procSet[i].lef.find(s);
        if (sp != string::npos) {//找到s的产生式
            res += firSets[i].firSet;
        }
        i++;
    }
    return res;
}

/*
 * 第五步：预测分析表
 */

void predAnaTable(){
    int x, y, i = 0;
    for(int i = 0; i < nonTerSign.size(); i++){
        for(int j = 0; j < terSign.size(); j++){
            analysTable[i][j] = -1;
        }
    }
    while (!procSet[i].rig.empty()) {
        x = nonTerSign.find(procSet[i].lef);//处理第i行
        int rsize = selSets[i].selSet.size(), j = 0;
        while(rsize-- >= 0){
            string s = selSets[i].selSet.substr(j++, 1);//遍历SELECT集字符
            if(s.size() == 0){
                continue;
            }
            y = terSign.find(s);
            analysTable[x][y] = i;
        }
        i++;
    }
    cout << "\t\t\t预测分析表" << endl;
    cout << "\t" << "n \t" << "+ \t" << "- \t" << "* \t" << "/ \t" << "( \t" << ") \t" << "$" << endl;
    for(int i = 0; i < nonTerSign.size(); i++){
        cout << nonTerSign[i] << "\t";
        for(int j = 0; j < terSign.size(); j++){
             cout << analysTable[i][j] << " \t";
        }
        cout << endl;
    }
    cout << "第五步结束" << endl;
}

/*
 * 第六步：初始化输入串栈
 */

void initInputS(){
    cout << "请输入要分析的表达式串..." << endl;
    string s;
    cin >> s;
    char x;
    int i = s.size();
    do {
        x = s[--i];
        inputS.push(x);
    }while (i);
}

/*
 * 第七步：分析
 */

void analys(){
    nonS.push('S');
    while (!nonS.empty()){
        char o = nonS.top();
        int x = nonTerSign.find(nonS.top());
        char t = inputS.top();
        int y = judge(t);
        int i = analysTable[x][y];
        cout << "使用" << procSet[i].lef << "->" << procSet[i].rig << endl;
        if(i == -1){
            cout << "这不是符合文法的算式" << endl;
            return;
        }
        int size = procSet[i].rig.size();
        char c;
        nonS.pop();
        do {
            c = procSet[i].rig[--size];
            if(c != '#'){
                nonS.push(c);
            }
        }while (size);
        if(nonS.empty()){
            if(inputS.top() == '$'){
                cout << "这是符合文法的算式" << endl;
                return;
            }
            cout << "这不是符合文法的算式" << endl;
            return;
        }
        //匹配成功 出栈
        if(nonS.top() == inputS.top() || nonS.top()=='n' && inputS.top()<='9' && inputS.top()>='0'){
            nonS.pop();
            inputS.pop();
        }
    }
}

/*
 * 去重
 */

void removDup(string& s1){
    string s2 = "";
    for(size_t i=0;s1.cbegin()+i<s1.cend();i++){
        if(find(s1.cbegin()+i+1,s1.cend(),s1[i])==s1.end()){//没找到重复的
            s2+=s1[i];//字符串拼接字符
        }
    }
    s1 = s2;
}

/*
 * 返回s的导出集
 */

string produceNull(string& s){
    int i = 0;
    string res = "";
    while (!procSet[i].rig.empty()) {
        int sp = procSet[i].lef.find(s);
        if (sp != string::npos) {//找到s的产生式
            res += procSet[i].rig;
        }
        i++;
    }
    return res;
}
int judge(char iTop){
    if(iTop>='0' && iTop <='9'){
        return 0;
    }
    string s = "";
    s += iTop;
    return terSign.find(s);
}