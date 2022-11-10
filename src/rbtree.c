#include "rbtree.h"
#include <stdlib.h>
#include <stdio.h>

// 트리 생성 및 초기화
rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree)); // rbtree 메모리 공간 할당

  // nil 노드 (sentinel)
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;

  return p;
}

// 노드 삭제 - 후위 순회 : 자식 노드부터 차례로 삭제
void delete_node(rbtree *t, node_t *cur_node)
{

  // 현재 노드가 nil 노드이면, 종료
  if (cur_node == t->nil)
    return;
  
  // 현재 노드의 왼쪽 끝까지 탐색 - 값을 갖는 마지막 노드까지
  if (cur_node->left != t->nil)
    delete_node(t, cur_node->left);

  // 오른쪽 노드 탐색
  if (cur_node->right != t->nil)
    delete_node(t, cur_node->right);

  // 현재 노드 메모리 공간 할당 해제
  free(cur_node);

  return;
}


// 트리 삭제 - reclaim the tree nodes's memory
void delete_rbtree(rbtree *t)
{

  delete_node(t, t->root);   // 루트 노드부터 탐색

  free(t->nil);  // nil 노드 메모리 해제

  free(t);  // 트리 메모리 해제
}

// 좌회전 : left-converted 
void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;
  
  x->right = y->left;  // y의 왼쪽 자식이 x의 오른쪽 자식이 됨

  if (y->left != t->nil)   // y의 왼쪽 자식이 nil노드가 아니라면, 
    y->left->parent = x;  // 부모 설정 (nil 노드는 부모 없음)
  
  y->parent = x->parent;  // x의 부모가 y의 부모가 됨

  if (x->parent == t->nil)   // x의 부모가 nil 노드라면,
    t->root = y;  // (x가 루트노드라면) 루트노드는 y가 됨
    
  else if (x == x->parent->left)   // x가 왼쪽 자식이었다면, 이제는 y가 왼쪽 자식 
    x->parent->left = y;

  else    // x가 오른쪽 자식이었다면, 이제는 y가 오른쪽 자식
    x->parent->right = y;


  y->left = x;  // x는 y의 왼쪽 자식, y는 x의 부모
  x->parent = y;
}

// 우회전 : right-converted - 좌회전에서 (right <=> left) 바꿈 
void right_rotate(rbtree *t, node_t *y)
{
  node_t *x = y->left;
  
  y->left = x->right;

  if (x->right != t->nil)
    x->right->parent = y;
  
  x->parent = y->parent;
  if (y->parent == t->nil)
    t->root = x;
  
  else if (y == y->parent->left)
    y->parent->left = x;

  else
    y->parent->right = x;
  
  x->right = y;
  y->parent = x;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  // 부모 노드의 색이 Red이면 반복 (Red가 아닐 때까지) - 조건4 : 부모 red이면, 자식 black : 위반
  while (z->parent->color == RBTREE_RED) {
    
    // 부모 노드가 조상 노드의 왼쪽 자식일 때,
    if (z->parent == z->parent->parent->left) {
     
      // 삼촌 노드는 조상 노드의 오른쪽 자식
      node_t *uncle = z->parent->parent->right;

      // #Case 1
      // 부모, 삼촌 = Red 일 때,
      if (uncle->color == RBTREE_RED) {
        // 부모, 삼촌 => Black, 할아버지 => Red
        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;

        z = z->parent->parent;  // 할아버지(Red)부터 재확인
      }

      // 부모 = Red, 삼촌 = Black 일 때
      else { // #Case 2 : 회전시켜 #Case 3으로 만들기        
        if (z == z->parent->right) {  // 왼쪽에서 꺾인 경우 => 부모 노드를 기준으로 좌회전
          z = z->parent;
          left_rotate(t, z);
        }

        // #Case 3 : 색상 변환 후, 회전
        // 부모 노드 => Black, grandparent 노드 => Red

        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        // grandparent 노드 기준으로 우회전
        right_rotate(t, z->parent->parent);
      }
    }

  // 부모 노드가 조상 노드의 오른쪽 자식일 때, 
    else {
      
      node_t *uncle = z->parent->parent->left; // 삼촌은 조상의 왼쪽 자식

      // 위와 동일 (왼쪽 <=> 오른쪽)
      if (uncle->color == RBTREE_RED) {

        z->parent->color = RBTREE_BLACK;
        uncle->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;

        z = z->parent->parent;
      }

      else {
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

  t->root->color = RBTREE_BLACK;        // 루트 노드의 색은 항상 Black
}

// implement insert
node_t *rbtree_insert(rbtree *t, const key_t key)
{

  node_t *new_node = (node_t *)calloc(1, sizeof(node_t));   // 노드 생성
  new_node->key = key;


  node_t *parent = t->nil;             // parent : 생성된 노드의 삽입 지점
  node_t *cur_node = t->root;
  
  
  
  while (cur_node != t->nil) {        // x가 루트노드부터 시작해서 말단노드가 될 때까지 while문 반복
    parent = cur_node;                // 루트 노드가 nil노드라면 (비어있는 트리) while문 건너뜀

    if (key < cur_node->key)          // 현재 노드의 키 값보다 작다면 왼쪽으로
      cur_node = cur_node->left;

    else                              // 현재 노드의 키 값보다 크다면 오른쪽으로
      cur_node = cur_node->right;
  }

  new_node->parent = parent;

  if (parent == t->nil)               // 부모 노드가 nil 노드 라면, 루트
    t->root = new_node;
  
  else if (key < parent->key)         // 부모 노드보다 키 값이 작다면, 왼쪽 자식
    parent->left = new_node;
  

  else                                // 부모 노드보다 키 값이 크다면, 오른쪽 자식
    parent->right = new_node;
  
  new_node->left = t->nil;            // 새로운 노드는 트리의 말단에 삽입 - 왼쪽, 오른쪽 자식은 nil노드
  new_node->right = t->nil;
  new_node->color = RBTREE_RED;

  // rbtree 특성 복구하기 위해 호출
  rbtree_insert_fixup(t, new_node);

  return new_node;
}

// 해당 key값을 갖는 노드 찾기 (implement find)
node_t *rbtree_find(const rbtree *t, const key_t key)
{
  node_t *p = t->root;
  
  // root노드에서 말단 노드로 갈 때까지 반복하다가 key 값이 나오면 멈춤
  while ((p != t->nil) && (p->key != key)) {
    if (key < p->key)
      p = p->left;
    else
      p = p->right;  
  }

  // 끝까지 탐색했는데, 찾는 값이 없다면 NULL 반환
  if (p == t->nil)
    return NULL;

  return p;
}

// 트리 내 최솟값 노드 반환 - 왼쪽 탐색
node_t *rbtree_min(const rbtree *t)
{
  node_t *ptr = t->root;

  if (ptr != t->nil) {
    while (ptr->left != t->nil)
      ptr = ptr->left;  
  }
  return ptr;
}

// 트리 내 최댓값 노드 반환 - 오른쪽 탐색
node_t *rbtree_max(const rbtree *t)
{
  node_t *ptr = t->root;

  if (ptr != t->nil) {
    while (ptr->right != t->nil)
      ptr = ptr->right;
  }
  return ptr;
}

// 노드 u 자리에 노드 v 이식
void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  
  else if (u == u->parent->left)
    u->parent->left = v;
  
  else
    u->parent->right = v;
  
  v->parent = u->parent;
}

void rbtree_erase_fixup(rbtree *t, node_t *x)
{
  node_t *w;

  while (x != t->root && x->color == RBTREE_BLACK) { // Black 노드인 경우에만 재조정
    if (x == x->parent->left) {     // x가 왼쪽 자식인 경우
      w = x->parent->right;         // w는 형제 노드
      if (w->color == RBTREE_RED) { // #Case 1 : 형제노드 Red
        w->color = RBTREE_BLACK;    // 형제 노드 => Black, 부모 노드 => Red 색 변환 후, 
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);  // 부모 노드 기준으로 좌회전 (Black 하나 넘김)
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) { // #Case 2 : 형제 노드 Black, 형제 노드의 자식 모두 Black
        w->color = RBTREE_RED;                                                 //  => 형제 노드 -> Red 색 변환 후, 부모 노드부터 재조정
        x = x->parent;                                                              
      }
      else {                                      
        if (w->right->color == RBTREE_BLACK) { // #Case 3 : 형제 노드 Black, 형제 노드의 오른쪽 자식 - Black, 왼쪽 자식 - Red
          w->left->color = RBTREE_BLACK;       //  => 색변환 후, 우회전 -> #Case 4가 됨
          w->color = RBTREE_RED;     
          right_rotate(t, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;          // #Case 4 : 형제 노드 Black, 형제의 오른쪽 자식 - Red 
        x->parent->color = RBTREE_BLACK;      // (자식 둘 다 Red 또는 왼-Black, 오-Right)
        w->right->color = RBTREE_BLACK;       //  => 형제 노드의 색은 부모 노드의 색으로 변환, 부모노드와 자식노드 모두 Black으로 변환 후 좌회전
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else {                                    // x가 오른쪽 자식인 경우 (left<=>right)
      w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else {
        if (w->left->color == RBTREE_BLACK) {
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

int rbtree_erase(rbtree *t, node_t *z)
{
  node_t *y = z;
  color_t y_original_color = y->color;  
  node_t *x;

  if (z->left == t->nil) {              // 왼쪽 자식 노드가 nil 일 때 (자식 노드가 1개 또는 0개)
    x = z->right;
    rbtree_transplant(t, z, z->right);  // 오른쪽 자식 노드를 현재 노드의 위치로 옮김
  }
  else if (z->right == t->nil) {        // 오른쪽 자식 노드가 nil 일 때 (자식 노드가 1개 또는 0개)
    x = z->left;
    rbtree_transplant(t, z, z->left);   // 왼쪽 자식 노드를 현재 노드의 위치로 옮김
  }
  else {                                // 자식 노드가 2개일 때
    y = z->right;                       

    while (y->left != t->nil)           // 오른쪽 서브트리의 최솟값을 갖는 노드(y) 찾기
      y = y->left;                      


    y_original_color = y->color;        // 실제로 삭제되는 노드의 색 저장
    x = y->right;                       // x : successor. y가 삭제 되고, y의 위치로 옮겨짐.
    
    if (y->parent == z)
      x->parent = y;                    // succ의 부모는 y 
    
    else {
      rbtree_transplant(t, y, y->right); // y가 삭제 되고, y의 오른쪽 자식이 y의 위치로 옮겨짐.
      y->right = z->right;                
      y->right->parent = y;
    }
    rbtree_transplant(t, z, y);         // y가 z 노드의 위치로 옮겨짐
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (y_original_color == RBTREE_BLACK) { //삭제된 노드의 색이 Black일 때,             
    rbtree_erase_fixup(t, x);             // rbtree 조건 만족시키기 위한 재조정
  }

  free(z);                                // 노드 삭제

  return 0;
}

// 중위 탐색 - 배열에 노드의 값 오름차순으로 삽입
int AddToArray(const rbtree *t, node_t *cur_node, key_t *arr, int i)
{
  if (cur_node == t->nil)                       //현재 노드가 말단 노드이면 index 반환
    return i;

  if (cur_node->left != t->nil)                 //왼쪽 서브트리 탐색
    i = AddToArray(t, cur_node->left, arr, i);

  arr[i] = cur_node->key;                       // 현재 노드의 키 값 배열에 삽입
  i++;                                          // index +1

  if (cur_node->right != t->nil)                //오른쪽 서브트리 탐색
    i = AddToArray(t, cur_node->right, arr, i);

  return i;
}

// rbtree => array 변환
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  node_t *cur_node = t->root;

  AddToArray(t, cur_node, arr, 0);

  return 0;
}