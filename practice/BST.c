// #include <stdio.h>
// #include <stdlib.h>

// typedef struct _TreeNode{
//     int key; // key 값
//     TreeNode * left;    // left child
//     TreeNode * right;   // right child
// }TreeNode;

// TreeNode* search(TreeNode* root, int key){
//     if(root == NULL){   // 값을 찾지 못한 경우  
//         printf("Error : 값을 찾을 수 없습니다\n");
//         return root;
//     }

//     if(key == root->key){   // 값을 찾음 
//         return root;
//     }
    
//     else if(key < root->key){   // 왼쪽 서브트리 탐색 
//         search(root->left, key);
//     }

//     else if(key > root->key){   // 오른쪽 서브트리 탐색 
//         search(root->right, key);
//     }
// }


#include <stdio.h>
#include <stdlib.h>

typedef char data;
typedef struct _Node {
    char key; // 255 이하
    struct _Node* left;
    struct _Node* right; 
} Node;

// 찾기
// 자기 자신보다 작으면 왼쪽 자식, 크면 오른쪽 자식
// 찾는 값이랑 같으면 값 반환
// NULL이 나올때까지 돌다가 NULL반환
// 반환은 Node* 또는 return : 반환형
Node* searchBST(Node *root, char x){
    Node *p = root;
    while (p != NULL){
        if (p->key == x) return p;
        else if (p->key < x){
            p = p->right;
        }else{
            p = p->left;
        }
    }
    return NULL;
};

Node* insertBST(Node* root, char x){
    Node* p = root;
    // 빈트리에 첫번째 노드가 들어갈 경우에는 while문을 돌지 않기때문에
    // parent에 Null을 넣어줌 
    Node* parent = NULL;
    while (p != NULL){        
        parent = p; // 부모 찾기
        if (p->key == x) {
            printf("같은키가 있습니다\n");
            return p;
        }
        // 같은 키가 있으면 함수 종료
        
        else if (p->key < x){
            p = p->right;
        }else{
            p = p->left;
        }
    }
    // 부모를 찾는 목적 !!


    //새 노드 할당
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = x;
    //단말노드이기 때문에 NULL을 채워줌
    newNode->left = newNode->right = NULL;

    //parent의 자식으로 새 노드 붙이기
    if (parent != NULL){
        if (parent->key < newNode->key){
            parent->right = newNode;
        }else{
            parent->left = newNode;
        }
    }
    return newNode;
};
//삭제 3가지 방법
//차수가 0인경우, 1인경우, 2인경우
//0인경우 = 해당 노드만 free시키고 노드로 향하는 parent 포인터만 NULL값으로 교체
Node* deleteBST(Node *root, char x){
    Node *p = root;
    Node* parent = NULL;
    //찾는 노드가 없거나 키를 찾은것도 아닌 동안은 계속 돌아야함
    while ((p!=NULL) && (p->key != x)){
        parent = p;
        if (p->key < x){
            p = p->right;   
        }else{
            p = p->left;
        }
    }
    if (p==NULL){
        printf("찾는 노드가 없습니다.\n");
        //찾는데 실패해 루트노드 그대로 반환
        return root;
    }
    // 차수가 0
    // 왼쪽도 널, 오른쪽도 널이면 차수가 0임
    if (p->left==NULL && p->right==NULL){
        // 현재 노드가 루트노드인데 차수가 0인 경우와
        // 루트가 아닌데 차수가 0인 경우로 나눠야함
        if(parent==NULL){
            root = NULL;
        }else{
            if (parent->left==p){
                parent->left = NULL;
            }else{
                parent->right = NULL;
            }
        }
    } 
    //차수가 1
    else if (p->left==NULL || p->right == NULL){
        // 왼쪽이랑 오른쪽중 값이있는걸 child에 넣음
        Node *child = (p->left != NULL) ? p->left : p->right;
        if (parent == NULL){
            root = child;
        }else{
            if (parent->left==p){
                parent->left = child;
            }else{
                parent->right = child;
            }
        }
    }else{//차수가 2
        Node *succ_parent = p;  // succ_parent = 18 , p = 18
        Node *succ = p->right;   // succ = 21
        while (succ->left != NULL){  // 19까지 내려감
            succ_parent = succ;   // succ_parent = 21
            succ = succ->left;    // succ = 19
        }
        p->key = succ->key;    // p -> key = 19 (위로 올렸음)
        if (succ_parent->left == succ){   // 만약에 21의 왼쪽 노드가 19라면,
            succ_parent->left = succ->right;  // 19를 NULL로 바꾸기 
        }else{          
            succ_parent->right = succ->right;}  // 21의 오른쪽노드가 19면, 19를 NULL로 바꾸기
        p = succ;  // p를 free로 없애기 위해 succ의 주소값을 할당해준다. 
    }
    free(p);
    return root;
};
void Inorder(Node * root){
    if (root == NULL){
        return;
    }
    Inorder(root->left);
    printf("%c", root->key);
    Inorder(root->right);
}
int main()
{
    Node* root = insertBST(NULL, 'D');
    insertBST(root, 'I');
    insertBST(root, 'F');
    insertBST(root, 'A');
    insertBST(root, 'G');
    insertBST(root, 'C');
    Inorder(root);
    printf("\n");
    return 0;
}