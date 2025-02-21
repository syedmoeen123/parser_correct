#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace std;

const char PLUS = '+';
const char MINUS = '-';
const char MUL = '*';
const char DIV = '/';
const char LPAREN = '(';
const char RPAREN = ')';


class Scanner{
private:
    int pos=-1;
    string s;
    int len;

public:
    Scanner(string s){
        this->s=s;
        this->len=s.length();
    }

    char nextToken(){
        return s[pos+1];
    }
    void advance(){
        pos++;
    }
    char tokenValue(){
        return s[pos+1];
    }
};

class TreeNode{
    private:
        char val;
        char op;
        TreeNode *left;
        TreeNode *right;

    public:
        TreeNode(char op, char val){
            this->val =val;
            this->left=nullptr;
            this->right=nullptr;
        }

        TreeNode(char op, TreeNode* exprSofar=nullptr, TreeNode* t2=nullptr) {
        this->val = op;
        this->left = exprSofar;
        this->right = t2;
    }
    
    void printTree(int indent = 0){
        if (val != '\0') {
             cout << string(indent,' ') << val<< "\n";
        }
        if (left != nullptr) {
            left->printTree(indent+2);
        }
        if (right != nullptr) {
            right->printTree(indent +2);
        }
    }
};

class NonTerminal {
public:
    NonTerminal(Scanner* sc){
        s = sc; tree = NULL;
    }
    virtual ~NonTerminal(){}
    virtual bool isPresent()=0;
    TreeNode* AST(){
        return tree;
    }
protected:
    Scanner* s;
    TreeNode* tree;
};

class Goal:public NonTerminal {
    public:
        Goal(Scanner* sc): NonTerminal(sc){ }
        virtual bool isPresent();
};

class Expr:public NonTerminal {
public:
    Expr(Scanner* sc): NonTerminal(sc){ }
    virtual bool isPresent();
};

class Eprime:public NonTerminal {
public:
    Eprime(Scanner* sc, TreeNode* t): NonTerminal(sc) {
        exprSofar = t;
    }
    virtual bool isPresent();
protected:
    TreeNode* exprSofar;
};

class Term:public NonTerminal {
public:
    Term(Scanner* sc): NonTerminal(sc){ }
    virtual bool isPresent();
};

class Tprime:public NonTerminal {
public:
    Tprime(Scanner* sc, TreeNode* t): NonTerminal(sc) {
        exprSofar = t;
    }
    virtual bool isPresent();
protected:
    TreeNode* exprSofar;
};

class Factor:public NonTerminal {
public:
    Factor(Scanner* sc): NonTerminal(sc){ };
    virtual bool isPresent();
};

bool Goal::isPresent(){
    Expr* op1 = new Expr(s);
    if(!op1->isPresent())
        return false;
    tree=op1->AST();
    cout<<"\nAbstract Syntax Tree:\n";
    tree->printTree();
    return true;
}

bool Expr::isPresent() {
    Term* op1 = new Term(s);
    if(!op1->isPresent())
        return false;
    tree = op1->AST();
    Eprime* op2 = new Eprime(s, tree);
    if(op2->isPresent())
        tree = op2->AST();
    
    return true;
}

bool Eprime::isPresent()
{
    char op=s->nextToken();
    if(op==PLUS || op==MINUS){
        s->advance();
        Term* op2=new Term(s);
        if(!op2->isPresent()){
            cout << "syntax error\n";
        }
            //syntaxError();
        TreeNode* t2=op2->AST();
        tree = new TreeNode(op,exprSofar,t2);
        Eprime* op3 = new Eprime(s, tree);
        if(op3->isPresent())
            tree = op3->AST();
        return true;
    }
    else return false;
}

bool Term::isPresent()
{
    Factor* op1 = new Factor(s);
    if(!op1->isPresent())
        return false;
    tree = op1->AST();
    Tprime* op2 = new Tprime(s, tree);
    if(op2->isPresent())
        tree = op2->AST();
    return true;
}

bool Tprime::isPresent()
{
    char op=s->nextToken();
    if(op == MUL || op == DIV){
        s->advance();
        Factor* op2=new Factor(s);
        if(!op2->isPresent()){
            cout << "syntax error\n";
        }
            //2syntaxError();
        TreeNode* t2=op2->AST();
        tree = new TreeNode(op,exprSofar,t2);
        Tprime* op3 = new Tprime(s, tree);
        if(op3->isPresent())
            tree = op3->AST();
        return true;
    }
    else return false;
}
bool isID(char x){
    return (x >= 'a' && x <= 'z');
}
bool isNUM(char x){
    return (x >= '0' && x <= '9');
}


bool Factor::isPresent()
{
    char op=s->nextToken();
    if( isID(op) || isNUM(op) )
    {
        tree = new TreeNode(op,s->tokenValue());
        s->advance();
        return true;
    }
    if( op == LPAREN ){
        s->advance();
        Expr* opr = new Expr(s);
        if(!opr->isPresent() ){
            cout << "syntax error\n";
        }
            // syntaxError();
        if(s->nextToken() != RPAREN){
           cout << "syntax error\n";
        }
            // syntaxError();
        s->advance();
        tree = opr->AST();
        return true;
    }
    return false;
}
int main(){
    string s = "8+(4+3)";
    Scanner sc(s);
    Goal goal(&sc);
    if(goal.isPresent()){
        cout << "success"<<endl;
    }
    return 0;
}
