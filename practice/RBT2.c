// 
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));    // rbtree 1개 만큼의 동적 메모리를 할당시키고 void 형태로 반환되기 때문에 (rbtree *)로 rbtree 크기만큼의 주소로 바꾼다.
                                                      // 만들어진 포인터 p는 생성된 rbtree 1개의 시작 주소값을 가지게 된다.(rbtree 1개의 시작 주소를 가르키게 된다.)
  // rbtree의 nil에 node_t만큼의 메모리를 동적으로 할당시킨다. 
  p->nil = (node_t *)calloc(1, sizeof(node_t));       // 이 때 nil의 color를 제외하곤 나머지 필드들, 즉 key, parent, left, right은 사용할 일이 없으므로 따로 값을 넣지 않아도 된다.
  p->root = p->nil;                                   // 처음 root의 값은 NULL이다. 하지만 NULL 대신 nil을 쓰기로 하였고, nil을 사용하면 color를 편하게 지정할 수 있으므로 nil을 사용하겠다.(nil->color = RBTREE_BLACK)
  p->nil->color = RBTREE_BLACK;                       // 레드 블랙 트리의 특징에 따라서 nil의 color는 black이다.
  
  // TODO: initialize struct if needed
  return p;
}

// [회전_left]
// rotation은 β, parent, x <-> y 순서로 주소 교환이 이루어진다.
void left_rotate(rbtree *t, node_t *x) {              // rbtree라는 하나의 구조체를 가르키는 포인터 t, 하나의 노드를 가르키는 포인터 x
  node_t *y = x->right;
  // 1. 한 단계 올라가는 y의 왼쪽 자식 β를, 한 단계 내려가는 x의 오른쪽 자식으로 옮기기
  x->right = y->left;                                 // 위로 올리고자 하는 y의 왼쪽 자식 혹은 서브 트리 β를, 한 단계 내려가는 x의 오른쪽 자식으로 붙여야 한다.
                                                      // NIL이라면 NIL이 그냥 x의 right로 붙는다.  
                                              
  // 2. 옮기고자 하는 β가 NIL이 아니라면 부모 설정해주기
  if (y->left != t->nil) {                            // NIL이라면 β의 부모를 설정할 필요가 없지만, NIL이 아니라면 부모를 설정해줘야 한다.
    y->left->parent = x;                              // y->left가 β이고, 이 β의 부모를 x로 바꿔주면 된다.
  }
  
  // 3. y의 부모가 원래는 x였다. y의 부모를 x의 부모(parent)로 바꾼다.
  y->parent = x->parent;                
  
  // 4. x의 부모의 왼쪽 혹은 오른쪽 자식에 y가 오도록 한다.
  if (x->parent == t->nil) {                          // x의 부모가 만약 NIL이라면, 즉 없었다면 x 자체가 root 노드였다는 뜻이다. 따라서 rbtree의 root를 y로 바꿔준다.
    t->root = y;
  } else if (x == x->parent->left) {                  // (if문에서 걸러지지 않았다면 parent가 있다는 것) x가 부모의 왼쪽 자식이었다면, 부모의 왼쪽 자식을 y로 바꾼다.
    x->parent->left = y;
  } else {                                            // (x == x->parent->right)를 만족한다면, 즉 x가 부모의 오른쪽 자식이라면, 부모의 오른쪽 자식을 y로 바꾼다.
    x->parent->right = y;
  }  
  
  // 5. 한 단계 올라간 y의 왼쪽 자식이 한 단계 내려간 x가 되도록 한다.
  y->left = x;                          
  
  // 6. 5번과 연관지어, 한 단계 내려간 x의 부모가 한 단계 올라간 y가 되도록 한다.
  x->parent = y;
}

// [회전_right]
// left_rotation을 반대로 바꾸면 된다.
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  
  x->left = y->right;
  
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  
  y->parent = x->parent;
  
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x->parent->right == x) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }
  
  y->right = x;
  x->parent = y;
}

// [동적 메모리 할당 해제]
// [동적 메모리 할당된 노드들의 메모리 할당 해제]
void postorder_delete_rbtree(rbtree *t, node_t *x) {
  if (x != t->nil) {
    postorder_delete_rbtree(t, x->left);
    postorder_delete_rbtree(t, x->right);
    free(x);
  }
}

// [동적 메모리 할당 해제]
// [동적 메모리 할당된 트리의 nil 노드와 트리의 메모리 할당 해제]
void delete_rbtree(rbtree *t) {
  postorder_delete_rbtree(t, t->root);
  free(t->nil);
  free(t);
  // TODO: reclaim the tree nodes's memory
}

// [삽입_insert_fixup]
// 일단 새로 삽입할 노드 z가 삽입되고 나서 fixup이 시작된다.
// 새로운 노드 z 삽입 시 특성 2, root는 black이다. 혹은 특성 4, red가 연속해서 나오면 안된다를 위반할 수 있으니 이를 보완해줘야 한다.
void rbtree_insert_fixup(rbtree *t, node_t *z) {
  // <특성 4를 위반할 경우>
  while (z->parent->color == RBTREE_RED) {            // z의 parent가 red가 아닐 때까지 반복해줘야 특성 4를 벗어날 수 있다.
  
    // a. z의 부모가 조부모의 왼쪽 자식이라면
    if (z->parent == z->parent->parent->left) {       
      node_t *uncle = z->parent->parent->right;       // z의 부모의 부모, 즉 조부모의 오른쪽 자식이 uncle일 것이다.
      
      // 1. 경우 1에 해당하는 것으로, uncle이 red라면 색깔 조정을 하는 것만으로 특성 4 위반을 보완할 수도 있다.
      if (uncle->color == RBTREE_RED) {               
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;                        // 해당 구역은, 즉 해당 z를 root로 하는 트리는 처리했으니 위로 올라가서 다시 부모를 보고, 정확히 말하자면 while 조건을 따져보고 red라면 또 특성 4를 위반하게 되니 다시 보완해야 한다
        
      // 2. 경우 2, 3에 해당하는 것으로, uncle이 black이라면 z, parent, parent의 parent가 linear인지 혹은 triangle인지 따져보고 회전 및 색깔 조정을 해줘야 한다.
      } else {
        
        // triangle이라면 linear로 만들어줘야 한다.
        if (z == z->parent->right) {                  // 만약 z가 부모의 오른쪽 자식이었다면, 부모가 grand parent의 왼쪽 자식이었으므로 triangle에 해당한다.
          z = z->parent;                              // z의 부모를 축으로 삼아
          left_rotate(t, z);                          // 왼쪽으로 회전해야 한다.
        }
        
        // 바로 위의 if문을 거치고 밑의 코드를 거치게 되면 경우 3에 해당하는 것이다.
        // linear라면 색깔 조정 후 회전하면 된다. 
        // 색깔 조정은 해당 트리의 root가 될 z의 parent를 black으로, 사촌이 될 grand parent를 red로 해주면 된다.
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
      
    // b. z의 부모가 조부모의 오른쪽 자식이라면(z->parent == z->parent->parent->right)
    } else {                                          
      node_t *uncle = z->parent->parent->left;
      
        if (uncle->color == RBTREE_RED) {
          z->parent->color = RBTREE_BLACK;
          uncle->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            right_rotate(t, z);
          }
          
          z->parent->color = RBTREE_BLACK;
          z->parent->parent->color = RBTREE_RED;
          left_rotate(t, z->parent->parent);
        }
    }
  }
  
  // <특성 2를 위반할 경우>
  // while문을 한번도 거치지 않았다는 것은, 새로 삽입할 노드 z의 parent가 red가 아니었음을 말한다.
  // z는 이미 rbtree_insert 함수에서 root 노드가 되어 있을 것이다. 따라서 밑의 구문을 만나 black으로 바뀌게 된다.
  // 이 경우는 red인 z를 그냥 붙이기만 하면 돼서 특성 4를 위반하지 않는다.
  t->root->color = RBTREE_BLACK;
}

// [삽입_insert]
// 기존 Binary Search Tree의 Insert에서 변형
// 하지만 삽입할 자리를 찾는 과정은 동일하다. NULL이 nil 노드로 바뀐다는 점과, 새로 삽입할 노드는 항상 단말 노드가 되기 때문에 새로 삽입할 노드 z의 left, right가 NIL이 된다는 점이 다르다.
// 또한 새로 삽입할 노드 z가 레드 블랙 트리의 색깔 특성을 위반시킬 수 있으므로 따로 함수(rbtree_insert_fixup)를 호출하여 보완해줄 필요가 있다.
// insert하고자 하는 위치를 찾고자 최악의 경우 log n이 걸릴 수 있다.
// 그리고 while이나 for문 안에서 fixup이 이루어지는 것이 아니라 그냥 삽입이 끝나고 재조정을 위해 1번 이루어진다.
// 따라서 (log n)^2가 아니라 log n + log n = 2log n으로, 삽입의 시간 복잡도는 log n이다.
node_t *rbtree_insert(rbtree *t, const key_t key) {   // rbtree만큼의 크기를 가르키는 포인터 t와, 새로운 삽입할 노드의 키 값을 매개 변수로 받는다.
  node_t *x = t->root;                                // 삽입할 곳을 탐색해 나가는 포인터로, nil(NULL)을 찾으면 x를 삽입한다.
  node_t *y = t->nil;                                 // parent를 저장해두는 포인터이다. 우리는 이미 parent라는 변수를 사용하고 있으므로, 해당 포인터의 변수명을 y로 정했다.
  
  // 중복되는 값도 넣을 수 있어야 한다. 여기서는 오른쪽에 넣는 것으로 동작한다.
  while (x != t->nil) {                               // nil이 아니라면 계속 찾고, nil이라면 root부터 비어있다는 뜻이다. 그리고 while문을 한 번도 돌지 않기 때문에 x는 주소값으로 t->nil을 가지고 있게 된다.
    y = x;                                            // y에는 미리 x의 값(주소값)을 저장해놓는다. parent를 저장해놓기 위해서이다.
    if (x->key > key) {                               // 삽입하려는 값이 현재 x가 가르키는 노드의 키 값보다 작다면 왼쪽으로 탐색한다.
      x = x->left;
    } else {                                          // 삽입하려는 값이 현재 x가 가르키는 노드의 키 값보다 크다면 오른쪽으로 탐색한다.
      x = x->right;                                   // 추가로, 만약 (x->key == key)를 만족하여 중복되는 값이 있다면 else문을 만나 오른쪽에 넣도록 동작하게 된다.
    }
  }
  
  // while 문을 다 돌게 되면 x는 nil을 가르키게 되었다는 뜻으로, parent인 y에 새로운 노드인 z를 삽입하면 된다.
  // 새로운 노드 z 생성(할당)
  node_t *z = (node_t *)calloc(1, sizeof(node_t));    // node_t만큼 메모리에 동적으로 할당시키고 이 공간의 시작 주소를 포인터 z에 대입한다.                       
  z->key = key;
  z->parent = y;                                      // 삽입하려는 시점에는 x가 nil을, y는 nil의 parent를 가르키고 있다.(주소값을 가지고 있다)
  
  if (y == t->nil) {                                  // while문을 1번도 돌지 않았다면 y, 즉 parent는 nil일 것이다.
    t->root = z;                                      // while문을 1번도 돌지 않았다는 것은 애초에 트리가 비어있다는 뜻으로, 새로 생성한 노드 z가 rbtree의 root로 된다.
  } else if (z->key < y->key) {                       // 만약 새로 삽입할 z의 키 값이 parent의 키 값보다 작다면 parent(y)의 왼쪽에 넣어야 한다.
    y->left = z;
  } else {                                            // 만약 새로 삽입할 z의 키 값이 parent의 키 값보다 크다면 parent(y)의 오른쪽에 넣어야 한다.
    y->right = z;
  }
  
  // 새로 삽입할 노드 z는 단말 노드(리프 노드, leaf node)이기 때문에 항상 left, right는 nil이다.
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;                              // 새로 삽입할 노드의 색은 처음에 적색이다.
  rbtree_insert_fixup(t, z);                          // 특성이 위반되는 경우를 보완해주는 함수 호출
  
  // TODO: implement insert
  return t->root;                                     // 새로 생성하여 삽입한 노드의 주소를 가르키는 포인터(주소값)를 반환한다.
}  

// [노드 탐색]
// <while문으로 탐색>
// 계속된 함수 호출로 정적 메모리 영역 stack에 메모리를 할당하는 재귀에 비해 빠르다.
// 레드 블랙 트리의 탐색은 BST와 마찬가지로 균형을 이루고 있는 균형 이진 탐색 트리이다.
// 따라서 탐색은 최악의 경우 트리의 높이인 log n만큼 걸릴 수 있다.
// 그러므로 탐색의 시간 복잡도는 log n이다.
node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (t->root == t->nil) {
    return NULL;                                      // nil 개념을 사용하여 전체 코드를 짰으나, 탐색 테스트 케이스에서 NULL을 요구한다. 따라서 t->nil이 아니라 NULL로 작성하였다.
  }
  node_t *x = t->root;
  
  while (x != t->nil) {
    if (x->key > key) {
      x = x->left;
    } else if (x->key < key) {
      x = x->right;
    } else {
      return x;
    }
  }
  
  // TODO: implement find
  return NULL;
}

// [전체 t의 min 노드 탐색]
// 최솟값을 가진 노드 탐색 후 노드 반환
node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  
  while (x->left != t->nil) {
    x = x->left;
  }
  
  // TODO: implement find
  return x;
}

// [전체 t의 max 노드 탐색]
// 최댓값을 가진 노드 탐색 후 노드 반환
node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  
  while (x->right != t->nil) {
    x = x->right;
  }
  // TODO: implement find
  return x;
}

// [이식]
// 삭제하고자 하는 노드를 대체할 노드를 찾아, 삭제하고자 하는 노드에 대입할 동작을 수행하는 함수
// u 자리에 v 노드를 심기
// u의 부모가 가르키는 것이 v가 된다.
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {                          // u가 root 노드라면, 단순하게 v가 root 노드가 되면 된다.
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  
  v->parent = u->parent;                              // v의 parent가 u의 parent를 향하도록 한다. 이 때 u->parent가 t->nil이더라도 상관없다. rbtree, 그리고 이 코드에서는 nil 개념을 사용하니까
}

// [최솟값을 가진 노드 탐색]
// 내부(혹은 root) 노드로부터 최솟값을 찾는 함수로, 맨 왼쪽 자식을 가르키는 포인터를 반환한다.
// rbtree_erase 함수에서 오른쪽 서브트리의 최솟값을 찾아, 즉 successor(후계자)를 찾아서 z에 넣기 위함이다.
node_t *find_min_successor(rbtree *t, node_t *y) {
  while (y->left != t->nil) {                         // y의 왼쪽 자식이 nil이 아닐 때까지 계속 파고들어간다.
    y = y->left;                                      
  }
  
  // y의 왼쪽 자식이 nil이라면 멈추기 때문에(y가 nil이면 멈추는게 아니라) y는 유의미한 값을 가진 노드를 가르키는 주소값이다. 여기서는 successor라고 보면 된다. 
  return y;
}

// [삭제_fixup]
// 노드를 삭제하고 나면 특성 2, 4, 5를 위반했을 수도 있으니 재조정이 필요하다.
void rbtree_erase_fixup(rbtree *t, node_t *x) {
  node_t *w; // x의 형제 노드를 가리키는 w 포인터를 미리 선언한다.
  
  // succ => red 이면, while문 진입 x -> case1
  while (x != t->root && x->color == RBTREE_BLACK) {  // x가 root가 되면 단순히 그냥 검은색으로 바꾸면된다. 그리고 while문 아래는 x가 doubly black일 떄 이뤄진다.
    // doubly black인 x가 왼쪽 자식일 때
    // succ 가 root가 아닐 때, 
    if (x == x->parent->left) {
      w = x->parent->right;
      
      // <경우 1> : 경우 2, 3, 4로 위임 가능
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;                         // 회전을 끝내고 난 후에는 x->parent->right가 새로운 노드가 되고 밑의 if, else if, else 중 한 가지, 즉 경우 2, 3, 4의 한 가지로 위임된다.
      }
      
      // 위의 if문을 만나지 않았으므로, w->color == RBTREE_BLACK인 경우이다.
      // <경우 2> : 경우 1, 2, 3, 4로 위임 가능
      // x->parent로 짬 때리는 경우이다.
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;                        // x의 extra black을 x->parent로 넘긴다. 그러면서 w는 red가 된다.
        x = x->parent;                                // 새롭게 doubly black 혹은 red and black이 x->parent이 짬 맞아서 재조정을 진행하도록 위임한다.
      } else {
        
        // <경우 3> : 경우 4로 위임 가능
        if (w->right->color == RBTREE_BLACK) {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        
        // <경우 4> : 특성이 위반되는 것을 해결한다. 경우 4는 다른 경우로 위임되지 않고 위반을 해결(특성을 만족)한다.
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;                                  // 경우 4를 거치면 특성 위반이 해결되는 것이므로 x를 root로 설정하여 while문을 빠져나가도록 한다.
      }
    
    // doubly black인 x가 오른쪽 자식일 때
    } else {
      w = x->parent->left;
      
      // <경우 1>
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      
      // <경우 2>
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else {
        
        // <경우 3>
        if (w->left->color == RBTREE_BLACK) {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        
        // <경우 4>
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  
  //succ -> 검은색으로 case1
  x->color = RBTREE_BLACK;                            // x가 root 노드이거나, red and black이면 해당 코드를 만나서 black이 되고 특성들을 만족시키게 된다.
}

// [삭제]
// 노드를 삭제하는 데에 필요한 최악의 시간복잡도는 log n이다. 그리고 fixup을 할 때 걸리는 최악의 시간복잡도도 log n이다.
// for문이나 while문을 돌면서 fixup이 일어나는 것이 아니라 그냥 erase를 하고나면 fixup을 1번 하게 된다.
// 즉 노드 삭제 1번을 실행하면 fixup도 그냥 1번 실행된다.
// 따라서 시간복잡도는 (log n)^2가 아니라 log n + log n = 2log n이 되어 최종적으로 log n이라고 할 수 있다.
// 따라서 레드 블랙 트리의 삭제 시간 복잡도는 log n이다.
int rbtree_erase(rbtree *t, node_t *z) {              // 원래는 z가 아닌 p였으나, 해당 전체 코드에서 새로운 노드 혹은 삭제하고자 하는 노드를 z라고 칭하기에 z로 바꾸었다.
  node_t *y = z;                                      // z는 우리가 삭제하고자 하는 노드의 주소값, y는 대체할 노드를 탐색한 후 그 대체할 노드를 가르키는 주소값(y는 z 혹은 successor)
  
  // 나중에 y의 color, 즉 우리가 삭제하고자 하는 노드에 들어갈(대체할), 실제로 삭제하는 노드의 색깔이다. 이는 레드 블랙 트리의 삭제 원리에 따라 블랙일 경우 문제가 되기에 미리 저장해둔다. 
  color_t y_original_color = y->color;                // 해당 코드에서는 나중에 y의 색깔이 z의 색깔로 되기 때문에 미리 저장해둬야 한다. 이 코드가 바뀌지 않으면, z 바로 자신이 실제로 삭제된다는 얘기이다. 그 경우는 자식 노드가 0개 혹은 1개일 때이므로 밑의 if문과 else if문을 말한다.
  
  node_t *x;                                          // x는 y의 자식으로, 여기서는 오른쪽 서브트리에서 가장 작은 값을 가진 노드를 탐색할 것이므로 y(successor)의 오른쪽 자식이다.
  
  if (z->left == t->nil) {                            // z의 자식이 오른쪽 자식 1개뿐이라면
    x = z->right;                                     // 그 z의 자식 x를
    rbtree_transplant(t, z, z->right);                // 그냥 z 자리에 심으면 된다.
  } else if (z->right == t->nil) {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  } else {
    y = find_min_successor(t, z->right);              // z의 오른쪽 자식에서, 즉 오른쪽 서브 트리에서 successor를 찾는다. 이는 z보다 크지만 오른쪽 서브 트리에서 가장 작은, z 바로 다음으로 큰 successor를 찾는 것이다.
    y_original_color = y->color;                      // 여기서는 이제 실제로 삭제될 노드 successor의 색을 저장해둬야 한다.
    x = y->right;                                     // y는 실제로 삭제될 거니까, y의 자리에 x가 대체자로 올라와야 한다.
    
    if (y->parent == z) {                             // 이건 왜 하는지 잘 모르겠다. transplant하면서 자연스럽게 부모가 바뀌지 않나?
      x->parent = y;                                  
    } else {
      rbtree_transplant(t, y, y->right);              // z에 y를 심기 전에, 먼저 y의 자리에 y의 자식을 심는다. 그렇지 않으면 y는 자식이 딸린 채로 z에 심어지기 때문이다. 해당 코드를 실행하면 y 노드 1개만 딱 떼어져 있다고 생각하면 편할 듯 하다.
      y->right = z->right;                            // z에 y를 심기 전에 행하는 사전 작업이다.
      y->right->parent = y;                           // 우리가 방금 y 하나만 딱 둥그러니 떼놨기 때문에 y의 자식과 그 자식의 부모를 y로 향하게 설정해줘야 한다.
    }                                                 // 근데 왜 오른쪽 자식만 먼저 하지?
    
    rbtree_transplant(t, z, y);                       // 이제 z에 y를 심는다. 정확히는 z의 부모는 이제 z가 아니라 y를 가르키게 된다는 것이다.
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;                              // y는 z로 올라오고, y의 데이터는 유지한 체 색깔만 바뀐다. 즉 z의 색깔이 아니라 y의 색깔이 삭제되는 것이다. z에 y가 이식된 후, z의 색깔을 y에 심었으니까.
  }
  
  // 만약 위에서 삭제한 색이, 즉 실제로 삭제되는 노드인 y의 색이 red였다면 레드 블랙 트리 특성 5가지 중에서 위반되는 것은 없다.
  // 허나 black이었다면 위반하는 항목들이 있기 때문에 재조정이 필요하다. 단순히 생각하면 black을 삭제하여 red, red끼리 만나거나 특정 노드에서 리프 노드까지 만나는 black의 갯수가 달라질 수 있다.
  if (y_original_color == RBTREE_BLACK) {
    rbtree_erase_fixup(t, x);                         // 대체하는 노드 y의 자식인 x가 y의 자리로 올라오면서 이 x에 extra black을 부여한다. 이 extra black을 처리하면서 재조정하는 것이 fixup의 원리이므로 x를 인자로 넣는다.
  }
  
  free(z);                                            // z가 삭제되고 그 자리에 y가 대체하는 것이므로 z를 free시켜준다.
  // TODO: implement erase
  return 0;
}

// [중위 순회]
void inorder_rbtree_to_array(const rbtree *t, node_t *x, key_t *arr, int *idx, const size_t n) {
  if (x == t->nil) {
    return;
  }
  
  inorder_rbtree_to_array(t, x->left, arr, idx, n);
  if (*idx < n) {
    arr[(*idx)++] = x->key;                           // *idx는 0, 1, 2, 3...이다. 그리고 후위 연산자 ++이므로 0부터 인덱스가 시작된다.
  } else {
    return;
  }
  inorder_rbtree_to_array(t, x->right, arr, idx, n);
}

// [오름차순으로 배열에 값 저장]
// rbtree를 가르키는 t 값(주소값)은 변하면 안되므로 const
// arr는 inorder 함수를 거칠 때 마다 변할 수 있으므로 const 없이
// n은 arr의 사이즈로 변하면 안되므로 const를 붙여준다. size_t는 unsigned int로, 어차피 배열의 사이즈 n은 양수일테니 size_t로 선언
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *x = t->root;
  if (x == t->nil) {
    return 0;
  }
  int cnt = 0;
  int *idx = &cnt;
  inorder_rbtree_to_array(t, x, arr, idx, n);
  
  // TODO: implement to_array
  return 0;
}

// [중위 순회] : 확인용
// 중위 순회로 트리를 탐색하며 오름차순으로 키 값을 출력하고 각 노드에 해당하는 색깔을 같이 출력한다.
void inorder_tree_work(rbtree *t, node_t *x) {
  if (x != t->nil) {
    inorder_tree_work(t, x->left);
    printf("색깔 : %d, 키 값 : %d\n", x->color, x->key);
    inorder_tree_work(t, x->right);
  }
}