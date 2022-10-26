#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  // 새로운 rbtree 생성
  // rbtree 1개 만큼의 동적 메모리를 할당시키고 void 형태로 반환되기 때문에 (rbtree *)로 rbtree 크기만큼의 주소로 바꾼다.
  // 만들어진 포인터 p는 생성된 rbtree 1개의 시작 주소값을 가지게 된다.(rbtree 1개의 시작 주소를 가리키게 된다.) 

  // rbtree의 nil에 node_t만큼의 메모리를 동적으로 할당시킨다. 

  //node_t* new_node = (node_t *)calloc(1, sizeof(node_t));
  p->nil = (node_t *)calloc(1, sizeof(node_t));       // 이 때 nil의 color를 제외하곤 나머지 필드들, 즉 key, parent, left, right은 사용할 일이 없으므로 따로 값을 넣지 않아도 된다.
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  //new_node->color = RBTREE_BLACK;                    // 레드 블랙 트리의 특징에 따라서 nil의 color는 black이다.
  // p->nil = new_node;
  // p->root = new_node;                                   // 처음 root의 값은 NULL이다. 하지만 NULL 대신 nil을 쓰기로 하였고, nil을 사용하면 color를 편하게 지정할 수 있으므로 nil을 사용하겠다.(nil->color = RBTREE_BLACK)
  // assert(t->root == NULL);
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory


  free(t);
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right; // y에 임시 저장
  x->right = y->left; // x의 right가 비었으므로 y의 left와 연결
  if(y->left != t->nil){ // nil은 부모를 지정해줄 필요 없음 ?????
    y->left->parent = x; // 부모까지 지정해줌으로써 완전 연결
  }
  y->parent = x->parent; // y의 부모를 x의 부모와 연결
  if (x->parent == t->nil){
    t->root = y; // x의 부모가 nil이라면 root는 y
  }
  else if (x == x->parent->left){
    x->parent->left = y; // x가 부모로부터 왼쪽 자식이었다면, y도 왼쪽 자식
  }
  else{
    x->parent->right = y;
  }

  y->left = x; // x를 y의 왼쪽으로 넣는다
  x->parent = y;

}

void right_rotate(rbtree *t, node_t *y){
  node_t *x = y->left; // x에 임시 저장
  y->left = x->right; // y의 left가 끊어져도 되므로 x의 right와 연결
  if(x->right != t->nil){ // nil은 부모를 지정해줄 필요 없음 ?????
    x->right->parent = y; // 부모까지 지정해줌으로써 완전 연결
  }
  x->parent = y->parent; // x의 부모를 y의 부모로 바꿈
  if (y->parent == t->nil){
    t->root = x; // y의 부모가 nil이라면 root는 x
  }
  else if (y == y->parent->left){
    y->parent->left = x; // x가 부모로부터 왼쪽 자식이었다면, y도 왼쪽 자식
  }
  else{
    y->parent->right = x;
  }

  x->right = y; // x를 y의 왼쪽으로 넣는다
  y->parent = x;
}

// node_t *rbtree_insert(tree1, 50)
node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert

  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));
  node_t *y = t->nil;
  node_t *x = t->root;
  new_node->key = key;
  // tree의 root부터 탐색
  // 부모를 찾아라 !!!
  while(x != t->nil){
    y = x; // 현재 노드를 부모 노드로, 현재 노드는 삽입 하면 자식 노드가 되니까

    // 입력 받은 값이 더 크면, key의 오른쪽
    if (key < x->key){
      x = x->left;
    }else{
      x = x->right;
    }
  }

  // 부모 주소 전달
  new_node->parent = y;

  if (y == t->nil){
    t->root = new_node;
  }
  else if(key < y->key){
    y->left = new_node;
  }
  else{
    y->right = new_node;
  }

  new_node -> left = t -> nil;
  new_node -> right = t -> nil;
  new_node -> color = RBTREE_RED;

  rbtree_insert_fixup(t, new_node);

  return new_node;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  // <특성 4를 위반할 경우> z : 현재 노드
  while (z->parent->color == RBTREE_RED) {
    // z의 parent가 red가 아닐 때까지 반복해줘야 특성 4를 벗어날 수 있다.
    if (z->parent == z->parent->parent->left){
      // 부모 노드가 조상 노드의 왼쪽 자식일 때,
      node_t *uncle = z->parent->parent->right;
      if (uncle->color == RBTREE_RED){
        // 삼촌 노드도 RED 일 때,
        // 부모, 삼촌 => Black, 할아버지 => Red
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        
        // 할아버지(RED)부터 재확인
        z = z->parent->parent;
        }

      // 삼촌이 검은 노드일 때,
      else{
        
        if (z == z->parent->right){
        // 꺾인 경우 => 왼쪽으로 회전
        z = z -> parent; // z는 z의 부모노드 ..... ?????
        left_rotate(t, z);
        }
        // 색상 바꾸기
        z -> parent -> color = RBTREE_BLACK;
        z -> parent -> parent -> color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else{
      // 부모 노드가 조상 노드의 오른쪽 자식일 때, 삼촌은 조상의 왼쪽 자식
      node_t *uncle = z->parent->parent->left;
      if (uncle->color == RBTREE_RED){
        // 삼촌 노드도 RED 일 때,
        // 부모, 삼촌 => Black, 할아버지 => Red
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        
        // 할아버지(RED)부터 재확인
        z = z->parent->parent;
        }

      // 삼촌이 검은 노드일 때,
      else{        
        if (z == z->parent->left){
        // 꺾인 경우 => 왼쪽으로 회전
        z = z -> parent; // z는 z의 부모노드 ..... ?????
        right_rotate(t, z);
        }
        // 색상 바꾸기
        z -> parent -> color = RBTREE_BLACK;
        z -> parent -> parent -> color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t* p = t->root;
  while((p!= t->nil) && (p->key != key)){
    p->parent = p;
    if(key < p->key){
      p = p->left;
    }else{
      p = p->right;
    }
  }
  if (p == t->nil){
    printf("찾는 노드가 없습니다.\n");
    // 찾는데 실패해 루트노드 그대로 반환
    return NULL;
  }
  printf("%d\n",p->key);
  return p;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  // u가 root 일 때,
  if(u->parent == t->nil){
    t->root = v;
  }
  else if(u == u->parent->left){
    u->parent->left = v;
  }
  else{
    u->parent->right = v;
  }
  v->parent = u->parent;
}

int rbtree_erase(rbtree *t, node_t *z){
  printf("=========000000000000000000");
  // node_t *y = z;
  // color_t y_original_color = y->color;
  // node_t *x;

  // if (z->left == t->nil){
  //   printf("11111");
  //   x = z->right;
  //   rbtree_transplant(t, z, z->right);
  // }
  // else if(z->right == t->nil){
  //   printf("22222");
  //   x = z->left;
  //   rbtree_transplant(t, z, z->left);
  // }
  // else{
  //   printf("33333");
  //   y = tree_minimum(t, z->right);
  //   y_original_color = y->color;
  //   x = y->right;
  //   if(y->parent == z){
  //     x->parent = y;
  //   }
  //   else{
  //     rbtree_transplant(t, y, y->right);
  //     y->right = z->right;
  //     y->right->parent = y;
  //   }
  //   rbtree_transplant(t, z, y);
  //   y->left = z->left;
  //   y->left->parent = y;
  //   y->color = z->color;
  // }
  // if(y_original_color == RBTREE_BLACK){
  //   // rbtree_erase_fixup(t, x);
  // }
  // free(z);
  return 0;
}

void rbtree_erase_fixup(rbtree* t, node_t *x){
  node_t* w;
  while(x != t->root && x->color == RBTREE_BLACK){
    if (x == x->parent->left){
      w = x->parent->right;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if(w->left->color==RBTREE_BLACK && w->right->color==RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->right->color == RBTREE_BLACK){
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->right->color = RBTREE_BLACK;
      left_rotate(t, x->parent);
      x = t->root;
      }
    }
    else{
      w = x->parent->left;
      if(w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if(w->right->color==RBTREE_BLACK && w->left->color==RBTREE_BLACK){
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if(w->left->color == RBTREE_BLACK){
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
      w->color = x->parent->color;
      x->parent->color = RBTREE_BLACK;
      w->left->color = RBTREE_BLACK;
      right_rotate(t, x->parent);
      x = t->root;
    }  
    }
  }
  x->color = RBTREE_BLACK;
}

node_t* tree_minimum(rbtree *t, node_t* x){
  while(x->left != t->nil){
    x = x->left;
  }
  return x;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}

void rbtree_display(rbtree *t, node_t *node) {
  node_t *z = node;
  printf("Node [%d][%d] > ", z->key, z->color);
  if (z->left != t->nil) printf("Left [%d][%d] ", z->left->key, z->left->color);
  if (z->right != t->nil) printf("Right [%d][%d] ", z->right->key, z->right->color);
  printf("\n");
  if (z->left != t->nil) rbtree_display(t, z->left);
  if (z->right != t->nil) rbtree_display(t, z->right);
}

// int rbtree_erase(rbtree *t, node_t *p) {
//   // TODO: implement erase
  
//   node_t *y = p;
//   color_t y_original_color = y->color;

//   node_t *x;

//   // 자식 노드가 0개
//   if((p->left==t->nil)&&(p->right==t->nil)){
//     rbtree_transplant(t, p, t->nil);
//   }
//   // 자식 노드가 1개
//   else if((p->left==t->nil)||(p->right==t->nil)){
//   node_t* child = (p->left != t->nil) ? p->left : p->right;
//   rbtree_transplant(t, p, child);
//   }
//   // 자식 노드가 2개
//   else{
//     node_t *succ_parent = p;

//     // succ = 우측 서브트리 중 최솟값 노드
//     node_t *succ = p->right;
//     while(succ->left != t->nil){
//       succ_parent = succ;
//       succ = succ->left;
//     }
    
//     p->key = succ->key;
//     if(succ_parent->left == succ){
//       succ_parent->left = succ->right;
//     }else{
//       succ_parent->right = succ->right;
//     }
    
//     x = succ->right;
//     y_original_color = succ->color;
//     p = succ;
//   }
//   free(p);

//   if (y_original_color == RBTREE_BLACK){
//     rbtree_erase_fixup(t, x);
//   }
//   return 0;
// }