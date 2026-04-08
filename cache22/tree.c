/*Tree.c*/
#include "tree.h"
Tree root = {
    .n={
    .tag = (TagRoot | TagNode),
    .north = (struct u_node *)&root,
    .west = 0,
    .east = 0,
    .path = "/"
    }};

    int8* indent(int8 n){
        static int8 buf[256];
        int8 *p;
        int16 i;

        if (n < 1 )return (int8*)"";

        // 256 /2 = 128
        assert(n < 120);
        zero(buf,256);

        for (i = 0, p = buf; i < n; i++, p+=2){
            memcpy((char *)p, "  ", 2);
        }
        return buf;
    }

void print_tree(int fd,Tree *_root){
    int8 indentation;
    int8 buf[256];
    int16 size;
    Node *n;
    Leaf *l ;

    indentation = 0;

    for (n = (Node *)_root; n ;n = n ->west){
        Print(indent(indentation++));
        Print(n->path);
        printf("\n");

        if(n->east){
            for (l = n->east; l; l = l ->east ){
            

                Print(indent(indentation));
                Print(n->path);
                Print("/");
                Print(l->key);
                Print(" --> '");
                write(fd,(char*)l->value, (int)l->size);
                Print("'\n");
            }
        }

    }

    return;
    }

    void zero(int8 *str,int16 size){
        int8 *p;
        int16 n;
        for (n = 0, p = str; n<size;p++,n++){
            *p = 0;
        }
    }

    Node *create_node(Node *parent,int8 *path){
        Node *n;
        int16 size;

        errno = NoError;

        assert(parent);

        size = sizeof(struct s_node);

        n = (Node *)malloc((int)size);
        zero((int8 *)n, size);

        parent->west = n;
        n->tag = TagNode;
        n->north = (struct u_node*)parent;
        strncpy((char *)n->path, (char *)path, 254);
        return n;
    }

   

    Leaf *find_last_linear(Node *parent){
        Leaf *l;
        errno = NoError;
        assert(parent);
        if(!parent->east){
            reterr(NoError);
        }
        for (l = parent->east; l->east;l=l->east)
            ;
        assert(l);
        
        return l;
    }

    Leaf *create_leaf(Node *parent,int8 *key,int8 *value,int16 count){
        Leaf *l,*new;
        
        int16 size;

        assert(parent);
        //NODE -- LEAF1 --LEAF2 --LEAF3
        // we need to find the last leaf and if we dont find it wel connect to the last node
        l = find_last(parent);

        size = sizeof(struct s_leaf);
        new = (Leaf *)malloc(size);
        assert(new);


        if(!l){
            // directly connected
            parent->east = new;
        }
        else{
            // l is a leaf
            l->east = new;
        }

        zero((int8 *)new, size);
        new->tag = TagLeaf;

        new->west = (!l)?(Tree *)parent : (Tree *)l;

        strncpy((char *)new->key, (char *)key, 126);

        new->value = (int8 *)malloc(count);
        assert(new->value);
        zero(new->value, count);
        
        strncpy((char *)new->value, (char *)value, count-1);
        new->size = count;
        return new;

    }
    int main(){
        Node *n, *n2;
        Leaf *l1, *l2;
        int8 *key,*value;
        int16 size;

   

        n = create_node((Node *)&root, (int8 *)"/Users");
        // printf("%p\n", (void*)&root);
        assert(n);
        n2 = create_node(n, (int8 *)"/Users/login");
        assert(n2);

        key = (int8 *)"goro";
        value = (int8 *)"abc77301aa";
        size = (int16)strlen((char *)value);

        l1 = create_leaf(n2, key, value, size);
        assert(l1);

        // printf("%s\n", l1->value);

        key = (int8 *)"bato";
        value = (int8 *)"asf46790jkl";
        size = (int16)strlen((char *)value);
        l2 = create_leaf(n2, key, value, size);
        assert(l2);
        printf("%s\n",l2->key);
        
        //printf("%p %p\n", n, n2);

        print_tree(1, &root);

        free(n);
        free(n2);

        return 0;
    }