#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)malloc(sizeof(rbtree));
  
  p->nil = (node_t *)malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  
  return p;
}

// 왼쪽 회전
void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right;
  
  x->right = y->left; // x의 오른쪽자식은 x의 오른쪽자식의 왼쪽자식 -> 자식을 가리키고
  if (y->left != t->nil) { // x의 오른쪽자식의 왼쪽자식이 nil이 아니면
    y->left->parent = x; // 부모를 가리킨다
  }
  
  y->parent = x->parent; // x의 오른쪽이 부모가리키는걸로 x의 부모가리키를 바꾼다. -> 회전이 완전히 된건아님

  if (x->parent == t->nil) { // x가 루트일때
    t->root = y; // 루트는 x의 오른쪽 자식이다.
  }
  else if (x == x->parent->left) { // x의 위치가 왼쪽에 있을 때 
    x->parent->left = y; 
  }
  else { // x의 위치가 오른쪽일 때
    x->parent->right = y;
  }

  y->left = x; // x를 y의 왼쪽으로 놓는다
  x->parent = y; // 윗줄과 세트
}

// 오른쪽 회전 -> 왼쪽과 같음
void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left; // x의 왼쪽 자식

  x->left = y->right; // x의 왼쪽 자식은 x의 왼쪽자식의 오른쪽 자식
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil) {
    t->root = y;
  }

  else if (x == x->parent->right) {
    x->parent->right = y;
  }

  else {
    x->parent->left = y;
  }

  y->right = x;
  x->parent = y;
}


void del_node(rbtree *t, node_t *n) {
  if (n != t->nil) {
    del_node(t, n->left);
    del_node(t, n->right);
    free(n);
    n = NULL;
  }
}

void delete_rbtree(rbtree *t) {
  del_node(t, t->root);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

void RB_INSERT_FIXUP(rbtree *t, node_t *new_node) {
  node_t *tmp;
  // 부모가 black / 반복문 거치지 않고 root를 black으로 변경 후 함수 종료  
  while (new_node->parent->color == RBTREE_RED) {
    if (new_node->parent == new_node->parent->parent->left) { // 왼쪽에 있을때
      tmp = new_node->parent->parent->right; // uncle 노드
      
      // case 1
      // 삽입된 red 노드의 부모도 red, 삼촌도 red일때
      if (tmp->color == RBTREE_RED) {
        new_node->parent->color = RBTREE_BLACK; // 부모 블랙으로
        tmp->color = RBTREE_BLACK; // 삼촌 블랙으로
        new_node->parent->parent->color = RBTREE_RED; // 할아버지 레드로
        // 할아버지에서 다시 확인 시작해주기 위해
        new_node = new_node->parent->parent;
      }
      // case2
      // uncle은 black 일때
      else {
        if (new_node == new_node->parent->right) { 
          new_node = new_node->parent; 
          left_rotate(t, new_node); // new_node 부모기준으로 왼쪽으로 회전
        }
      // case3
        new_node->parent->color = RBTREE_BLACK; 
        new_node->parent->parent->color = RBTREE_RED;
        right_rotate(t, new_node->parent->parent);  
      } 
    }
    else { // 오른쪽에 있을때 -> 왼쪽이랑 같은 로직
      tmp = new_node->parent->parent->left;

        // case 1
      if (tmp->color == RBTREE_RED) {
        new_node->parent->color = RBTREE_BLACK;
        tmp->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        new_node = new_node->parent->parent;
      }
        // case 2
      else {
        if (new_node == new_node->parent->left){
          new_node = new_node->parent;
          right_rotate(t, new_node);
        }
      // case 3
        new_node->parent->color = RBTREE_BLACK;
        new_node->parent->parent->color = RBTREE_RED;
        left_rotate(t, new_node->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK; // 루트가 레드일때 블랙으로 마지막으로 바꿔줘야함.
}

// 삽입
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new_node = malloc(sizeof(node_t));
  node_t *x = t->root; // x는 root
  node_t *y = t->nil; // y는 nil
  new_node->key = key;

  while (x != t->nil) {
    y = x;
    if (new_node->key < y->key) {
      x = x->left;
    }
    else {
      x = x->right;
    }
  }
  new_node->parent = y;

  if (y == t->nil) {  
    t->root = new_node;
  }
  else if (new_node->key < y->key) {
    y->left = new_node;
  }
  else {
    y-> right = new_node;
  }
  new_node->left = t->nil;
  new_node->right = t->nil;
  new_node->color = RBTREE_RED;
  RB_INSERT_FIXUP(t, new_node);

  return new_node;
}

// 찾기
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *tmp = t->root;

  while(tmp != t->nil && tmp->key != key) {
    if (tmp->key > key) {
      tmp = tmp->left;
    }
    else {
      tmp = tmp->right;
    }
  }
  if (tmp != t->nil) {
    return tmp;
  }
  
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *x = t->root;
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

node_t *node_min(const rbtree *t, node_t *n) {
  node_t *ptr;
  ptr = n;
  while(ptr->left != t->nil) {
    ptr = ptr->left;
  }
  return ptr;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *x = t->root;
  while (x->right != t->nil) {
    x = x->right;
  }
  return x;
}

void RB_TRANSPLANT(rbtree *t, node_t *u, node_t *v) { // 삭제될 노드의 자노드를 삭제위치에 대체시키는 함수
  v->parent = u->parent; // 자식노드의 부모가리키는 것이 삭제될 노드의 부모를 가리키는 것이 됨 

  if (u->parent == t->nil) { // 삭제될 노드의 부모가 닐이면
    t->root = v; // 대체되는 노드가 루트가 된다.
  }
  else if (u == u->parent->left) { // 삭제될 것이 왼쪽에 있으면 
    u->parent->left = v; // 왼쪽에
  }
  else { // 삭제될 것이 오른쪽에 있으면
    u->parent->right = v; // 오른쪽에
  }
}

void RB_ERASE_FIXUP(rbtree *t, node_t *target) {
  node_t *u;
  while(target != t->root && target->color == RBTREE_BLACK) { // 삭제 or 이동될 노드 오른쪽 자식 색이 레드거나 타겟이 루트라면 루프 탈출(while은 fixup 조건)
    if (target == target->parent->left) { // 삭제 or 이동된 위치로 갈 노드가 왼쪽인 경우
      // case1 : 형제가 레드일때
      u = target->parent->right; // u는 형제노드
      if (u->color == RBTREE_RED) { // 형제가 레드일때
        u->color = RBTREE_BLACK; // 형제의 색 레드를 블랙으로
        u->parent->color = RBTREE_RED; // 부모는 블랙에서 레드로(형제가 레드면 부모는 레드가 될수 없어) 
        left_rotate(t, target->parent); // 부모 기준으로 왼쪽 회전
        u = target->parent->right; // 새로운 형제가 생김
      }
      // case2 : 형제가 블랙이고 형제의 두 자식이 모두 흑색일 때
      if (u->left->color == RBTREE_BLACK && u->right->color == RBTREE_BLACK) {
        u->color = RBTREE_RED; // 형제만 빨간색으로 만들고 
        target = target->parent; // 부모로 타겟 변경
      }
      // case3, case4 : 형제가 블랙이고 형제 자식들이 서로 다른 색일 때
      else {
        // case3 : 형제 오른쪽 자식이 블랙 왼쪽은 레드
        if (u->right->color == RBTREE_BLACK) {
          u->color = RBTREE_RED; // 형제와
          u->left->color = RBTREE_BLACK; // 형제의 왼쪽 자식색을 바꾼다.
          right_rotate(t, u); // 형제를 기준으로 우회전
          u = target->parent->right; // 새로운 형제
        }
        // case4 : 형제 오른쪽 자식이 레드 왼쪽은 블랙
        u->color = target->parent->color; // 형제를 부모의 색으로 바꾸고
        target->parent->color = RBTREE_BLACK; // 부모를 블랙으로 바꾸고
        u->right->color = RBTREE_BLACK; // 형제의 오른쪽자식도 블랙으로 바꾼다.
        left_rotate(t, target->parent); // 부모를 기준으로 좌회전
        target = t->root; // 노드의 색깔 변경과 좌회전을 통해 여분의 흑색은 노드는 삭제되며 루프는 종료. 
      }
    }
    else { // 오른쪽인 경우 -> 왼쪽과 대칭
      u = target->parent->left;
      if (u->color == RBTREE_RED) {
        u->color = RBTREE_BLACK;
        u->parent->color = RBTREE_RED;
        right_rotate(t, target->parent);
        u = target->parent->left;
      }
      if (u->right->color == RBTREE_BLACK && u->left->color == RBTREE_BLACK) {
        u->color = RBTREE_RED;
        target = target->parent;
      }
      else {
        if (u->left->color == RBTREE_BLACK) {
          u->right->color = RBTREE_BLACK;
          u->color = RBTREE_RED;
          left_rotate(t, u);
          u = target->parent->left;
        }
        u->color = target->parent->color;
        target->parent->color = RBTREE_BLACK;
        u->left->color = RBTREE_BLACK;
        right_rotate(t, target->parent);
        target = t->root;
      }
    }
  }
  target->color = RBTREE_BLACK; // While 종료되면 루트를 블랙으로 색을 바꿔준다.
}


int rbtree_erase(rbtree *t, node_t *p) {
  if (p == NULL) {
    return 0; // 널가드
  }
  node_t *y = p; // 삭제되거나 이동될 노드를 가리킨다, 자노드가 없거나, 삭제되는 노드
  // 둘다 있을 경우엔 y는 이동될 노드를 가르킨다(최소값의 키를 가진 노드)
  node_t *ptr; // y의 원래 위치로 가는 노드, 즉 이동되기전 위치인 노드
  color_t y_original_color = p->color; // y노드의 원래 색상을 저장함. y노드의 색상이 변경될 수 있음

  // case1 : 왼쪽노드가 nil일때
  if (p->left == t->nil) { // 삭제될 노드 왼쪽자식이 닐
    ptr = p->right; // y의 원래 위치는 nil로 설정된다 
    RB_TRANSPLANT(t, p, p->right); // 대체
  }
  // case2 : 오른쪽노드가 nil일때
  else if (p->right == t->nil) {
    ptr = p->left;
    RB_TRANSPLANT(t, p, p->left);
  }
  // case3 : 둘다 일반 노드일때
  else {
    // y는 p.r을 루트로 하는 오른쪽 서브트리에서의 최소 값이며 이동될 노드
    y = node_min(t, p->right); // successor
    // ptr을 y->right로 바꿔주고 color를 위해서 찾은 y의 color로 변경
    y_original_color = y->color;
    
    ptr = y->right; // 원래 위치로 이동하는 노드는 이동될 노드의 오른쪽에

    if (y->parent == p) { // 이동될 노드의 부모가 삭제될 노드라면
      ptr->parent = y; // 원래위치로 이동할 노드의 부모가 이동될 노드다 (fixup 후 y가 nil일 수도 있는데 nil은 가리킬 수 없어서 nil 대신)
    }
    else {
      RB_TRANSPLANT(t, y, y->right); // 대체 
      y->right = p->right; // successor의 오른쪽 자식은 삭제될 노드의 오른쪽 자식이다
      y->right->parent = y;
    }
    RB_TRANSPLANT(t, p, y);
    y->left = p->left;
    p->left->parent = y;
    y->color = p->color;
  }
  if (y_original_color == RBTREE_BLACK) { // 삭제 or 이동 되는 노드가 블랙일때 문제 발생
    RB_ERASE_FIXUP(t, ptr); // 삭제 or 이동 한 노드의 위치로 갈 노드를 기준으로 확인
  }

  free(p);
  p = NULL;

  return 0;
}

int inorder_array(node_t *p, key_t *arr, const rbtree *t, int i) {
  if (p == t->nil) {
    return i;
  }
  i = inorder_array(p->left, arr, t, i);
  arr[i++] = p->key;
  i = inorder_array(p->right, arr, t, i);
  return i;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t *tmp = t->root;
  inorder_array(tmp, arr, t, 0);
  return 0;
}