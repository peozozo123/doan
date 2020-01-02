#include <iostream>
#include <fstream>
#include <string>

#define MAX_CHAR 256

using namespace std;

struct Node
{
    struct Node* children[MAX_CHAR];
    struct Node* suffixLink;
    int start;
    int* end;
    int xy;
};

string text;
Node* root = NULL;
Node* lastNewNode = NULL;
Node* activeNode = NULL;
int activeEdge;
int activeLength = 0;
int remainingSuffixCount = 0;
int leafEnd;
int* rootEnd = NULL;
int* splitEnd = NULL;
int size1;
int size2;

Node* newNode(int start, int* end)
{
    Node* node = (Node*)malloc(sizeof(Node));
    for (int i = 0; i < MAX_CHAR; i++) node->children[i] = NULL;
    node->suffixLink = root;
    node->start = start;
    node->end = end;
    node->xy = -1;
    return node;
}

int edgeLength(Node* n)
{
    if (n == root) return 0;
    return *(n->end) - (n->start) + 1;
}

bool walkDown(Node* currNode)
{
    if (activeLength >= edgeLength(currNode))
    {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}

void extendSuffixTree(int pos)
{
    leafEnd = pos;
    remainingSuffixCount++;
    lastNewNode = NULL;
    while (remainingSuffixCount > 0)
    {
        if (activeLength == 0) activeEdge = pos;
        if (activeNode->children[text[activeEdge]] == NULL)
        {
            activeNode->children[text[activeEdge]] = newNode(pos, &leafEnd);
            if (lastNewNode != NULL && activeNode != root)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }
        else
        {
            Node* next = activeNode->children[text[activeEdge]];
            if (walkDown(next))
            {
                continue;
            }
            if (text[next->start + activeLength] == text[pos])
            {
                if (lastNewNode != NULL && activeNode != root)
                {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }
                activeLength++;
                break;
            }
            splitEnd = (int*)malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;
            Node* split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;
            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = split;
            }
            lastNewNode = split;
        }
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0)
        {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        }
        else if (activeNode != root)
        {
            activeNode = activeNode->suffixLink;
        }
    }
}

void buildSuffixTree()
{
    size1 = text.size();
    int i;
    rootEnd = (int*)malloc(sizeof(int));
    *rootEnd = -1;
    root = newNode(-1, rootEnd);
    activeNode = root;
    for (i = 0; i < size1; i++) extendSuffixTree(i);
}

int setXY(Node* n, int labelHeight, int* maxHeight, int* substringStartIndex)
{
    int i = 0;
    int temp = -1;
    if (n->end==&leafEnd)
    {
        if ( size1 - labelHeight < size2) return -2;
        else if (size1 - labelHeight >= size2) return -3;
    }
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            temp = setXY(n->children[i], labelHeight + edgeLength(n->children[i]), maxHeight, substringStartIndex);

            if (n->xy == -1) n->xy = temp;
            else if ((n->xy == -2 && temp == -3) || (n->xy == -3 && temp == -2) || n->xy == -4)
            {
                n->xy = -4;
                if (*maxHeight < labelHeight)
                {
                    *maxHeight = labelHeight;
                    *substringStartIndex = *(n->end) - labelHeight + 1;
                }
            }
        }
    }

    return n->xy;
}

string getLCS()
{
    string res = "";
    int maxHeight = 0;
    int substringStartIndex = 0;
    setXY(root, 0, &maxHeight, &substringStartIndex);
    for (int i = 0; i < maxHeight; i++) res += text[i + substringStartIndex];
    return res;
}

int main()
{
   // ifstream in ("in1.txt");
  //  ofstream out ("out.txt");
    string a,b;
    in>>a>>b;
    text = a + "#" + b + "$";
    size2 = a.size();
    buildSuffixTree();
    cout << getLCS().size();
    return 0;
}
