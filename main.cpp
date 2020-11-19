#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
using namespace std;
typedef struct{
    string lef;
    string rig;
}proc;//����ʽ
typedef struct{
    string selSet;
}sel;//select��
typedef struct{
    string firSet;
}fir;//fir��
typedef struct{
    string folSet;
}fol;//fol��
proc  procSet[200];//ԭʼ�ķ��Ĳ���ʽ��
fir  firSets[200];
fol  folSets[200];
sel  selSets[200];
string terSign = "n+-*/()$";
string nonTerSign = "SEXTYF";//����˳�����ķ�һ��
string null = "#";//#�Ǧ�
int analysTable[200][200];
stack<char> nonS;//���ս��ջ
stack<char> inputS;//���봮ջ
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
    init();         //�������ʽ
    headTerSign();  //�ս������FIRST��
    first();        //���з��ս����FIRST��
    follow();       //FOLLOW��
    select();       //FIRST��+FOLLOW��
    predAnaTable(); //Ԥ�������
    initInputS();   //��ʼ��ջ
    analys();       //��ӡ���
    return 0;
}

/*
 * �������ʽ
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
    cout << "�ķ�" << endl;
    for(int i = 0; !procSet[i].lef.empty(); i++){
        cout << procSet[i].lef << "->" << procSet[i].rig << endl;
    }
}

/*
 * ��һ�����ս������SELECT��
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
    cout << "��һ������" << endl;
}

/*
 * �ڶ��������з��ս����FIRST������SELECT��
 */

void first(){
    int i = 0;
    while(!procSet[i].rig.empty()){//�׷��ս��X
        if(nonTerSign.find(procSet[i].rig[0]) != string::npos){//ȡ�׷��ս��X
            string s = procSet[i].lef.substr(0,1);
            firstSet(s, i);//��FIRST(X)
        }
        i++;
    }
    i = 0;
    cout << "FIRST��" << endl;
    while(!procSet[i].rig.empty()){
        cout << "firSets[" << i << "].firSet " << firSets[i].firSet << endl;
        i++;
    }
    cout << "�ڶ�������" << endl;
}

/*
 * ��ĳ��s��FIRST��
 */

string firstSet(string& s, int p){//sָ��ǰ�󲿵ķ��ս��
    int i = p;//�ݹ��ȥ���ֱ���ҵ���������ʽ
    string terSigns = "";//����s���ս����
    while(!procSet[i].rig.empty()){
        if(s.find(procSet[i].lef) != string::npos){//�ҵ�X�Ĳ���ʽ
            if(procSet[i].rig.find(null) != string::npos){//���������
                continue;
            }
            if(terSign.find(procSet[i].rig[0]) != string::npos){//���ս��
                terSigns += procSet[i].rig[0];//��ӵ���ǰ�󲿷��ս��FIRST��
                if(!procSet[i+1].rig.empty()){//�Ƿ���ʽ���
                    i++;
                    continue;
                }
                return terSigns;
            }
            string r = procSet[i].rig.substr(0,1);//�׷��ս��
            string params = firstSet(r, i);
            if(firSets[i].firSet.find(params)){
                firSets[i].firSet += params;//��FIRST(X)
            }
            return params;
        }
        i++;
    }
    return "";
}

/*
 * ��ĳ��s��FOLLOW��
 */

string followSet(string& s){
    if(s[0] == 'S'){
        return "";
    }
    int i = 0;
    string res = "";//�ս����
    string sNext = "";//s��һ���ַ�
    while(!procSet[i].rig.empty()) {//AXB
        int sp = procSet[i].rig.find(s);//s��λ��
        int si = procSet[i].rig.size()-1;//�Ҳ�����±�
        if (sp != string::npos) {//�ҵ��ܵ���s�Ĳ���ʽ
            if (sp < si) {//s�����ַ�
                sNext += procSet[i].rig[sp + 1];//s��һ���ַ�
                if (terSign.find(sNext) != string::npos) {//AXb��������ս��
                    res += sNext;//��ӵ��ս����
                    i++;
                    continue;
                }//AXB����������ս��
                res += findSFirSet(sNext);
                if (produceNull(sNext).find(null) != string::npos) {//�˷��ս���ɵ�����
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
        int sp = procSet[i].rig.find(s);//s��λ��
        int si = procSet[i].rig.size()-1;//�Ҳ�����±�
        if (sp != string::npos) {//�ҵ��ܵ���s�Ĳ���ʽ
            if (sp == si) {//s�����ַ�
                if (procSet[i].lef == s) {//������Ⱦ�����
                    i++;
                    continue;
                }
                string f = "$";
                if (folSets[i].folSet.find(f) == string::npos) {//δ��ӹ�$
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
 * �����������з��ս����FOLLOW������SELECT��
 */

void follow(){
    int i = 0;
    while(!procSet[i].rig.empty()){
        if(null.find(procSet[i].rig[0]) != string::npos){//�׿�#
            folSets[i].folSet += followSet(procSet[i].lef);
        }
        i++;
    }
    i = 0;
    cout << "FOLLOW��" << endl;
    while(!procSet[i].rig.empty()){
        if(folSets[i].folSet.size() != 0){
            removDup(folSets[i].folSet);
            cout << "folSets[" << i << "].folSet " << folSets[i].folSet << endl;
        }
        i++;
    }
    cout << "����������" << endl;
}

/*
 * ���Ĳ���FIRST��+FOLLOW��
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
    cout << "SELECT��" << endl;
    while(!procSet[i].rig.empty()){
        cout << "selSets[" << i << "].selSet " << selSets[i].selSet << endl;
        i++;
    }
    cout << "���Ĳ�����" << endl;
}

/*
 * ����s��FIRST��
 */

string findSFirSet(string& s) {
    int i = 0;
    string res = "";
    while (!procSet[i].rig.empty()) {
        int sp = procSet[i].lef.find(s);
        if (sp != string::npos) {//�ҵ�s�Ĳ���ʽ
            res += firSets[i].firSet;
        }
        i++;
    }
    return res;
}

/*
 * ���岽��Ԥ�������
 */

void predAnaTable(){
    int x, y, i = 0;
    for(int i = 0; i < nonTerSign.size(); i++){
        for(int j = 0; j < terSign.size(); j++){
            analysTable[i][j] = -1;
        }
    }
    while (!procSet[i].rig.empty()) {
        x = nonTerSign.find(procSet[i].lef);//�����i��
        int rsize = selSets[i].selSet.size(), j = 0;
        while(rsize-- >= 0){
            string s = selSets[i].selSet.substr(j++, 1);//����SELECT���ַ�
            if(s.size() == 0){
                continue;
            }
            y = terSign.find(s);
            analysTable[x][y] = i;
        }
        i++;
    }
    cout << "\t\t\tԤ�������" << endl;
    cout << "\t" << "n \t" << "+ \t" << "- \t" << "* \t" << "/ \t" << "( \t" << ") \t" << "$" << endl;
    for(int i = 0; i < nonTerSign.size(); i++){
        cout << nonTerSign[i] << "\t";
        for(int j = 0; j < terSign.size(); j++){
             cout << analysTable[i][j] << " \t";
        }
        cout << endl;
    }
    cout << "���岽����" << endl;
}

/*
 * ����������ʼ�����봮ջ
 */

void initInputS(){
    cout << "������Ҫ�����ı��ʽ��..." << endl;
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
 * ���߲�������
 */

void analys(){
    nonS.push('S');
    while (!nonS.empty()){
        char o = nonS.top();
        int x = nonTerSign.find(nonS.top());
        char t = inputS.top();
        int y = judge(t);
        int i = analysTable[x][y];
        cout << "ʹ��" << procSet[i].lef << "->" << procSet[i].rig << endl;
        if(i == -1){
            cout << "�ⲻ�Ƿ����ķ�����ʽ" << endl;
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
                cout << "���Ƿ����ķ�����ʽ" << endl;
                return;
            }
            cout << "�ⲻ�Ƿ����ķ�����ʽ" << endl;
            return;
        }
        //ƥ��ɹ� ��ջ
        if(nonS.top() == inputS.top() || nonS.top()=='n' && inputS.top()<='9' && inputS.top()>='0'){
            nonS.pop();
            inputS.pop();
        }
    }
}

/*
 * ȥ��
 */

void removDup(string& s1){
    string s2 = "";
    for(size_t i=0;s1.cbegin()+i<s1.cend();i++){
        if(find(s1.cbegin()+i+1,s1.cend(),s1[i])==s1.end()){//û�ҵ��ظ���
            s2+=s1[i];//�ַ���ƴ���ַ�
        }
    }
    s1 = s2;
}

/*
 * ����s�ĵ�����
 */

string produceNull(string& s){
    int i = 0;
    string res = "";
    while (!procSet[i].rig.empty()) {
        int sp = procSet[i].lef.find(s);
        if (sp != string::npos) {//�ҵ�s�Ĳ���ʽ
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